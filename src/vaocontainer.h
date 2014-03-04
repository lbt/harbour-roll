#ifndef VAOCONTAINER_H
#define VAOCONTAINER_H

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <QOpenGLShaderProgram>

class VAOContainer
{
public:
    VAOContainer(aiMesh *m);
    char* VAO() { return m_VAO ; }
    GLushort* VAO_Indices() { return m_Indices ; }
    int numIndices() { return m_isize ; }
    int numVertices() { return m_vsize ; }
    int stride(){ return m_vao_stride ; }

    int m_vao_stride;
    int m_vaosize;
    int m_pos_loc;
    int m_normal_loc;
    int m_tangent_loc;
    int m_bitangent_loc;
    int m_texture_loc;
    int m_color_loc;
    int m_isize;
    int m_vsize;
    char *m_VAO;
    GLushort* m_Indices;
};

#endif // VAOCONTAINER_H
