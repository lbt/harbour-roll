#include "transform.h"

Transform::Transform()
{
}

Transform::Transform(QMatrix4x4 t) :
    QMatrix4x4(t)
{
}

void Transform::rotateOnly(QMatrix4x4 r)
{
    QVector4D p = position(); // store position
    *this = r * *this; // rotate
//    QMatrix4x4 tmp(r * *this);
//    tmp.copyDataTo(this->data());
//    this->optimize();
    setPosition(p); // restore position
}
