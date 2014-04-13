#ifndef WORLDITEM_H
#define WORLDITEM_H

#include <QObject>
#include <QList>

#include "physics.h"
class Physics;    // Mutual link with Physics from physics.h
#include "renderable.h"
class Renderable; // Mutual link
#include "shader.h"
class Shader;     // Mutual link
#include "world.h"
class World;      // Mutual link

class WorldItem : public QObject
{
    Q_OBJECT
    friend class World;
public:
    explicit WorldItem(QString name, World *parent);

    void add(Renderable* r);
    void add(Physics* p);
    Physics* physics() const {return m_physics;}
    QMatrix4x4 transform() const;

    void render(const Shader *activeProgram);
    void setupGL();
    void addToWorld();
    void leaveWorld();
    void setVelocity(QVector3D v);
    QVector3D getVelocity();
    void setTransform(QMatrix4x4 t);
    QMatrix4x4 getTransform();
signals:

public slots:

private:
    World* m_world;
    QList<Renderable*> m_renderables;
    Physics* m_physics;
    QMatrix4x4 m_transform;
    QVector3D m_velocity;
};

#endif // WORLDITEM_H
