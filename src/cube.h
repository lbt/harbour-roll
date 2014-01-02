#ifndef CUBE_H
#define CUBE_H

#include "glitem.h"

class Cube : public GLItem
{
    Q_OBJECT
    Q_PROPERTY(qreal d READ d WRITE setD NOTIFY dChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    explicit Cube(QObject *parent = 0);
    qreal d() const { return m_d; }
    void setD(qreal d);
    qreal t() const { return m_t; }
    void setT(qreal t);

    void prep();
    void render();

signals:
    void tChanged();
    void dChanged();

public slots:
    void sync();

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    int m_frame;
    GLuint m_vboIds[2];
    qreal m_d;
    qreal m_t;
    qreal m_thread_t;

};

#endif // CUBE_H
