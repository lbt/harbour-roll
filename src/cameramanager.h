#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include "worlditem.h"
#include "transform.h"

class CameraManager : public WorldItem
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

    explicit CameraManager(QString name, Display display=Display());
    void setDisplay(int w, int h, qreal fov) { m_display=Display(w, h, fov); }

    qreal FOV() const { return m_display.m_fov; }
    int screenWidth() const { return m_display.m_screenWidth; }
    int screenHeight() const { return m_display.m_screenHeight; }

    virtual void reset();

    virtual QVector3D right() const { return getTransform().right(); }
    virtual QVector3D up() const { return getTransform().up(); }
    virtual QVector3D forward() const { return getTransform().forward(); }
    virtual QVector3D at() const { return getTransform().at(); }
    virtual QMatrix4x4 projViewMatrix() const;

    virtual void addToWorld(World *world);
    virtual void removeFromWorld();
signals:

public slots:
    virtual void lookAt(QVector3D go, QVector3D target, QVector3D up);
    virtual void activate();
    virtual void update(int deltaTms);

protected:
    Display m_display;

};

#endif // ROTATIONMANAGER_H
