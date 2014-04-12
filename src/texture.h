#ifndef TEXTURE_H
#define TEXTURE_H

#include <QObject>
#include <QSGTexture>
#include <QImage>

class Texture : public QObject
{
    Q_OBJECT
public:
    explicit Texture(QString name, QImage img, QObject *parent = 0);

    bool isNull(){ return m_img.isNull(); }

    void setupGL();
    void bind();
signals:

public slots:

private:
    QSGTexture* m_texture;
    QImage m_img;
};

#endif // TEXTURE_H
