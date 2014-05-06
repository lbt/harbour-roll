#include "rollball.h"

RollBall::RollBall(QString name, BaseMotion *motion) :
    WorldItem(name, motion)
  , m_ballStartPos(QVector3D())
{
}

void RollBall::reset() {
    Transform pos;
    pos.translate(m_ballStartPos);
    physicsMotion()->setTransform(pos);
    physicsMotion()->setVelocity(QVector3D());
}
