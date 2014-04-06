#ifndef WORLDITEM_H
#define WORLDITEM_H

#include <QObject>
#include <QList>

#include "renderable.h"
#include "physics.h"
class Physics; // Mutual link with Physics from physics.h
#include "world.h"
class World; // Mutual link with World from world.h

class WorldItem : public QObject
{
    Q_OBJECT
    friend class World;
public:
    explicit WorldItem(World *w, World *parent);

    void add(Renderable* r);
    void add(Physics* p);
    Physics* physics() const {return m_physics;}
    QMatrix4x4 pos() const { return m_pos; }

    void render(GLProgram *activeProgram);
signals:

public slots:

private:
    World* m_world;
    QList<Renderable*> m_renderables;
    Physics* m_physics;
    QMatrix4x4 m_pos;
};

#endif // WORLDITEM_H
