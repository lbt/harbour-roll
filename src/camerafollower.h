#ifndef CAMERAFOLLOWER_H
#define CAMERAFOLLOWER_H

#include "motionmanager.h"
class WorldItem;

class CameraFollower : public MotionManager
{
    Q_OBJECT
public:
    explicit CameraFollower(WorldItem* parent=0);
    void runStep(int deltaTms);

signals:

public slots:
    void follow(WorldItem* wi, float dist);

protected:
    WorldItem* m_wi;
    float m_dist;
};

#endif // CAMERAFOLLOWER_H
