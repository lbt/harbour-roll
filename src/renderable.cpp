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
Renderable::Renderable(QString name, VAO *v, Texture *t, QObject *parent) :
    QObject(parent)
  , m_shader(NULL)
  , m_vao(v)
  , m_texture(t)
{
    setObjectName(name);
    if (!m_texture) {
        qDebug() << "no Texture";
    }
    if (!m_vao) {
        qDebug() << "no VAO";
    }
}

void Renderable::setupGL() {
    qDebug() << "Setup GL";
    if (!m_shader) {
        qDebug() << "No shader";
        return;
    }
    GLProgram* p = m_shader->getProgram();

    glGenBuffers(2, m_vboIds); // one for VAO, other for indices
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vao->m_vaosize, m_vao->getVAO(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vao->numIndices() * sizeof(GLushort), m_vao->VAO_Indices(), GL_STATIC_DRAW);

    //    GLushort* p = m_vao->VAO_Indices();
    //    for (int c; c<m_vao->numIndices();c+=3) {
    //        qDebug() << "Found face (" << *p++ << "," << *p++ << "," << *p++ << ")";
    //    }

    //    glGenTextures(1, m_texIds);
    //    glBindTexture(GL_TEXTURE_2D);

    //    qDebug() << "VAO stride is :"<<  m_vao->stride();
    //    qDebug() << "PosA points to base:"<< (const void *)m_vao->VAO() << " offset " << m_vao->m_pos_loc
    //             << " = " << (void *)((const char *)m_vao->getVAO()+m_vao->m_pos_loc);
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          m_vao->getVAO()+m_vao->m_pos_loc);

    //    if (m_vao->m_pos_loc >= 0) {
    //        char* p = (m_vao->VAO()+m_vao->m_pos_loc);
    //        for (int c; c<m_vao->numVertices(); c++) {
    //            float* f = (float*)p;
    //            qDebug() << "PosA (" << *f << "," << f[1] << "," << f[2] << ")";
    //            p += m_vao->stride();
    //        }
    //    }
    //    qDebug() << "normalA points to base:"<< (const void *)m_vao->VAO() << " offset " << m_vao->m_normal_loc
    //             << " = " << (void *)((const char *)m_vao->VAO()+m_vao->m_normal_loc);
    glVertexAttribPointer(p->getA("normalA"), 3, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          m_vao->getVAO()+m_vao->m_normal_loc);

    //    if (m_vao->m_normal_loc >= 0) {

    //        char* p = ((char *)m_vao->VAO()+m_vao->m_normal_loc);
    //        for (int c; c<m_vao->numVertices(); c++) {
    //            float* f = (float*)p;
    //            qDebug() << "normalA (" << *f << "," << f[1] << "," << f[2] << ")";
    //            p += m_vao->stride();
    //        }
    //    }
    //    qDebug() << "texA points to base:"<< (const void *)m_vao->VAO() << " offset " << m_vao->m_texture_loc
    //             << " = " << (void *)((const char *)m_vao->VAO()+m_vao->m_texture_loc);
    glVertexAttribPointer(p->getA("texA"), 2, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          m_vao->getVAO()+m_vao->m_texture_loc);

    //    if (m_vao->m_texture_loc >= 0) {
    //        char* p = ((char *)m_vao->VAO()+m_vao->m_texture_loc);
    //        for (int c; c<m_vao->numVertices(); c++) {
    //            float* f = (float*)p;
    //            qDebug() << "texA (" << *f << "," << f[1] << "," << f[2] << ")";
    //            p += m_vao->stride();
    //        }
    //    }

}


// The approach that's likely to work is to define the app's model format and which attribs
// must be used. Then preset all the lighting in one area and then render each object

void Renderable::render(const Shader *activeShader)
{
    if (activeShader != m_shader) return; // This is not our program

    GLProgram* p = m_shader->getProgram();

//    qDebug() << "rendering a " << objectName();
    // Switch to using our buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // This is probably the same for all VAOs at the moment - it may change if some
    // VAOs have different layouts
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          (const GLvoid*)m_vao->m_pos_loc);
    glVertexAttribPointer(p->getA("normalA"), 3, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          (const GLvoid*)m_vao->m_normal_loc);
    glVertexAttribPointer(p->getA("texA"), 2, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          (const GLvoid*)m_vao->m_texture_loc);

    glEnableVertexAttribArray(p->getA("posA"));
    glEnableVertexAttribArray(p->getA("texA"));
    glEnableVertexAttribArray(p->getA("normalA"));

    p->setUniformValue(p->getU("matSpecularIntensityU"), 2.0f);
    p->setUniformValue(p->getU("specularPowerU"), 32.0f);

    m_texture->bind();

    glDrawElements(GL_TRIANGLES, m_vao->numIndices(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));
}
