#ifndef ROLLWORLD_H
#define ROLLWORLD_H

#include "world.h"
#include "cameramanager.h"

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
    QMatrix4x4 getActiveCameraPVM();
    QVector3D getActiveCameraAt();

    CameraManager* getCameraMangager() { return m_camera; }
    void setCameraMangager(CameraManager* camera) { m_camera = camera; }

    QStringList getTrackNames();
signals:

public slots:
    void setDebugDraw(bool state) ;
    void setGravity(float x, float y, float z);
    void gravity(bool state) { m_gravity = state; }
    void fly(bool state) { m_fly = state; }

private:
    WorldItem* m_ball;
    WorldItem* m_floor;
    CameraManager* m_camera;

    QAccelerometer m_sensor;
    bool m_gravity;
    bool m_fly;
};


#endif // ROLLWORLD_H
