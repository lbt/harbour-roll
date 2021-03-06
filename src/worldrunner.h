#ifndef WORLDRUNNER_H
#define WORLDRUNNER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include "world.h"

// How often the physics world is ticked
#define TICK 17

class WorldRunner : public QObject
{
    Q_OBJECT
public:
    explicit WorldRunner(World *w);

signals:
    void stepReady();

public slots:

    virtual void runStep();
    virtual void setDebugDraw(int dbg);

    virtual void setRunning(bool running);
    virtual void setup();

protected:
    World* m_world;
    QElapsedTimer m_worldTime; // FIXME do we need this?
    bool m_running;
    QTimer* m_timer;

};

#endif // WORLDRUNNER_H
