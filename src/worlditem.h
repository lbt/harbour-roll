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

    virtual void addRenderable(Renderable* r);
    virtual void setMotionManager(MotionManager *m);
    virtual MotionManager* motion() const { return m_motion;}
    virtual Physics* physics() { return dynamic_cast<Physics*>(m_motion);}

    virtual void setupGL();
    virtual void render(const Shader *activeProgram);

    virtual void setVelocity(QVector3D v) { m_motion->setVelocity(v); }
    virtual QVector3D getVelocity() const { return m_motion->getVelocity(); }
    virtual void setTransform(QMatrix4x4 t) { m_motion->setTransform(t); }
    virtual Transform getTransform() const { return m_motion->getTransform(); }

    virtual void addToWorld(World* world);
    virtual void removeFromWorld();

signals:

public slots:

protected:
    virtual bool inWorld(); // If in a World
    virtual QList<Shader *> shaderList();

private:
    QList<Renderable*> m_renderables;
    MotionManager* m_motion;
    QString m_name;
};

#endif // WORLDITEM_H
