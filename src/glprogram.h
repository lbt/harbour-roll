#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QUrl>

class GLProgram : public QOpenGLShaderProgram
{
    Q_OBJECT
public:
    explicit GLProgram(QUrl vpath, QUrl fpath, QObject *parent = 0);

    GLuint getU(QString);
    GLuint getA(QString);
//    GLuint setU(QString);
//    GLuint setA(QString);
//    void dirty(); // Call when linking

signals:

public slots:

protected:
    QHash<QString, GLuint> m_uniforms;
    QHash<QString, GLuint> m_attributes;
};

#endif // GLPROGRAM_H
