#ifndef WORLDITEM_H
#define WORLDITEM_H

#include <QObject>
#include <QList>
#include "transform.h"

class Physics;    // Mutual link with Physics from physics.h
class Renderable; // Mutual link
class Shader;     // Mutual link
class World;      // Mutual link

class WorldItem : public QObject
{
    Q_OBJECT
    friend class World;
public:
    explicit WorldItem(QString name);

    void add(Renderable* r);
    void add(Physics* p);
    Physics* physics() const {return m_physics;}

    void setupGL();
    void render(const Shader *activeProgram);

    void setVelocity(QVector3D v);
    QVector3D getVelocity() const;
    void setTransform(QMatrix4x4 t);
    Transform getTransform() const;

    void addToWorld(World* world);
    void removeFromWorld();

signals:

public slots:

protected:
    bool inWorld(); // If in a World
    QList<Shader *> shaderList();

private:
    QList<Renderable*> m_renderables;
    Physics* m_physics;
    Transform m_transform;
    QVector3D m_velocity;
};

#endif // WORLDITEM_H
