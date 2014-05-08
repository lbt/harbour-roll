#include "rendermeshtextured.h"
#include "shader.h"
#include <QDebug>

///////////////////////////////////////////////////////////////////
/// \brief RenderMeshTextured::RenderMeshTextured
/// \param scene
/// \param node
/// \param parent
///
RenderMeshTextured::RenderMeshTextured(QString name, VAO *v, Texture *t, QObject *parent) :
    Renderable(name, v, parent)
  , m_texture(t)
{
    if (!m_texture) {
        qDebug() << "no Texture";
    }
}

void RenderMeshTextured::setupGL() {
    qDebug() << "Setup GL for " << objectName();
    if (!m_shader) {
        qDebug() << "No shader";
        return;
    }
    // This is safe to call multiple times
    m_shader->setupGL();

    glGenBuffers(2, m_vboIds); // one for VAO, other for indices
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vao->m_vaosize, m_vao->getVAO(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vao->numIndices() * sizeof(GLushort), m_vao->VAO_Indices(), GL_STATIC_DRAW);
}


// The approach that's likely to work is to define the app's model format and which attribs
// must be used. Then preset all the lighting in one area and then render each object

void RenderMeshTextured::render(const Shader *activeShader, QMatrix4x4 worldMatrix)
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

    p->setUniformValue(p->getU("worldMatrixU"), worldMatrix);

    p->setUniformValue(p->getU("matSpecularIntensityU"), 2.0f);
    p->setUniformValue(p->getU("specularPowerU"), 32.0f);

    m_texture->bind();

    glDrawElements(GL_TRIANGLES, m_vao->numIndices(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
