#ifndef VAOCONTAINER_H
#define VAOCONTAINER_H

#include <assimp/scene.h>
#include <assimp/postprocess.h>

class VAOContainer
{
public:
    VAOContainer(aiMesh *m);
    void* VAO();
    void* VAO_Indices();

private:
    int m_normal_loc;
    int m_tangent_loc;
    int m_bitangent_loc;
    int m_texture_loc;
    int m_color_loc;

    char *m_VAO;
    int* m_Indices;
};

#endif // VAOCONTAINER_H
