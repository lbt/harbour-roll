#ifndef ROLLWORLD_H
#define ROLLWORLD_H

#include "world.h"
#include "cameramanager.h"
#include "rollball.h"

#include <QJsonDocument>
#include <QtSensors/QAccelerometer>
#include <QStringList>

class RollWorld : public World
{
    Q_OBJECT
    class ContactResultCallback : public btCollisionWorld::ContactResultCallback
    {
    public:
        btScalar addSingleResult(btManifoldPoint & cp,
                                 const btCollisionObjectWrapper *colObj0Wrap,
                                 int partId0,
                                 int index0,
                                 const btCollisionObjectWrapper *colObj1Wrap,
                                 int partId1,
                                 int index1
                                 ) { Q_UNUSED(cp) Q_UNUSED(partId0) Q_UNUSED(index0) Q_UNUSED(partId1) Q_UNUSED(index1)
                    m_contacts << colObj0Wrap->getCollisionObject()
                               << colObj1Wrap->getCollisionObject();
                                     return 0; } // return value is unused
        QList< const btCollisionObject *> getContacts() { return m_contacts; }
    private:
        QList< const btCollisionObject *> m_contacts;
    };


    friend class RollRunner;
    friend class RollBuilder;
public:
    explicit RollWorld(QObject *parent = 0);
    void setup();
    void runStep(int ms);
    RollBall* getBall() { return m_ball; }
    Transform getFollowInfo() { return m_ball->getTransform(); }

    QStringList getTrackNames();
signals:

public slots:
    void setDebugDraw(int mode) ;
    void setGravity(float x, float y, float z);
    void gravity(bool state) { m_gravity = state; }

protected:
    virtual void handleCollision(Collision &collision);

private:
    RollBall* m_ball;
    WorldItem* m_floor;
    QVector3D m_ballStartPos;

    QAccelerometer m_sensor;
    bool m_gravity;
    int m_dbgMode;
};


#endif // ROLLWORLD_H
