#include "bullet.h"
#include <QDebug>

#include <sailfishapp.h>

#define MAXX 2.4
#define MAXY 4.2

namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

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
    dynamicsWorld->setDebugDrawer(this);


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

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
    rbInfo.m_friction=0.4;
    rbInfo.m_restitution=0.9;

    btRigidBody* body = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(body);
}

void Bullet::addCube(btVector3 pos)
{

    if (!m_cubeShape) {
        m_cubeShape = new btBoxShape(btVector3(.5,.5,.5));
        collisionShapes.push_back(m_cubeShape);
    }

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(pos);

    btScalar	mass(0.01f);
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        m_cubeShape->calculateLocalInertia(mass,localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,m_cubeShape,localInertia);
    rbInfo.m_friction=0.9;
    rbInfo.m_restitution=0.7;

    btRigidBody* body = new btRigidBody(rbInfo);
    m_cubeMutex.lock();
    m_cubes << body;
    m_cubeMutex.unlock();

    dynamicsWorld->addRigidBody(body);
}


void Bullet::setupModel()
{
    int i;
    qDebug() << "Doing bullet setup";

    ///create a few basic rigid bodies
    this->addWall(btVector3( 0, 0, 1), 0);
    this->addWall(btVector3( 0, 0,-1), -9);
    this->addWall(btVector3( 0, 1, 0), -MAXY);
    this->addWall(btVector3( 0,-1, 0), -MAXY);
    this->addWall(btVector3( 1, 0, 0), -MAXX);
    this->addWall(btVector3(-1, 0, 0), -MAXX);

    {
        //create a dynamic rigidbody
        this->addCube(btVector3(0,0,5));
        this->addCube(btVector3(0, 0.2, 4));
        this->addCube(btVector3(0.6, -0.1, 4));
        this->addCube(btVector3(-0.6, 0.1, 3));
        this->addCube(btVector3(0.1, 0.2, 2));
        this->addCube(btVector3(0,1,5));

    }
}

void Bullet::setNumCubes(int n)
{
    if (m_cubes.size() == n) return;
    while (m_cubes.size() < n)
        this->addCube(btVector3(0,1,5));
    m_cubeMutex.lock();
    while (m_cubes.size() > n) {
        btRigidBody* body = btRigidBody::upcast(m_cubes.takeLast());
        if (body && body->getMotionState()) { delete body->getMotionState(); }
        dynamicsWorld->removeCollisionObject(body);
        delete body;
    }
    m_cubeMutex.unlock();
}

void Bullet::runStep(int ms)
{
    m_cubeMutex.lock();
    dynamicsWorld->stepSimulation(ms/1000.f, 10, 1.f/300.f);
    m_cubeMutex.unlock();
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

void Bullet::render(GLProgram *p, QMatrix4x4 projViewMatrix)
{
    glEnableVertexAttribArray(p->getA("posA"));
    glEnableVertexAttribArray(p->getA("texA"));
    glEnableVertexAttribArray(p->getA("normalA"));

    m_cubeMutex.lock();
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
    m_cubeMutex.unlock();
    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));

    //  Setup shader for debug draw
    QMatrix4x4 worldMatrix; // null atm
    m_program_debug->bind();
    m_program_debug->setUniformValue(m_program_debug->getU("projViewMatrixU"), projViewMatrix);
    m_program_debug->setUniformValue(m_program_debug->getU("worldMatrixU"), worldMatrix);
    glEnableVertexAttribArray(m_program_debug->getA("posA"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    dynamicsWorld->debugDrawWorld();
    glDisableVertexAttribArray(m_program_debug->getA("posA"));
    //    p->bind(); // should do this but we're only going to drop it again

}

void Bullet::setGravity(qreal x, qreal y, qreal z) {
    dynamicsWorld->setGravity(btVector3(-x*20, -y*20, -z*20));
    //    dynamicsWorld->setGravity(btVector3(0, 0, z/10.0));
}

void Bullet::kick(){
    m_cubeMutex.lock();
    for (m_cubes_i = m_cubes.begin(); m_cubes_i != m_cubes.end(); ++m_cubes_i) {
        btRigidBody* body = btRigidBody::upcast(*m_cubes_i);
        if (body && body->getMotionState()) {
            body->applyCentralImpulse(btVector3(1.0-rnd(0.5),
                                                1.0-rnd(0.5),
                                                1.0-rnd(0.5)));
        }
    }
    m_cubeMutex.unlock();
}

void Bullet::setup()
{
    m_program_debug = new GLProgram();
    m_program_debug->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             SailfishApp::pathTo("debug_vert.glsl").toLocalFile());
    m_program_debug->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             SailfishApp::pathTo("debug_frag.glsl").toLocalFile());
    if (! m_program_debug->link()) {
        QList<QOpenGLShader *>::iterator i;
        for (i = m_program_debug->shaders().begin(); i != m_program_debug->shaders().end(); ++i) {
            if ((*i)->isCompiled())
                qDebug() << "Shader compile log: \n" << (*i)->log();
        }

    }

    setDebugMode(DBG_DrawWireframe);
}

void Bullet::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    glLineWidth(5);

    QVector3D line[] = { QVector3D(from.x(), from.y(), from.z()), QVector3D(to.x(), to.y(),to.z()) };
//    m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(1,1,1, 1));
    m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(color.x(),color.y(),color.z(), 1));
    glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, line);
    glDrawArrays(GL_LINES, 0, 2);
//    qDebug()<< "drawLine() " << line[0] << " to " << line[1];
}
void	Bullet::reportErrorWarning(const char* warningString) {
    qDebug()<< warningString;
}

void	Bullet::draw3dText(const btVector3& location,const char* textString) {
    qDebug()<< "draw3dText " << textString;
}
void	Bullet::setDebugMode(int debugMode) {
    m_debug_mode = debugMode;
    qDebug()<< "setDebugMode to " << debugMode;
}
int		Bullet::getDebugMode() const {
    return m_debug_mode;
}
void	Bullet::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color){
    qDebug()<< "drawContactPoint()";
}
