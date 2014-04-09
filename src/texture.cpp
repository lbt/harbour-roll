#include "texture.h"

Texture::Texture(QImage img, QObject *parent) :
    QObject(parent)
  , m_img(img)
  , m_texture(NULL)
{
}

void Texture::bind() {
    if (m_texture) m_texture->bind();
}

// Declare an external hack window :: FIXME 5.2
#include <QQuickWindow>
extern QQuickWindow* global_hack_window;
void Texture::setupGL()
{
    glEnable(GL_TEXTURE_2D);
    m_texture = global_hack_window->createTextureFromImage(m_img);
    if (m_texture->isAtlasTexture()) { m_texture = m_texture->removedFromAtlas(); }
    m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    m_texture->setVerticalWrapMode(QSGTexture::Repeat);
}
