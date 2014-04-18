#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <QObject>
#include "transform.h"

class LightManager: public QObject
{
    Q_OBJECT
public:
    explicit LightManager(QObject* parent=0);
    explicit LightManager(Transform t, QObject* parent=0);
    void update(int ms) {Q_UNUSED(ms)}
    void randomise() {}
    Transform getTransform() const { return m_transform; }

public slots:

protected:
    Transform m_transform;
};

#endif // LIGHTMANAGER_H
