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
    void follow(WorldItem* wi, QVector3D offset);

protected:
    WorldItem* m_wi;
    QVector3D m_offset;
};

#endif // FOLLOWMOTION_H
