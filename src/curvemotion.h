#ifndef CURVEMOTION_H
#define CURVEMOTION_H

#include "basemotion.h"
#include "assetstore.h"
#include "vao.h"

class CurveMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit CurveMotion(WorldItem *parent = 0);

    void setCurve(VAO* vao);
    void setSpeed(float speed) { m_speed = speed; }
    void runStep(int deltaTms);

signals:

public slots:

protected:
    qreal m_speed;
    VAO* m_curveVAO;
    QList<qreal> m_distance;
    int m_lastSeenLine;
    float m_circumference;
    float m_currentDist;

};

#endif // CURVEMOTION_H
