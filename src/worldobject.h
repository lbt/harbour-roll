#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include "bimesh.h"
#include <QElapsedTimer>

// Declared in bullet.cpp
QMatrix4x4 bt2QMatrix4x4(btTransform *transform);

class WorldObject : public QObject
{
    Q_OBJECT
public:
    explicit WorldObject(BiMesh* bimesh, btRigidBody* body, QObject *parent = 0);

    btRigidBody* getRigidBody() const { return m_rigidBody; }
    BiMesh* getBiMesh() const { return m_bimesh; }

    void setHit(bool h) {m_hit =  h;}
    void render(GLProgram *p);
    void setup(GLProgram *p);

signals:

public slots:

private:
    BiMesh* m_bimesh;
    btRigidBody* m_rigidBody;

    QVector4D m_glow;
    bool m_hit;

};

class World : public QObject
{
    Q_OBJECT
private:
    QSet<WorldObject*> m_objects;

public:
    explicit World(QObject *parent = 0);

};

#endif // WORLDOBJECT_H
