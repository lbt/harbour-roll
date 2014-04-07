#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QList>
#include <QHash>
#include <QList>
#include <QMutex>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/LinearMath/btIDebugDraw.h>

#include <stdio.h>

#include "glprogram.h"
#include "bimesh.h"
#include "worldobject.h"

class Bullet : public QObject, public btIDebugDraw
{
    Q_OBJECT

public:
    struct Line {
        const QVector3D from;
        const QVector3D to;
    };

    class Color {
    public:
        Color(QVector4D c) { m_c = c; }
        Color() {}
        Color(const Color &other) { m_c = other.m_c; }
        ~Color() {}
        Color &operator=(const Color &other);

    public:
        QVector4D m_c;
    };

    class ContactResultCallback : public btCollisionWorld::ContactResultCallback
    {
    public:
        btScalar addSingleResult(btManifoldPoint & cp,
                                 const btCollisionObjectWrapper *colObj0Wrap,
                                 int partId0,
                                 int index0,
                                 const btCollisionObjectWrapper *colObj1Wrap,
                                 int partId1,
                                 int index1
                                 ) { m_contacts << colObj0Wrap->getCollisionObject()
                                                << colObj1Wrap->getCollisionObject(); }
        QList< const btCollisionObject *> getContacts() { return m_contacts; }
    private:
        QList< const btCollisionObject *> m_contacts;
    };

public:
    explicit Bullet(QObject *parent = 0);
    ~Bullet();

    virtual void setupModel(QString state="");
    void runStep(int ms);
    void report();

    void renderWalls(GLProgram *p);
    void render(GLProgram *p, QMatrix4x4 projViewMatrix);
    void addWall(btVector3 normal, float offset);

    const QList<QString> getNames() const;

signals:
    void numWorldObjectsChanged(int num);

public slots:
    void setGravity(qreal x, qreal y, qreal z);
    void kick();

    void touch(float x, float y, QMatrix4x4 projViewMatrix, QVector3D lookingToward);
    void release();

    void addRoll(QString meshName, btVector3 pos = btVector3(0,1,5), btScalar mass = btScalar(0.1));
    void removeObject(WorldObject *wobj);
    QMatrix4x4 getFollowInfo();

protected:
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    void addCube(btVector3 pos);
    void loadDice();

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;

    QVector3D m_touchRay[14];
    bool m_touchRayActive;
    WorldObject* m_ball;
    WorldObject* m_floor;

    // Below here is the functionality needed to be a btIDebugDraw
public:
    void setupGL(GLProgram *p); // setup GL stuff
    void drawLine(const btVector3 &  	from,
                  const btVector3 &  	to,
                  const btVector3 &  	color);
    void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    void	reportErrorWarning(const char* warningString);
    void	draw3dText(const btVector3& location,const char* textString);
    void	setDebugMode(int debugMode);
    int		getDebugMode() const;
    void render2(GLProgram *p, QMatrix4x4 projViewMatrix);
    QString serialise();
    void restore(QString state);
    void useTrack(QString track);
private:
    GLProgram* m_program_debug;
    int m_debug_mode;
    QHash<Color, QList<Line> > *m_worldLines;
    typedef QHash<Color, QList<Line> >::const_iterator m_worldLines_iter_type;
    typedef QList<Line>::const_iterator m_worldLines_list_iter_type;
    QVector<QVector3D> m_qlinepoints;
};

static uint qHash(Bullet::Color c) { return c.m_c.x()*10000 + c.m_c.y()*100 + c.m_c.z(); }
inline bool operator==(const Bullet::Color &c1, const Bullet::Color &c2) { return c1.m_c == c2.m_c; }

#endif // BULLET_H
