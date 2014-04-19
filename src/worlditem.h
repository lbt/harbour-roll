#ifndef WORLDITEM_H
#define WORLDITEM_H

#include <QObject>
#include <QList>
#include "transform.h"

#include "motionmanager.h"
#include "physics.h"
class Renderable; // Mutual link
class Shader;     // Mutual link
class World;      // Mutual link

class WorldItem : public QObject
{
    Q_OBJECT
    friend class World;
public:
    explicit WorldItem(QString name);

    void addRenderable(Renderable* r);
    void setMotionManager(MotionManager *m);
    MotionManager* motion() const { return m_motion;}
    Physics* physics() { return dynamic_cast<Physics*>(m_motion);}

    void setupGL();
    void render(const Shader *activeProgram);

    void setVelocity(QVector3D v) { m_motion->setVelocity(v); }
    QVector3D getVelocity() const { return m_motion->getVelocity(); }
    void setTransform(QMatrix4x4 t) { m_motion->setTransform(t); }
    Transform getTransform() const { return m_motion->getTransform(); }

    void addToWorld(World* world);
    void removeFromWorld();

signals:

public slots:

protected:
    bool inWorld(); // If in a World
    QList<Shader *> shaderList();

private:
    QList<Renderable*> m_renderables;
    MotionManager* m_motion;
    Transform m_transform;
    QVector3D m_velocity;
};

#endif // WORLDITEM_H
