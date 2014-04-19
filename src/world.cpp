#include "world.h"

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
    if (m_debugDrawer.getDebugMode() != WorldDebugDrawer::DBG_NoDebug )
    {
        m_debugDrawer.newFrame();
        dynamicsWorld->debugDrawWorld();
    }
    // Ensure all objects are permanently activated
    for (WorldItem* wi : m_worlditems) {
        if (wi->physics()) {
            wi->physics()->getRigidBody()->activate();
        }
        wi->motion()->runStep(ms);
    }

    m_worldMutex.unlock();
}

////////////////////////////////////////////////
/// \brief World::setupGL
/// Call this when there's an active GL context
///
/// called from the render thread

void World::setupGL(){
    qDebug() << "Setup GL";
    m_worldMutex.lock();
    for (Shader* s : m_byShader.keys()) {
        qDebug() << "setupGL for shader " << s;
        s->setupGL(); // bind and setup Lights (maybe do that here?)
        for (WorldItem* wi : m_byShader[s]) {
            qDebug() << "setupGL for WI " << wi->objectName();
            wi->setupGL();
        }
    }
    m_worldMutex.unlock();
    m_debugShader->setupGL();
    m_debugDrawer.setupGL();
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

    for (Shader* s : m_byShader.keys()) {
        //        qDebug() << "renderPrep for shader " << s;
        s->renderPrep(); // bind and setup Lights (maybe do that here?)
        for (WorldItem* wi : m_byShader[s]) {
            //            qDebug() << "render for WI " << wi->objectName();
            wi->render(s);
        }
    }

    if (m_debugDrawer.getDebugMode() != 0) {
        m_debugShader->renderPrep();
        for (Light* l : getLights()) {
            l->debugRender(getActiveCameraPVM());
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
    m_worlditems[item->objectName()] = item;
    for (Shader* s : shaderList) m_byShader[s] << item;
    m_worldMutex.unlock();
}

void World::remove(WorldItem* item, QList<Shader*> shaderList){
    m_worldMutex.lock();
    m_worlditems.remove(item->objectName());
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

