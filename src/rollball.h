#ifndef ROLLBALL_H
#define ROLLBALL_H

#include "worlditem.h"

class RollBall : public WorldItem
{
    Q_OBJECT
public:
    explicit RollBall(QString name, BaseMotion *motion);

    void setStart(QVector3D start) { m_ballStartPos = start; }
    void reset();

signals:

public slots:

protected:
   QVector3D  m_ballStartPos;
};

#endif // ROLLBALL_H
