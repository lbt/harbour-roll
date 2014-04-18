#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include <QObject>
#include "transform.h"
#include <QtSensors/QAccelerometer>

class CameraManager : public QObject
{
    Q_OBJECT
public:
    class Display {
    public:
        explicit Display(int width=540, int height=960, qreal fov=50.0) :
            m_screenWidth(width)
          , m_screenHeight(height)
          , m_fov(fov)
    {}

        int m_screenWidth;
        int m_screenHeight;
        qreal m_fov;
    };

    explicit CameraManager(QString name, Display display=Display(), QObject *parent = 0);
    void setDisplay(int w, int h, qreal fov);

    qreal FOV() const { return m_display.m_fov; }
    int screenWidth() const { return m_display.m_screenWidth; }
    int screenHeight() const { return m_display.m_screenHeight; }

    virtual void reset();

    virtual QVector3D right() { return m_camera.right(); }
    virtual QVector3D up() { return m_camera.up(); }
    virtual QVector3D forward() { return m_camera.forward(); }
    virtual QVector3D at() { return m_camera.at(); }
    virtual QMatrix4x4 projViewMatrix();

signals:

public slots:
    virtual void lookAt(QVector3D go, QVector3D target, QVector3D up);

    virtual void update(int deltaTms);

protected:
    Transform m_camera;
    Display m_display;

};

#endif // ROTATIONMANAGER_H
