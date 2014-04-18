#ifndef CAMERAFOLLOWER_H
#define CAMERAFOLLOWER_H

#include "cameramanager.h"
#include "worlditem.h"

class CameraFollower : public CameraManager
{
    Q_OBJECT
public:
    explicit CameraFollower(QString name, Display display, QObject *parent = 0);
    void update(int deltaTms);

signals:

public slots:
    void follow(WorldItem* wi, float dist);

protected:
    WorldItem* m_wi;
    float m_dist;
};

#endif // CAMERAFOLLOWER_H
