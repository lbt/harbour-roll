#ifndef WORLDITEM_H
#define WORLDITEM_H

#include <QObject>
#include <QList>
#include "transform.h"

#include "basemotion.h"
#include "physicsmotion.h"
class Renderable; // Mutual link
class Shader;     // Mutual link
class World;      // Mutual link
#include "collision.h"
class Collision;

class WorldItem : public QObject
{
    Q_OBJECT
    friend class World;
public:
    explicit WorldItem(QString name, BaseMotion* motion);

    virtual void addRenderable(Renderable* r);
    virtual BaseMotion* setMotion(BaseMotion *m);
    virtual BaseMotion* motion() const { return m_motion;}
    virtual PhysicsMotion* physicsMotion() { return dynamic_cast<PhysicsMotion*>(m_motion);}

    virtual void setupGL();
    enum render_pass { OPAQUE, TRANSLUCENT } ;
    virtual void render(const Shader *activeProgram, render_pass pass);
    virtual void debugRender(QMatrix4x4 projViewMatrix) { Q_UNUSED(projViewMatrix); }

    virtual void setTransform(QMatrix4x4 t) { m_motion->setTransform(t); }
    Transform getTransform() const { return m_transform; }
    virtual void updateTransform() { m_transform = m_motion->getTransform(); }

    enum collision_type { NO_COLLISONS, WORLD_COLLISONS, ITEM_COLLISONS } ;
    virtual collision_type getCollisionType() { return m_collisionType; }
    virtual void setCollisionType(collision_type type) { m_collisionType = type; }
    virtual void collision(Collision &collision);

    virtual void addToWorld(World* world);
    virtual void removeFromWorld();

signals:

public slots:

protected:
    virtual bool inWorld(); // If in a World
    virtual QList<Shader *> shaderList();

private:
    QList<Renderable*> m_renderables;
    BaseMotion* m_motion;
    QString m_name;
    Transform m_transform;
    collision_type m_collisionType;
};

#endif // WORLDITEM_H
