#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>

#include <assimp/scene.h>


class Physics : public QObject
{
    Q_OBJECT
public:
    explicit Physics(QObject *parent = 0);

    void addShape(QString modelType, btScalar r);
    void addShape(QString modelType, aiMesh *m = NULL);

signals:

public slots:

private:
    btCollisionShape* m_shape;

};

#endif // PHYSICS_H
