#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QMatrix4x4>

class Transform : public QMatrix4x4
{
public:
    Transform();
    Transform(QMatrix4x4 t);

    QVector3D right() { return column(0).toVector3D(); }
    QVector3D up() { return column(1).toVector3D(); }
    QVector3D forward() { return column(2).toVector3D(); }
    QVector3D at() { return column(3).toVector3D(); }
    void rotateOnly(QMatrix4x4 r);

private:
    QVector4D position() {return column(3);}
    void setPosition(QVector4D position){ setColumn(3, position); }
};

#endif // TRANSFORM_H
