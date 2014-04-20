#ifndef ITEMFOLLOWER_H
#define ITEMFOLLOWER_H

#include "basemotion.h"
class WorldItem;

class ItemFollower : public BaseMotion
{
    Q_OBJECT
public:
    explicit ItemFollower(WorldItem* parent=0);
    void runStep(int deltaTms);

signals:

public slots:
    void follow(WorldItem* wi, float dist);

protected:
    WorldItem* m_wi;
    float m_dist;
};

#endif // ITEMFOLLOWER_H
