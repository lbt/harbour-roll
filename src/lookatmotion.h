#ifndef LOOKATMOTION_H
#define LOOKATMOTION_H
#include "basemotion.h"

class LookAtMotion : public BaseMotion
{
    Q_OBJECT
public:
    explicit LookAtMotion(WorldItem *parent = 0);

    void setLookAt(QVector3D point, QVector3D up = QVector3D(0,0,1)) { m_lookAt=point; m_up=up; }
    void setLookAt(WorldItem* wi) { m_wi = wi; }

    void runStep(int deltaTms);
    Transform getTransform(Transform current) const;

signals:

public slots:

protected:
    WorldItem* m_wi;
    QVector3D m_lookAt;
    QVector3D m_up;

};

#endif // LOOKATMOTION_H
