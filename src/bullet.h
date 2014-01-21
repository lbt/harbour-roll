#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QVector>

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

class Bullet : public QObject
{
    Q_OBJECT
public:
    explicit Bullet(QObject *parent = 0);
    ~Bullet();

    void setupModel();
    void runStep();
    void report();

signals:

public slots:
    void setGravity(qreal x, qreal y, qreal z);

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;


};

#endif // BULLET_H
