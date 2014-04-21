#include "renderable.h"
#include <QHash>
#include <QDebug>

#include "sailfishapp.h"

#include "utils.h"

///////////////////////////////////////////////////////////////////
/// \brief Renderable::Renderable
/// \param scene
/// \param node
/// \param parent
///
Renderable::Renderable(QString name, VAO *v, QObject *parent) :
    QObject(parent)
  , m_shader(NULL)
  , m_vao(v)
  , m_opaque(true)

{
    setObjectName(name);
    if (!m_vao) {
        qDebug() << "no VAO";
    }
}

