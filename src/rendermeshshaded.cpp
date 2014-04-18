#include "rendermeshshaded.h"

RenderMeshShaded::RenderMeshShaded(QString name, VAO *v, QObject *parent) :
    Renderable(name, v, parent)
  , m_colorF(0.5, 0.5, 0.5, 0.8)
  , m_colorB(0.2, 0.2, 0.2, 1.0)
  , m_specPowerF(2)
  , m_specPowerB(32)
  , m_specIntensityF(1)
  , m_specIntensityB(8)
{
}

void RenderMeshShaded::setupGL() {
    qDebug() << "Setup GL for " << objectName();
    if (!m_shader) {
        qDebug() << "No shader";
        return;
    }
    GLProgram* p = m_shader->getProgram();

    // bind buffers to GL_ARRAY_BUFFER and then upload mesh data
    // VA pointers are defined as they're used
    glGenBuffers(2, m_vboIds); // one for VAO, other for indices
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vao->m_vaosize, m_vao->getVAO(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vao->numIndices() * sizeof(GLushort), m_vao->VAO_Indices(), GL_STATIC_DRAW);
}


// The approach that's likely to work is to define the app's model format and which attribs
// must be used. Then preset all the lighting in one area and then render each object

void RenderMeshShaded::render(const Shader *activeShader, QMatrix4x4 worldMatrix)
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

    glEnableVertexAttribArray(p->getA("posA"));
    glEnableVertexAttribArray(p->getA("normalA"));

    p->setUniformValue(p->getU("worldMatrixU"), worldMatrix);

    p->setUniformValue(p->getU("matColorFU"), m_colorF);
    p->setUniformValue(p->getU("matColorBU"), m_colorB);
    p->setUniformValue(p->getU("matSpecularIntensityFU"), m_specIntensityF);
    p->setUniformValue(p->getU("matSpecularIntensityBU"), m_specIntensityB);
    p->setUniformValue(p->getU("specularPowerFU"), m_specPowerF);
    p->setUniformValue(p->getU("specularPowerBU"), m_specPowerB);

    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, m_vao->numIndices(), GL_UNSIGNED_SHORT, 0);
    glEnable(GL_CULL_FACE);

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("normalA"));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
