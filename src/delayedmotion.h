#ifndef DELAYEDMOTION_H
#define DELAYEDMOTION_H

#include "followmotion.h"

class DelayedMotion : public FollowMotion
{
    Q_OBJECT
public:
    explicit DelayedMotion(int frameDelay, WorldItem *parent = 0);
    void runStep(int deltaTms);

signals:

public slots:
    void delay(int frameDelay) { m_frameDelay = frameDelay; }

protected:
    int m_frameDelay;
    int m_ptr;
    QList<Transform> m_stack;
};

#endif // DELAYEDMOTION_H
