#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include <QObject>
#include <QMatrix4x4>

class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = 0);

    bool isActive() const { return m_pressed; }
    QMatrix4x4 transform(QMatrix4x4 v);
    QVector3D at();
    void reset();

signals:

public slots:
    void release(); // no longer in use
    void touch(qreal x, qreal y); // current position

private:
    bool m_pressed;
    qreal m_touchX;
    qreal m_touchY;
    qreal m_theta;
    qreal m_phi;
    qreal m_depth;
    qreal m_x;
    qreal m_y;
};

#endif // ROTATIONMANAGER_H
