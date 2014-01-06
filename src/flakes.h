#ifndef FLAKES_H
#define FLAKES_H

#include "glitem.h"

class Flakes : public GLItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    explicit Flakes(QObject *parent = 0);
    qreal t() const { return m_t; }
    void setT(qreal t);

    void prep();
    void render();

signals:
    void tChanged();

public slots:
    void sync();


private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    int m_frame;
    GLuint m_vboIds[2];
    qreal m_t;
    qreal m_thread_t;

};

#endif // FLAKES_H