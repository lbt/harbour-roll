#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QMutex>
#include <QThread>

#include "light.h"
class Light;        // Mutual link
#include "physics.h"
class Physics;      // Mutual link
#include "shader.h"
class Shader;       // Mutual link
#include "worlddebugdrawer.h"
#include "worlditem.h"
class WorldItem;    // Mutual link with WorldItem from worlditem.h

#include "worldrunner.h"
class WorldRunner;  // Mutual link

class World : public QObject
{
    Q_OBJECT

    friend class WorldItem;
    friend class WorldRunner;
public:
    explicit World(QObject *parent = 0);
    ~World();
    void start();
    QString serialise();
    void restore(QString state);

    void runStep(int ms);

    void lock() { m_worldMutex.lock(); }
    void unlock() { m_worldMutex.unlock(); }

    // Supporting various shader/render world info
    void setupGL();
    void render();
    QList<Light*> getLights();
    QMatrix4x4 getActiveCameraPVM();
    QVector3D getActiveCameraAt();


protected:
    void add(WorldItem *i);
    void add(Physics *p);
    void add(Light *l);

    // Support subclassing
    void setupPhysicsWorld();
    void destroyPhysicsWorld();
    void createRunner();

signals:
    void stepReady();

public slots:    
    void setRunning(bool running);

protected:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    QMap<QString, WorldItem*> m_worlditems;
    QMap<Shader*, QList<WorldItem*>> m_byShader;

    WorldDebugDrawer m_debugDrawer;
    Shader* m_debugShader;
    QMutex m_worldMutex;


    QThread m_runnerThread;
    WorldRunner* m_runner;
};

#endif // WORLD_H
