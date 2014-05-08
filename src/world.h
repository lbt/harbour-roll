#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QMutex>
#include <QThread>

#include "light.h"
class Light;        // Mutual link
#include "physicsmotion.h"
class PhysicsMotion;      // Mutual link
#include "shader.h"
class Shader;       // Mutual link
#include "worlddebugdrawer.h"
#include "worlditem.h"
class WorldItem;    // Mutual link with WorldItem from worlditem.h

#include "worldrunner.h"
class WorldRunner;  // Mutual link

#include "cameramanager.h"

class World : public QObject
{
    Q_OBJECT

    friend class WorldBuilder;
    friend class WorldItem;
    friend class Light;
    friend class PhysicsMotion;
    friend class CameraManager;
    friend class WorldRunner;
public:
    explicit World(QObject *parent = 0);
    ~World();
    virtual void setup();
    virtual void start();
    virtual QString serialise();
    virtual void restore(QString state);

    virtual void runStep(int ms);
    virtual void btTickCallback(btScalar timestep);

    // Supporting various shader/render world info
    virtual void setupGL();
    virtual void render();

    // Items
    virtual WorldItem* getItem(QString name);

    // Lights
    virtual Light* getLight(QString name);
    virtual QList<Light*> getLights();

    // Camera
    virtual QMatrix4x4 getActiveCameraPVM();
    virtual QVector3D getActiveCameraAt();
    virtual CameraManager* getActiveCamera() { return m_activeCamera; }
    void setActiveCamera(CameraManager* camera) { m_activeCamera = camera; }
    void setActiveCamera(QString name);
    virtual CameraManager* getCamera(QString name);


protected:
    virtual void add(WorldItem *item, QList<Shader *> shaderList);
    virtual void remove(WorldItem *item, QList<Shader *> shaderList);
    virtual void add(Light *light);
    virtual void remove(Light *light);
    virtual void add(CameraManager *camera);
    virtual void remove(CameraManager *camera);

    virtual void lock(){ m_worldMutex.lock(); }
    virtual void unlock() { m_worldMutex.unlock(); }

    // Support subclassing
    virtual void setupPhysicsWorld();
    virtual void destroyPhysicsWorld();
    virtual void createRunner();
    virtual void handleCollision(Collision& collision) { Q_UNUSED(collision); }

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

    QHash<QString, WorldItem*> m_worlditems;
    QHash<Shader*, QSet<WorldItem*>> m_byShader;

    QMultiMap<qreal, WorldItem*> m_WIByDistance;
    QHash<QString, Light*> m_lights;
    QHash<QString, CameraManager*> m_cameras;

    WorldDebugDrawer m_debugDrawer;
    Shader* m_debugShader;
    QMutex m_worldMutex;

    QThread m_runnerThread;
    WorldRunner* m_runner;

    CameraManager* m_activeCamera;
};

#endif // WORLD_H
