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
    // this = r * this; // rotate
    QMatrix4x4::operator*=(r); // rotate
    setPosition(p); // restore position
}
