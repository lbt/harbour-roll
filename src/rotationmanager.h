#ifndef ROTATIONMANAGER_H
#define ROTATIONMANAGER_H

#include <QObject>
#include <QMatrix4x4>

class RotationManager : public QObject
{
    Q_OBJECT
public:
    explicit RotationManager(QObject *parent = 0);

    bool isActive() const { return m_pressed; }
    QMatrix4x4 transform(QMatrix4x4 v);

signals:

public slots:
    void release(); // no longer in use
    void touch(qreal x, qreal y); // current position

private:
    bool m_pressed;
    qreal m_touchX;
    qreal m_touchY;
    qreal m_x;
    qreal m_y;
    qreal m_theta;
    qreal m_phi;
    qreal m_depth;
};

#endif // ROTATIONMANAGER_H
