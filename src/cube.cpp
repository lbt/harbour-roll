#include "cube.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

#define DEPLOYPATH "/opt/sdk/harbour-snowglobe/usr/share/harbour-snowglobe/"

struct VertexData
{
#define VertexData_0 (0)
    QVector3D position;
#define VertexData_1 (sizeof(QVector3D))
    QVector2D texCoord;
#define VertexData_2 (Vertex_Data_2+sizeof(QVector2D))
};

Cube::Cube(QObject *parent) :
    GLItem()
  , m_t(0)
  , m_d(0)
  , m_thread_t(0)
  , m_frame(0)
{
}

void Cube::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();

}
void Cube::setD(qreal d)
{
    if (d == m_d)
        return;
    m_d = d;
    emit dChanged();
}

void Cube::prep()
{
    // prep must add and link any shaders
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                       DEPLOYPATH "cube_vert.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                       DEPLOYPATH "cube_frag.glsl");

    // prep must bind any attributes
    m_program->bindAttributeLocation("vertices", 0);

    if (! m_program->link()) {
        qDebug() << "Linking failed\n" << m_program->log();
    }

    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    // and then prepare any one-time data like VBOs
    glGenBuffers(2, m_vboIds);

    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D( 0.9,  0.9,  1.0), QVector2D(0.0, 0.0)},  // v0
        {QVector3D( 1.0,  0.9,  1.0), QVector2D(0.33, 0.0)}, // v1
        {QVector3D( 0.9,  1.0,  1.0), QVector2D(0.0, 0.5)},  // v2
        {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.33, 0.5)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0,  0.9,  1.0), QVector2D( 0.0, 0.5)}, // v4
        {QVector3D( 1.0,  0.9,  0.9), QVector2D(0.33, 0.5)}, // v5
        {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.0, 1.0)},  // v6
        {QVector3D( 1.0,  1.0,  0.9), QVector2D(0.33, 1.0)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0,  0.9,  0.9), QVector2D(0.66, 0.5)}, // v8
        {QVector3D( 0.9,  0.9,  0.9), QVector2D(1.0, 0.5)},  // v9
        {QVector3D( 1.0,  1.0,  0.9), QVector2D(0.66, 1.0)}, // v10
        {QVector3D( 0.9,  1.0,  0.9), QVector2D(1.0, 1.0)},  // v11

        // Vertex data for face 3
        {QVector3D( 0.9,  0.9,  0.9), QVector2D(0.66, 0.0)}, // v12
        {QVector3D( 0.9,  0.9,  1.0), QVector2D(1.0, 0.0)},  // v13
        {QVector3D( 0.9,  1.0,  0.9), QVector2D(0.66, 0.5)}, // v14
        {QVector3D( 0.9,  1.0,  1.0), QVector2D(1.0, 0.5)},  // v15

        // Vertex data for face 4
        {QVector3D( 0.9,  0.9,  0.9), QVector2D(0.33, 0.0)}, // v16
        {QVector3D( 1.0,  0.9,  0.9), QVector2D(0.66, 0.0)}, // v17
        {QVector3D( 0.9,  0.9,  1.0), QVector2D(0.33, 0.5)}, // v18
        {QVector3D( 1.0,  0.9,  1.0), QVector2D(0.66, 0.5)}, // v19

        // Vertex data for face 5
        {QVector3D( 0.9,  1.0,  1.0), QVector2D(0.33, 0.5)}, // v20
        {QVector3D( 1.0,  1.0,  1.0), QVector2D(0.66, 0.5)}, // v21
        {QVector3D( 0.9,  1.0,  0.9), QVector2D(0.33, 1.0)}, // v22
        {QVector3D( 1.0,  1.0,  0.9), QVector2D(0.66, 1.0)}, // v23

        // Vertex data for face 0
        {QVector3D(-1.0, -1.0, -0.9), QVector2D(0.0, 0.0)},  // v0
        {QVector3D(-0.9, -1.0, -0.9), QVector2D(0.33, 0.0)}, // v1
        {QVector3D(-1.0, -0.9, -0.9), QVector2D(0.0, 0.5)},  // v2
        {QVector3D(-0.9, -0.9, -0.9), QVector2D(0.33, 0.5)}, // v3

        // Vertex data for face 1
        {QVector3D(-0.9, -1.0, -0.9), QVector2D( 0.0, 0.5)}, // v4
        {QVector3D(-0.9, -1.0, -1.0), QVector2D(0.33, 0.5)}, // v5
        {QVector3D(-0.9, -0.9, -0.9), QVector2D(0.0, 1.0)},  // v6
        {QVector3D(-0.9, -0.9, -1.0), QVector2D(0.33, 1.0)}, // v7

        // Vertex data for face 2
        {QVector3D(-0.9, -1.0, -1.0), QVector2D(0.66, 0.5)}, // v8
        {QVector3D(-1.0, -1.0, -1.0), QVector2D(1.0, 0.5)},  // v9
        {QVector3D(-0.9, -0.9, -1.0), QVector2D(0.66, 1.0)}, // v10
        {QVector3D(-1.0, -0.9, -1.0), QVector2D(1.0, 1.0)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0, -1.0, -1.0), QVector2D(0.66, 0.0)}, // v12
        {QVector3D(-1.0, -1.0, -0.9), QVector2D(1.0, 0.0)},  // v13
        {QVector3D(-1.0, -0.9, -1.0), QVector2D(0.66, 0.5)}, // v14
        {QVector3D(-1.0, -0.9, -0.9), QVector2D(1.0, 0.5)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0, -1.0, -1.0), QVector2D(0.33, 0.0)}, // v16
        {QVector3D(-0.9, -1.0, -1.0), QVector2D(0.66, 0.0)}, // v17
        {QVector3D(-1.0, -1.0, -0.9), QVector2D(0.33, 0.5)}, // v18
        {QVector3D(-0.9, -1.0, -0.9), QVector2D(0.66, 0.5)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0, -0.9, -0.9), QVector2D(0.33, 0.5)}, // v20
        {QVector3D(-0.9, -0.9, -0.9), QVector2D(0.66, 0.5)}, // v21
        {QVector3D(-1.0, -0.9, -1.0), QVector2D(0.33, 1.0)}, // v22
        {QVector3D(-0.9, -0.9, -1.0), QVector2D(0.66, 1.0)}  // v23
    };
    GLushort indices[] = {
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
       12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
       16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
       20, 20, 21, 22, 23,      // Face 5 - triangle strip (v20, v21, v22, v23)
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
       12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
       16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
       20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 34 * sizeof(GLushort), indices, GL_STATIC_DRAW);
}

void Cube::render()
{
    QMatrix4x4 matrix;
    matrix.perspective(60, 540.0/960.0, 0.1, 100.0);
    matrix.translate(0, 0, m_d);
    matrix.rotate(100.0f * m_frame / 60, 0, 1, 0);
    matrix.rotate(100.0f * m_frame / 400, 0, 0, 1);
    matrix.rotate(100.0f * m_frame / 100, 1, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Because we're using VBOs this is pointer into them
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_0);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                          (const void *)VertexData_1);

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colAttr);

//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 23);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(0, 0.2, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(0, -0.4, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(0.2, 0.2, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(-0.4, 0, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(0.2, 0, 0.2);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    matrix.translate(0, 0, -0.4);
    m_program->setUniformValue(m_matrixUniform, matrix);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);


    glDisableVertexAttribArray(m_posAttr);
    glDisableVertexAttribArray(m_colAttr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glClearDepthf(0.0);
//    glClear(GL_DEPTH_BUFFER_BIT);
//    glFlush();

}
void Cube::sync()
{
    m_thread_t = m_t;
    ++m_frame;
}
