#include "rollrunner.h"

//////////////////////////////////////////////////////////
/// \brief RollRunner::RollRunner
/// \param w
/// \param parent
///
RollRunner::RollRunner(RollWorld *w):
    WorldRunner(w)
  , m_rollWorld(w)
  , m_gravity(true)
  , m_fly(false)
{
    qDebug() << "create";
}

void RollRunner::setup()
{
    qDebug() << "setup";
    WorldRunner::setup();
    m_sensor.start();
}

void RollRunner::gravity(bool state)
{
    m_gravity = state;
}

void RollRunner::setDebugDraw(bool state)
{
    if (state)
        m_rollWorld->m_debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    else
        m_rollWorld->m_debugDrawer.setDebugMode(0);
}

void RollRunner::runStep() {
//    qDebug() << "runstep";
    if (m_gravity) {
        //        if (!m_fly) {
        QAccelerometerReading *reading = m_sensor.reading();
        m_rollWorld->setGravity(reading->x(), reading->y(), reading->z());
        //        }
    } else {
        m_rollWorld->setGravity(0, 0, 0);
    }

    WorldRunner::runStep();
}
