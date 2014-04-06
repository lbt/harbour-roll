#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QMutex>

#include "worlditem.h"
class WorldItem; // Mutual link with WorldItem from worlditem.h
#include "physics.h"
class Physics;   // Mutual link with Physics from physics.h
#include "worlddebugdrawer.h"

class World : public QObject
{
    Q_OBJECT

    friend class WorldItem;
public:
    explicit World(QObject *parent = 0);
    ~World();


    void render();

protected:
    void add(WorldItem *i);
    void add(Physics *p);

signals:

public slots:

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    QMap<QString, WorldItem*> m_world;
    QMap<GLProgram*, WorldItem*> m_byShader;

    WorldDebugDrawer m_debugDrawer;
    QMutex m_worldMutex;
};

#endif // WORLD_H
