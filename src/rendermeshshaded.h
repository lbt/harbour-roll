#ifndef RENDERMESHSHADED_H
#define RENDERMESHSHADED_H
#include "renderable.h"

class RenderMeshShaded : public Renderable
{
    Q_OBJECT
public:
    explicit RenderMeshShaded(QString name, VAO *v, QObject *parent = 0);
    void setupGL();
    void render(const Shader *activeShader, QMatrix4x4 worldMatrix);

    void setFaceColours(QVector4D front, QVector4D back){ m_colorF = front; m_colorB = back; }
    void setSpecularIntensity(qreal front, qreal back) {m_specIntensityF=front;m_specIntensityB=back;}
    void setSpecularPower(qreal front, qreal back){ m_specPowerF=front;m_specPowerB=back;}
signals:

public slots:

protected:
    QVector4D m_colorF;
    QVector4D m_colorB;
    qreal m_specPowerF;
    qreal m_specPowerB;
    qreal m_specIntensityF;
    qreal m_specIntensityB;

};

#endif // RENDERMESHSHADED_H
