#ifndef FOLLOWMOTION_H
#define FOLLOWMOTION_H

#include "basemotion.h"
class WorldItem;

class FollowMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit FollowMotion(WorldItem* parent=0);
    void runStep(int deltaTms);

signals:

public slots:
    void follow(WorldItem* wi, float dist);

protected:
    WorldItem* m_wi;
    float m_dist;
};

#endif // FOLLOWMOTION_H
