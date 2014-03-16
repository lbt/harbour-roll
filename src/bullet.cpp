#include "bullet.h"
#include <QDebug>
#include <QFile>
#include <QVector>

#include <sailfishapp.h>

#include <bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#define MAXX 2.4
#define MAXY 4.2

namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

uint qHash(Bullet::Color c) {
    return c.m_c.x()*10000 + c.m_c.y()*100 + c.m_c.z();
}
inline bool operator==(const Bullet::Color &c1, const Bullet::Color &c2) { return c1.m_c == c2.m_c; }

QMatrix4x4 bt2QMatrix4x4(btTransform *transform) {
    float ft[16];
    transform->getOpenGLMatrix(ft);

    return QMatrix4x4(ft[0], ft[1], ft[2], ft[3],
            ft[4],  ft[5],  ft[6],  ft[7],
            ft[8],  ft[9],  ft[10], ft[11],
            ft[12], ft[13], ft[14], ft[15]).transposed();
}

QVector3D bt2QtVector3D(const btVector3 &bv) {
    return QVector3D(bv.x(), bv.y(), bv.z());
}
btVector3 Q2btVector3 (const QVector3D  &qv) {
    return btVector3(qv.x(), qv.y(), qv.z());
}


Bullet::Bullet(QObject *parent) :
    QObject(parent)
  , m_qlinepoints(500)
  , m_touchRayActive(false)
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new	btCollisionDispatcher(collisionConfiguration);
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
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
    body->setUserPointer((void*)new QString("wall"));
    dynamicsWorld->addRigidBody(body);
}

const QList<QString> Bullet::getNames() const {
    if (m_meshes) {
        return m_meshes ->getNames();
    } else {
        QList<QString> a;
        return a;
    }
}


//void Bullet::loadDice(QString die, QString filename)
void Bullet::loadDice()
{
    m_meshes = new BiMeshContainer();
    m_meshes->load(SailfishApp::pathTo("dice.obj").toLocalFile());

    //    if (!m_diceShape[die]) {
    //        m_diceShape[die] = new btBoxShape(btVector3(.5,.5,.5));
    //        collisionShapes.push_back(m_diceShape[die]);
    //    }
}

void Bullet::addDice(QString die, btVector3 pos)
{
    if (m_cubes.size() >= 20) {
        qDebug() << "Too many dice";
        // emit tooManyDice();
        return;
    }

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(pos);

    btScalar	mass(0.01f);
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);
    qDebug() << "add dice";

    btVector3 localInertia(0,0,0);

    // Typeinfo AABB bug???
    btGImpactMeshShape* shape = (btGImpactMeshShape*) m_meshes->getCollisionMesh(die);

    if (isDynamic)
        shape->calculateLocalInertia(mass,localInertia);
    qDebug() << "add dice " << die;
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
    rbInfo.m_friction=0.9;
    rbInfo.m_restitution=0.7;

    btRigidBody* body = new btRigidBody(rbInfo);
    m_cubeMutex.lock();
    m_cubes << body;
    m_cubeMutex.unlock();

    body->setUserPointer((void*)new QString(die));

    dynamicsWorld->addRigidBody(body);
    emit numDiceChanged(m_cubes.size());
    qDebug() << "add dice";
}


void Bullet::setupModel()
{
    qDebug() << "Doing bullet setup";
    loadDice();

    ///create a few basic rigid bodies
    this->addWall(btVector3( 0, 0, 1), 0);
    this->addWall(btVector3( 0, 0,-1), -9); // offset -9 from the normal - so location is z=10
    this->addWall(btVector3( 0, 1, 0), -MAXY);
    this->addWall(btVector3( 0,-1, 0), -MAXY);
    this->addWall(btVector3( 1, 0, 0), -MAXX);
    this->addWall(btVector3(-1, 0, 0), -MAXX);

    QList<QString> names = m_meshes->getNames();
    for (int i=0; i++<6;)  {
        this->addDice(names[rand()%names.length()], btVector3(0,1,5));
    }
}

void Bullet::setNumDice(int n)
{
    if (! m_meshes)
        return;
    QList<QString> names = m_meshes->getNames();
    if (m_cubes.size() == n) return;
    while (m_cubes.size() < n)
        this->addDice( names[rand()%names.length()], btVector3(0,1,5));
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
    if (m_debug_mode != DBG_NoDebug )
    {
        //        qDebug() << "Drawing world ------------------------------------------------------";
        if (m_worldLines)
            delete m_worldLines;
        m_worldLines = new QHash<Color, QList<Line> >;
        dynamicsWorld->debugDrawWorld();
    }
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

void Bullet::render(GLProgram *p, QMatrix4x4 projViewMatrix)
{
    m_cubeMutex.lock();
    for (m_cubes_i = m_cubes.begin(); m_cubes_i != m_cubes.end(); ++m_cubes_i) {
        btRigidBody* body = btRigidBody::upcast(*m_cubes_i);
        if (body && body->getMotionState())
        {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            QMatrix4x4  pos = bt2QMatrix4x4(&trans);
            p->setUniformValue(p->getU("worldMatrixU"), pos);

            //            if (m_debug_mode == DBG_NoDebug) {
            BiMesh* bimesh = dynamic_cast<BiMesh *>(body->getCollisionShape());
            bimesh->render(p);
            //            }
            body->activate();
        }
    }
    m_cubeMutex.unlock();

    if (m_touchRayActive) {
        //  Setup shader for debug draw
        qDebug() << "Lasers!!!" ;
        QMatrix4x4 worldMatrix; // null atm
        m_program_debug->bind();
        m_program_debug->setUniformValue(m_program_debug->getU("projViewMatrixU"), projViewMatrix);
        m_program_debug->setUniformValue(m_program_debug->getU("worldMatrixU"), worldMatrix);
        glEnableVertexAttribArray(m_program_debug->getA("posA"));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glLineWidth(8);
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(1.0,0.0,0.0,1.0));
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_touchRay+6);
        glDrawArrays(GL_LINES, 0, 8);
        glLineWidth(2);
        glDisableVertexAttribArray(m_program_debug->getA("posA"));
    }

    if (m_debug_mode == DBG_NoDebug ) return;

    if (! m_worldLines) return;

    //  Setup shader for debug draw
    QMatrix4x4 worldMatrix; // null atm
    m_program_debug->bind();
    m_program_debug->setUniformValue(m_program_debug->getU("projViewMatrixU"), projViewMatrix);
    m_program_debug->setUniformValue(m_program_debug->getU("worldMatrixU"), worldMatrix);
    glEnableVertexAttribArray(m_program_debug->getA("posA"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // iterate over worldlines
    glLineWidth(2);
    m_cubeMutex.lock();
    auto i = m_worldLines->constBegin();
    while (i != m_worldLines->constEnd()) {
        Color color = i.key();
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), color.m_c);
        auto list = i++.value();
        //        qDebug() << "Drawing color " << color.m_c << " has " << list.size() << " entries, list has size " << m_qlinepoints.size();
        if (list.size()*2 > m_qlinepoints.size()) {
            m_qlinepoints.resize(list.size()*2);
            //            qDebug() <<"resized to " << m_qlinepoints.size();
        }
        auto j = list.constBegin();
        int n=0;
        while (j != list.constEnd()) {
            Line line = *j++;
            m_qlinepoints[n++] = line.from;
            m_qlinepoints[n++] = line.to;
            //            QVector3D lqline[] = {line.from, line.to};
            //            glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, lqline);
            //            glDrawArrays(GL_LINES, 0, 2);
            //            qDebug() << "Rendering line "<< n << " from " << line.from << " to " << line.to;
        }
        //        qDebug() << "Stored " << n << " vertices";
        //        int n2 =0;
        //        do {
        //            QVector3D lqline[] = {m_qlinepoints[n2], m_qlinepoints[n2+1]};
        //            glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, lqline);
        //            glDrawArrays(GL_LINES, 0, 2);
        //            n2++; n2++;
        //        } while (n2 < n);
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_qlinepoints.data());
        glDrawArrays(GL_LINES, 0, n);
        //        qDebug() << "Drew " << n2 << " vertices";

    }
    m_cubeMutex.unlock();

    if (m_touchRayActive) {
        //  Setup shader for debug draw
        QMatrix4x4 worldMatrix; // null atm
        m_program_debug->bind();
        m_program_debug->setUniformValue(m_program_debug->getU("projViewMatrixU"), projViewMatrix);
        m_program_debug->setUniformValue(m_program_debug->getU("worldMatrixU"), worldMatrix);
        glEnableVertexAttribArray(m_program_debug->getA("posA"));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glLineWidth(8);
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(1.0,0.0,0.0,1.0));
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_touchRay);
        glDrawArrays(GL_LINES, 0, 2);
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(0.0,1.0,0.0,1.0));
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_touchRay+2);
        glDrawArrays(GL_LINES, 0, 2);
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), QVector4D(0.0,0.0,1.0,1.0));
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_touchRay+4);
        glDrawArrays(GL_LINES, 0, 2);
        glLineWidth(2);
    }

    glDisableVertexAttribArray(m_program_debug->getA("posA"));
    //    p->bind(); // should do this but we're only going to drop it again

}

void Bullet::setGravity(qreal x, qreal y, qreal z) {
    dynamicsWorld->setGravity(btVector3(-x*20, -y*20, -z*20));
    //    dynamicsWorld->setGravity(btVector3(0, 0, z/10.0));
}

////////
/// \brief Bullet::touch
/// \param x Normalised device coords [-1,1]
/// \param y
/// \param projViewMatrix
/// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
///
void Bullet::touch(float x, float y, QMatrix4x4 projViewMatrix, QVector3D lookingToward){
    m_touchRayActive = true;
    qDebug() <<"touched ("<< x <<","<< y <<")";

    qDebug() <<"projViewMatrix "<< projViewMatrix;
    qDebug() <<"lookingToward "<< lookingToward;

    QMatrix4x4 pvInverse = projViewMatrix.inverted();
    qDebug() <<"inverse " << pvInverse;
    // Create start/end in the world space based on the position on the near/far planes
    // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glDepthRangef.xml
    // says: After clipping and division by w, depth coordinates range from -1 to 1,
    // corresponding to the near and far clipping planes.
    QVector4D start = pvInverse * QVector4D(x, y, -1.0, 1.0);
    qDebug() <<"start " << start;
    start /= start.w();
    //    qDebug() <<"start " << start;
    QVector4D end = pvInverse * QVector4D(x, y, 1.0, 1.0);
    qDebug() <<"end " << end;
    end /= end.w();
    //    qDebug() <<"end " << end;
    //    QVector3D end = start - (lookingToward * 20.0);

    m_touchRay[0] = start.toVector3D();
    m_touchRay[1] = end.toVector3D();
    m_touchRay[2] = QVector3D(0.0, 0.0, 0.0);
    m_touchRay[3] = start.toVector3D();
    m_touchRay[4] = QVector3D(0.0, 0.0, 0.0);
    m_touchRay[5] = end.toVector3D();
    m_touchRay[6] = start.toVector3D();

    // Some laser rays!
    int c=6; QVector4D corner;
    corner = (pvInverse * QVector4D(-1, 1, -1.0, 1.0)); corner /= corner.w();
    m_touchRay[c++] = corner.toVector3D(); m_touchRay[c++] = start.toVector3D();
    corner = (pvInverse * QVector4D(1, 1, -1.0, 1.0)); corner /= corner.w();
    m_touchRay[c++] = corner.toVector3D(); m_touchRay[c++] = start.toVector3D();
    corner = (pvInverse * QVector4D(1, -1, -1.0, 1.0)); corner /= corner.w();
    m_touchRay[c++] = corner.toVector3D(); m_touchRay[c++] = start.toVector3D();
    corner = (pvInverse * QVector4D(-1, -1, -1.0, 1.0)); corner /= corner.w();
    m_touchRay[c++] = corner.toVector3D(); m_touchRay[c++] = start.toVector3D();

    btVector3 bstart = Q2btVector3(start.toVector3D());
    btVector3 bend = Q2btVector3(end.toVector3D());

    btCollisionWorld::AllHitsRayResultCallback RayResults(bstart, bend);
    dynamicsWorld->rayTest(bstart, bend, RayResults );

    if (RayResults.hasHit()) {
        qDebug() << "Hit ";
        // const BiMesh* bimesh = dynamic_cast<const BiMesh *>(RayResults.m_collisionObject); // This will fail when it hits a non-bimesh (like a wall!)
        m_cubeMutex.lock();
        for (int i = 0; i< RayResults.m_collisionObjects.size(); i++) {
            QString *name = (QString*)(RayResults.m_collisionObjects[i]->getUserPointer());
            qDebug() << "Hit a " << *name;
            if (name->startsWith("d")) {
                // cast away the const (I think this makes sense)
                btRigidBody* body = (btRigidBody*)btRigidBody::upcast(RayResults.m_collisionObjects[i]);
                if (m_cubes.removeOne(body)) {
                    qDebug() << "Deleting a " << *name;
                    if (body && body->getMotionState()) { delete body->getMotionState(); }
                    dynamicsWorld->removeCollisionObject(body);
                    delete body;
                }
            }
        }
        m_cubeMutex.unlock();
        qDebug() << "Drawing touchRay " << m_touchRay[0] <<" to "<< m_touchRay[1];
    } else {
        qDebug() << "Missed ";
    }

    //    m_cubeMutex.lock();
    //    for (m_cubes_i = m_cubes.begin(); m_cubes_i != m_cubes.end(); ++m_cubes_i) {
    //        btRigidBody* body = btRigidBody::upcast(*m_cubes_i);
    //        if (body && body->getMotionState())
    //        {
    //            btTransform trans;
    //            body->getMotionState()->getWorldTransform(trans);
    //            QMatrix4x4  pos = bt2QMatrix4x4(&trans);
    //            qDebug() << *(QString*)(body->getUserPointer()) << " at " << pos.column(3);
    //        }
    //    }
    //    m_cubeMutex.unlock();

}

void Bullet::release(){
    m_touchRayActive = false;
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

// This sets up the GL objects/meshes that *can* be used in bullet
void Bullet::setupGL(GLProgram* p)
{
    m_program_debug = new GLProgram(SailfishApp::pathTo("debug_vert.glsl"), SailfishApp::pathTo("debug_frag.glsl"));

    for (auto meshname : m_meshes->getNames()) {
        BiMesh* bimesh = m_meshes->getBiMesh(meshname);
        bimesh->setup(p);
    }
}

void Bullet::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    QVector3D qfrom(from.x(), from.y(), from.z());
    QVector3D qto(to.x(), to.y(), to.z());
    QVector4D qcol(color.x(), color.y(), color.z(), 1.0);

    //    qDebug() << "Drawing line color " << qcol << " from " << qfrom << " to " << qto;
    (*m_worldLines)[Color(qcol)].append({qfrom, qto});
    //    qDebug() << "color " << qcol << " has " << (*m_worldLines)[Color(qcol)].size() << " entries now";
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
