#include "texture.h"
#include "QDebug"

Texture::Texture(QString name, QImage img, QObject *parent) :
    QObject(parent)
  , m_img(img)
  , m_texture(NULL)
{
    setObjectName(name);
    if (m_img.isNull()) {
        qDebug() << "Image is null "  << objectName();
    }
}

void Texture::bind() {
    if (m_img.isNull()) {
        qDebug() << "Image is null "  << objectName();
        return;
    }
    if (m_texture) {
        m_texture->bind();
    }
}

// Declare an external hack window :: FIXME 5.2
#include <QQuickWindow>
extern QQuickWindow* global_hack_window;
void Texture::setupGL()
{
    qDebug() << "SetupGL " << objectName();
    if (m_img.isNull()) {
        qDebug() << "Image is null "  << objectName();
        return;
    }
    glEnable(GL_TEXTURE_2D);
    m_texture = global_hack_window->createTextureFromImage(m_img);
    if (m_texture->isAtlasTexture()) { m_texture = m_texture->removedFromAtlas(); }
    m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    m_texture->setVerticalWrapMode(QSGTexture::Repeat);
//    m_texture->bind();
}
