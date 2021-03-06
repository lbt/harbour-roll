#include "worldrunner.h"

WorldRunner::WorldRunner(World *w):
    QObject(0)
  , m_world (w)
  , m_running(false)
{
}

void WorldRunner::setup() {
    qDebug() << "setup";
    m_worldTime.start();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(runStep()) );
    m_timer->setInterval(TICK);;
}

void WorldRunner::setRunning(bool running){
    if (m_running == running) return;

    m_running = running;
    if (running) {
        m_timer->start();
    } else {
        m_timer->stop();
    }
}

void WorldRunner::setDebugDraw(int dbg)
{
    m_world->m_debugDrawer.setDebugMode(dbg);
}

void WorldRunner::runStep() {
    int timeDelta_ms = m_worldTime.restart();
    m_world->runStep(timeDelta_ms );
    emit stepReady();
}
