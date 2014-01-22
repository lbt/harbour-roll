#include "bullet.h"
#include <QDebug>

#define MAXX 2.4
#define MAXY 4.2

Bullet::Bullet(QObject *parent) :
    QObject(parent)
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new	btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0,0,0));

}

Bullet::~Bullet()
{
    qDebug() << "Doing bullet cleanup";
    //cleanup in the reverse order of creation/initialization
    int i;

    //remove the rigidbodies from the dynamics world and delete them
    for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete collision shapes
    for (int j=0;j<collisionShapes.size();j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
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

    //next line is optional: it will be cleared by the destructor when the array goes out of scope
    collisionShapes.clear();
    qDebug() << "Done bullet cleanup";

}


void Bullet::addWall(btVector3 normal, float offset) {
    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0,0,0));  // position the groundbox at origin

    btScalar	mass(0.f);
    btVector3 localInertia(0,0,0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);

    btCollisionShape* groundShape = new btStaticPlaneShape(normal, offset);
    collisionShapes.push_back(groundShape);
    btRigidBody* body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
                                            mass, myMotionState, groundShape, localInertia));
    body->setRestitution(1.0);
    dynamicsWorld->addRigidBody(body);
}

void Bullet::addCube(btVector3 pos, btCollisionShape *shape)
{
    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(pos);

    btScalar	mass(0.01f);
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        shape->calculateLocalInertia(mass,localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
    rbInfo.m_friction=0.9;
    rbInfo.m_restitution=0.7;

    btRigidBody* body = new btRigidBody(rbInfo);
    m_cubes << body;

    dynamicsWorld->addRigidBody(body);
}

void Bullet::setupModel()
{
    int i;
    qDebug() << "Doing bullet setup";

    ///create a few basic rigid bodies
    this->addWall(btVector3( 0, 0, 1), 0);
    this->addWall(btVector3( 0, 0,-1), -50);
    this->addWall(btVector3( 0, 1, 0), -MAXY);
    this->addWall(btVector3( 0,-1, 0), -MAXY);
    this->addWall(btVector3( 1, 0, 0), -MAXX);
    this->addWall(btVector3(-1, 0, 0), -MAXX);

    {
        //create a dynamic rigidbody
        btCollisionShape* colShape = new btBoxShape(btVector3(.5,.5,.5));
        collisionShapes.push_back(colShape);

        this->addCube(btVector3(0,0,5), colShape);
        this->addCube(btVector3(0, 0.2, 4), colShape);
        this->addCube(btVector3(0.6, -0.1, 4), colShape);
        this->addCube(btVector3(-0.6, 0.1, 3), colShape);
        this->addCube(btVector3(0.1, 0.2, 2), colShape);
        this->addCube(btVector3(0,1,5), colShape);

    }
}

void Bullet::runStep(int ms)
{
    dynamicsWorld->stepSimulation(ms/100.f, 10, 1.f/300.f);
}

void Bullet::report()
{
    //print positions of all objects
    for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            //            printf("world pos = %f,%f,%f\n",float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
        }
    }
}

void Bullet::renderWalls(GLProgram *p)
{

}

QMatrix4x4 transform2Matrix(btTransform *transform) {
    float ft[16];
    transform->getOpenGLMatrix(ft);

    return QMatrix4x4(ft[0], ft[1], ft[2], ft[3],
            ft[4],  ft[5],  ft[6],  ft[7],
            ft[8],  ft[9],  ft[10], ft[11],
            ft[12], ft[13], ft[14], ft[15]).transposed();
}

void Bullet::renderCubes(GLProgram *p)
{
    for (m_cubes_i = m_cubes.begin(); m_cubes_i != m_cubes.end(); ++m_cubes_i) {
        btRigidBody* body = btRigidBody::upcast(*m_cubes_i);
        if (body && body->getMotionState())
        {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            //            printf("world pos (%f,%f,%f)\n",float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
            QMatrix4x4  world;
            world.scale(0.5, 0.5, 0.5);
            QMatrix4x4  pos = transform2Matrix(&trans);
            p->setUniformValue(p->getU("worldMatrixU"), pos*world);
            glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
            body->activate();
        }
    }

}

void Bullet::setGravity(qreal x, qreal y, qreal z) {
    dynamicsWorld->setGravity(btVector3(-x*5, -y*5, -z*5));
    //    dynamicsWorld->setGravity(btVector3(0, 0, z/10.0));
}

float rnd(float max) {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max);
}
void Bullet::kick(){
    for (m_cubes_i = m_cubes.begin(); m_cubes_i != m_cubes.end(); ++m_cubes_i) {
        btRigidBody* body = btRigidBody::upcast(*m_cubes_i);
        if (body && body->getMotionState()) {
            body->applyCentralImpulse(btVector3(1.0-rnd(0.5),
                                                1.0-rnd(0.5),
                                                1.0-rnd(0.5)));
        }
    }
}
