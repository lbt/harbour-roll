#include "linearmotion.h"
#include "worlditem.h"
#include "world.h"
#include "utils.h"

LinearMotion::LinearMotion(WorldItem *parent) :
    BaseMotion(parent)
{
}

void LinearMotion::setTransformVelocity(Transform t, QVector3D v)
{
    m_transform = t;
    m_velocity = v;
}

void LinearMotion::runStep(int deltaTms) {
    m_transform.translate(m_velocity * (deltaTms/1000.0));
}


void LinearMotion::randomise(QVector3D bottomBackLeft, QVector3D topFrontRight)
{
    QVector3D &min = bottomBackLeft;
    QVector3D &max = topFrontRight;
    QVector3D pos(rnd(max.x()-min.x())+min.x(),
                  rnd(max.y()-min.y())+min.y(),
                  rnd(max.z()-min.z())+min.z() );
    Transform t;
    t.translate(pos);
    setTransform(t);
    // Max/min seconds to traverse entire world
#define MAXT 18
#define MINT 2
    m_velocity=QVector3D(
                (max.x()-min.x())/(rnd(MAXT)+MINT) * (((rand()%2)==0)?-1:1) ,
                (max.y()-min.y())/(rnd(MAXT)+MINT) * (((rand()%2)==0)?-1:1) ,
                (max.z()-min.z())/(rnd(MAXT)+MINT) * (((rand()%2)==0)?-1:1) );
}
