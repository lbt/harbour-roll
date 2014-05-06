#include "delayedmotion.h"

DelayedMotion::DelayedMotion(int frameDelay, WorldItem *parent) :
    FollowMotion(parent)
  , m_frameDelay(frameDelay)
  , m_ptr(0)
{
    m_stack.reserve(frameDelay);
}

void DelayedMotion::runStep(int deltaTms)
{
    Q_UNUSED(deltaTms);
    FollowMotion::runStep(deltaTms);
    if (m_stack.size() <= m_frameDelay) {
        m_stack << m_transform;
        m_transform = m_stack[0];
    } else {
        m_stack[m_ptr] = m_transform;
        int p = (m_ptr+1)%m_frameDelay;
//        qDebug() << "size " << m_stack.size() << " ptr " << m_ptr << " p " << p;
        m_transform = m_stack.at(p);
    }
    m_ptr++;
    m_ptr %= m_frameDelay;
}
