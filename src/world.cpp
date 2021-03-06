#include "world.h"

static void WorldTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    World *w = static_cast<World *>(world->getWorldUserInfo());
    w->btTickCallback(timeStep);
}

World::World(QObject *parent) :
    QObject(parent)
  , m_debugDrawer(this)
  , m_debugShader(NULL)
  , m_worldMutex(QMutex::Recursive)
  , m_runner(NULL)
  , m_activeCamera(NULL)
{
}

World::~World()
{
    this->destroyPhysicsWorld();
}

// This must be called outside the constructor to allow virtual setupXXX()
// to be used
void World::setup() {
    this->setupPhysicsWorld();
    qDebug() << "Making a Runner";
    this->createRunner();

    m_runner->moveToThread(&m_runnerThread);
    connect(&m_runnerThread, &QThread::finished, m_runner, &QObject::deleteLater);
    connect(&m_runnerThread, &QThread::started, m_runner, &WorldRunner::setup);
    connect(m_runner, SIGNAL(stepReady()), this, SIGNAL(stepReady()) );
}

void World::createRunner() {
    // Setup a worker Thread to do the bullet calcs
    qDebug() << "Making a WorldRunner";
    m_runner = new WorldRunner(this);
}

void World::setupPhysicsWorld() {
    // Setup the bullet physics world
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new	btCollisionDispatcher(collisionConfiguration);
    // Register the GImpactCollisionAlgorithm to detect inter-GImpactMesh collisions
    // btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,0,0));
    dynamicsWorld->setDebugDrawer(&m_debugDrawer);

    // To be used by WorldTickCallback to call 'this'
    dynamicsWorld->setInternalTickCallback(WorldTickCallback, static_cast<void *>(this));

}

void World::destroyPhysicsWorld() {
    qDebug() << "Doing bullet cleanup";
    //cleanup in the reverse order of creation/initialization
    int i;

    //remove the *remaining* rigidbodies from the dynamics world and delete them
    for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)  {
        qDebug() <<"Removing an unmanaged CollisionObject";
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) { delete body->getMotionState(); }
        dynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete dynamics world
    delete dynamicsWorld;
    //delete solver
    delete solver;
    //delete broadphase
    delete overlappingPairCache;
    //delete dispatcher
    delete dispatcher;
    delete collisionConfiguration;

    qDebug() << "Done bullet cleanup";

}

void World::start() {
    m_runnerThread.start();
}

void World::setRunning(bool running)
{
    QMetaObject::invokeMethod(m_runner, "setRunning", Qt::QueuedConnection, Q_ARG(bool, running));
}

QString World::serialise() {
    QString state;
    m_worldMutex.lock();
    //    for (auto wobj : m_worldObjects) {
    //        state += wobj->getBiMesh()->name() + ",";
    //    }
    m_worldMutex.unlock();
    //    qDebug() << "Saved state " << state;
    return state;
}

void World::restore(QString state) {
    qDebug() << "Restore state " << state;
    m_worldMutex.lock();
    //    for (auto wobj : m_worldObjects) {
    //        removeObject(wobj);
    //    }
    //    for (auto die : state.split(",")) {
    ////        qDebug() << "Adding " << die;
    //        addRoll(die);
    //    }
    m_worldMutex.unlock();
}

////////////////////////////////////////////////////
/// \brief World::runStep
/// \param ms
/// This method is invoked by the WorldRunner in its own thread
void World::runStep(int ms)
{
    m_worldMutex.lock();

    dynamicsWorld->stepSimulation(ms/1000.f, 10, 1.f/300.f);

    // At this point the per-substep btTickCallback() is called and any collisions
    // are handled

    if (m_debugDrawer.getDebugMode() != WorldDebugDrawer::DBG_NoDebug )
    {
        m_debugDrawer.newFrame();
        dynamicsWorld->debugDrawWorld();
    }
    // Ensure all objects are permanently activated
    for (WorldItem* wi : m_worlditems) {
        if (wi->physicsMotion()) {
            wi->physicsMotion()->getRigidBody()->activate();
        }
        // This will break if/when Motions are shared
        wi->motion()->runStep(ms);
    }
    for (WorldItem* wi : m_worlditems) {
        wi->updateTransform();
    }

    // WTF GL?
    // http://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-10-transparency/
    QVector3D camPos = m_activeCamera->at();
    m_WIByDistance.clear();

    for (WorldItem* wi : m_worlditems) {
        qreal dist = (camPos - wi->getTransform().at()).lengthSquared();
        //qDebug() << "Distance:" << dist << " " << wi->objectName();
        m_WIByDistance.insert(dist, wi);
    }

    m_worldMutex.unlock();
}

void World::btTickCallback(btScalar timestep)
{
    Q_UNUSED(timestep);
    // This approach to collision detection looks at all pre-calculated
    // contact manifolds in the physics step
    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i=0;i<numManifolds;i++)
    {
        btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
        const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

        // *possible* that same pair may exist in another manifold - hope for the best :)
        WorldItem* wiA = static_cast<WorldItem*>(obA->getUserPointer());
        WorldItem* wiB = static_cast<WorldItem*>(obB->getUserPointer());
        if ((wiA->getCollisionType() == WorldItem::NO_COLLISONS) ||
                (wiB->getCollisionType() == WorldItem::NO_COLLISONS))
            continue;
        int numContacts = contactManifold->getNumContacts();
        for (int j=0;j<numContacts;j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance()<=0.f)
            {
                Collision c(wiA, wiB, pt);
                if (wiA->getCollisionType() == WorldItem::ITEM_COLLISONS)
                    wiA->collision(c);
                if (wiB->getCollisionType() == WorldItem::ITEM_COLLISONS)
                    wiB->collision(c);
                handleCollision(c);
            }
        }
    }

}

////////////////////////////////////////////////
/// \brief World::setupGL
/// Call this when there's an active GL context
///
/// called from the render thread

void World::setupGL(){
    qDebug() << "Setup GL";
    setupNewGL(); // setup all WIs and corresponding shaders
    m_debugShader->setupGL();
    m_debugDrawer.setupGL();
    qDebug() << "Setup GL done";
}

void World::setupNewGL(){
    if (m_WIneedGLSetup.isEmpty()) return;

    qDebug() << "Setup NewGL";
    m_worldMutex.lock();
    for (WorldItem* wi : m_WIneedGLSetup) {
        qDebug() << "setupGL for WI " << wi->objectName();
        wi->setupGL();
    }
    m_WIneedGLSetup.clear();
    m_worldMutex.unlock();
    qDebug() << "Setup GL done";
}
/////////////////////////////////////////////////
/// \brief World::render
///
/// Iterate through the Worlditems by shader
/// Setup each shader (they pull Camera and light from the world - and anything else they may need)
/// Render relevant WorldItems
///
/// called from the render thread
void World::render()
{
    m_worldMutex.lock();

    // Prepare to do an OPAQUE render pass
    // Depth is coarsley ordered by Transform of the item, not triangles!

    // We want to store depth information in this pass and no need to blend
    glDepthMask(true);
    glDisable(GL_BLEND);
    Shader* activeShader = NULL;
    for (WorldItem* wi : m_WIByDistance.values()){
        // qDebug() << "Render by distance:" << wi->objectName();
        for (Shader* s: wi->shaderList()){ // Only swap shaders if we must
            if (activeShader != s){
                s->renderPrep();
                activeShader = s;
            }
            wi->render(s, WorldItem::OPAQUE);
        }
    }

    // Prepare to do a TRANSLUCENT render pass
    // reverse depth order if it matters
    glDepthMask(false);
    glEnable(GL_BLEND);
    QListIterator<WorldItem*> it(m_WIByDistance.values());
    it.toBack();
    while (it.hasPrevious()) {
        WorldItem* wi = it.previous();
        // qDebug() << "Render by reverse distance:" << wi->objectName();
        for (Shader* s: wi->shaderList()){
            if (activeShader != s){
                s->renderPrep();
                activeShader = s;
            }
            wi->render(s, WorldItem::TRANSLUCENT);
        }
    }

    // Now do any debug. Later introduce finer control
    if (m_debugDrawer.getDebugMode() != 0) {
        m_debugShader->renderPrep();
        for (WorldItem* wi : m_worlditems) {
            wi->debugRender(getActiveCameraPVM());
        }
        m_debugDrawer.render(getActiveCameraPVM());
    }
    m_worldMutex.unlock();
}


QList<Light *> World::getLights()
{
    return m_lights.values();
}

QMatrix4x4 World::getActiveCameraPVM()
{
    if (m_activeCamera)
        return m_activeCamera->projViewMatrix();
    else
        return QMatrix4x4();
}

QVector3D World::getActiveCameraAt()
{
    if (m_activeCamera)
        return m_activeCamera->at();
    else
        return QVector3D(0,0,5);
}

void World::setActiveCamera(QString name)
{
    if (m_cameras.contains(name))
        m_activeCamera = m_cameras[name];
}

// Called by the item
// Store by item Name and by all item->Renderable[]->shader
// All other bits of the WI are added by the WorldItem but the m_byShader is more
// local to world and the majority of this is about world containers
void World::add(WorldItem* item, QList<Shader*> shaderList){
    m_worldMutex.lock();
    if (m_worlditems.contains(item->objectName())) {
        qDebug() <<"Existing Item " << item->objectName();
    } else {
        qDebug() <<"Adding Item " << item->objectName();
        m_worlditems[item->objectName()] = item;
        for (Shader* s : shaderList) m_byShader[s] << item;
    }
    // Mark this item as needing GLSetup
    m_WIneedGLSetup << item;
    m_worldMutex.unlock();
}

WorldItem *World::getItem(QString name)
{
    if (m_worlditems.contains(name)) {
        return m_worlditems.value(name);
    }
    return NULL;
}

void World::remove(WorldItem* item, QList<Shader*> shaderList){
    m_worldMutex.lock();
    m_worlditems.remove(item->objectName());
    qreal dist = m_WIByDistance.key(item); // could be a 'default constructed key'
    if (m_WIByDistance.contains(dist, item)) // so check before removing
        m_WIByDistance.remove(dist, item);
    // Extremely unlikely that a WI is removed before GLSetup
    // but remove() is rare so no harm in being cautious
    if (m_WIneedGLSetup.contains(item))
        m_WIneedGLSetup.remove(item);
    for (Shader* s : shaderList) m_byShader[s].remove(item);
    m_worldMutex.unlock();
}

void World::add(CameraManager* camera){
    m_worldMutex.lock();
    if (m_cameras.contains(camera->objectName())) {
        qDebug() <<"Existing camera " << camera->objectName();
    } else
        m_cameras[camera->objectName()] = camera;
    m_worldMutex.unlock();
}

void World::remove(CameraManager* camera){
    m_worldMutex.lock();
    m_cameras.remove(camera->objectName());
    m_worldMutex.unlock();
}

CameraManager*  World::getCamera(QString name){
    if (m_cameras.contains(name)) return m_cameras[name];
    return NULL;
}

void World::add(Light* light){
    m_worldMutex.lock();
    if (m_lights.contains(light->objectName())) {
        qDebug() <<"Existing light " << light->objectName();
    } else
        m_lights[light->objectName()] = light;
    m_worldMutex.unlock();
}

void World::remove(Light* light){
    m_worldMutex.lock();
    m_lights.remove(light->objectName());
    m_worldMutex.unlock();
}

Light*  World::getLight(QString name){
    if (m_lights.contains(name)) return m_lights[name];
    return NULL;
}
