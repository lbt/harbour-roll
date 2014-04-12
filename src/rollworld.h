#ifndef ROLLWORLD_H
#define ROLLWORLD_H

#include "world.h"
#include "worldrunner.h"
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
    void createRunner();
    void runStep(int ms);

    QStringList getTrackNames();
signals:

public slots:
    void fly(bool state) { QMetaObject::invokeMethod(
                    m_runner,"fly", Qt::QueuedConnection, Q_ARG(bool, state));}
    void gravity(bool state) { QMetaObject::invokeMethod(
                    m_runner,"gravity", Qt::QueuedConnection, Q_ARG(bool, state));}
    void setDebugDraw(bool state) { QMetaObject::invokeMethod(
                    m_runner,"setDebugDraw", Qt::QueuedConnection, Q_ARG(bool, state));}
    void setGravity(float x, float y, float z);

private:
    WorldItem* m_ball;
    WorldItem* m_floor;

};

class RollRunner : public WorldRunner
{
    Q_OBJECT
public:
    explicit RollRunner(RollWorld *w);

public slots:
    void setup();
    void setDebugDraw(bool state);
    void runStep();

    void gravity(bool state);
    void fly(bool state) { m_fly = state; }

protected:
    RollWorld*  m_rollWorld;

private:
    QAccelerometer m_sensor;
    bool m_gravity;
    bool m_fly;
};


#endif // ROLLWORLD_H
