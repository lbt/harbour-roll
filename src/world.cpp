#include "world.h"

World::World(QObject *parent) :
    QObject(parent)
  , m_worldMutex(QMutex::Recursive)
  , m_debugDrawer(this)
{
    setupPhysicsWorld();
    createRunner();
    m_runner->moveToThread(&m_runnerThread);
    connect(&m_runnerThread, &QThread::finished, m_runner, &QObject::deleteLater);
    connect(&m_runnerThread, &QThread::started, m_runner, &WorldRunner::setup);
    connect(m_runner, SIGNAL(stepReady()), this, SIGNAL(stepReady()) );

}

World::~World()
{
    destroyPhysicsWorld();
}
void World::createRunner() {
    // Setup a worker Thread to do the bullet calcs
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
    for (auto wi : m_worlditems) {
        if (wi->physics()) {
            wi->physics()->getRigidBody()->activate();
        }
    }
    for (auto l : getLights()) {
        l->update(ms);
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
    for (auto k : m_worlditems.keys()){
        qDebug() << "GL for " << k;
        m_worlditems[k]->setupGL();
    }
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
        s->renderPrep(); // bind and setup Lights (maybe do that here?)
        for (WorldItem* wi : m_byShader[s]) {
            wi->render(s);
        }
    }
    m_worldMutex.unlock();

    if (m_debugDrawer.getDebugMode() != 0) {
        m_debugShader->renderPrep();
        for (Light* l : getLights()) {
            l->debugRender(getActiveCameraPVM());
        }

    }
}

QList<Light *> World::getLights()
{
    return m_lights.values();
}

QMatrix4x4 World::getActiveCameraPVM()
{

}

QVector3D World::getActiveCameraAt()
{

}
// Called by the item
// Store by item Name and by all item->Renderable[]->shader
void World::add(WorldItem* i){
    m_worldMutex.lock();
    m_worlditems[i->objectName()] = i;
    for (Renderable* r: i->m_renderables) {
        m_byShader[r->getShader()] << i;
    }
    m_worldMutex.unlock();
}

void World::add(Physics* p){
    m_worldMutex.lock();
    dynamicsWorld->addRigidBody(p->getRigidBody());
    m_worldMutex.unlock();
}

void World::add(QString name, Light* l){
    m_worldMutex.lock();
    if (m_lights.contains(name)) {
        qDebug() <<"Existing light " << name;
    }
    m_lights[name] = l;
    m_worldMutex.unlock();
}

