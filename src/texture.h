#ifndef TEXTURE_H
#define TEXTURE_H

#include <QObject>
#include <QSGTexture>
#include <QImage>

class Texture : public QObject
{
    Q_OBJECT
public:
    explicit Texture(QImage img, QObject *parent = 0);

    void setupGL();
    void bind();
signals:

public slots:

    private:
    QSGTexture* m_texture;
    QImage m_img;
};

#endif // TEXTURE_H
