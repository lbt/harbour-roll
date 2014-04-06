#include "world.h"

World::World(QObject *parent) :
    QObject(parent)
  , m_worldMutex(QMutex::Recursive)
  , m_debugDrawer(this)
{
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

World::~World()
{
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

void World::render()
{
    m_worldMutex.lock();
//    for (auto wobj : m_worldObjects) { // ideally group by shader
//        wobj->render();
//    }
    m_worldMutex.unlock();
}

// Called by the item
// Store by item Name and by all item->Renderable[]->shader
void World::add(WorldItem* i){
    m_worldMutex.lock();
    m_world[i->objectName()] = i;
    for (Renderable* r: i->m_renderables) {
        m_byShader[r->getShader()] = i;
    }
    m_worldMutex.unlock();
}
void World::add(Physics* i){
    dynamicsWorld->addRigidBody(i->getRigidBody());
}
