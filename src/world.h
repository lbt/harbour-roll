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

    friend class WorldBuilder;
    friend class WorldItem;
    friend class WorldRunner;
public:
    explicit World(QObject *parent = 0);
    ~World();
    virtual void setup();
    virtual void start();
    virtual QString serialise();
    virtual void restore(QString state);

    virtual void runStep(int ms);

    virtual void lock() { m_worldMutex.lock(); }
    virtual void unlock() { m_worldMutex.unlock(); }

    // Supporting various shader/render world info
    virtual void setupGL();
    virtual void render();
    virtual QList<Light*> getLights();
    virtual QMatrix4x4 getActiveCameraPVM();
    virtual QVector3D getActiveCameraAt();


protected:
    virtual void add(WorldItem *i);
    virtual void add(Physics *p);
    virtual void remove(WorldItem *i);
    virtual void remove(Physics *p);
    virtual void add(QString name, Light *l);

    // Support subclassing
    virtual void setupPhysicsWorld();
    virtual void destroyPhysicsWorld();
    virtual void createRunner();

signals:
    void stepReady();

public slots:    
    virtual void setRunning(bool running);

protected:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    QMap<QString, WorldItem*> m_worlditems;
    QMap<Shader*, QSet<WorldItem*>> m_byShader;

    QMap<QString, Light*> m_lights;

    WorldDebugDrawer m_debugDrawer;
    Shader* m_debugShader;
    QMutex m_worldMutex;


    QThread m_runnerThread;
    WorldRunner* m_runner;
};

#endif // WORLD_H
