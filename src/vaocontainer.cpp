#include "vaocontainer.h"
#include <QDebug>

VAOContainer::VAOContainer(aiMesh* m)
{

    // Determine the storage size of each interleaved array element.
    int vsize = sizeof(aiVector3D);
    if (m->HasNormals()) {
        m_normal_loc = vsize;
        vsize += sizeof(aiVector3D);
    }

    if (m->HasTangentsAndBitangents())  {
        m_tangent_loc = vsize;
        vsize += sizeof(aiVector3D);
        m_bitangent_loc = vsize;
        vsize += sizeof(aiVector3D);
    }
    for (unsigned int i = 0; m->HasTextureCoords(i); ++i) {
        m_texture_loc = vsize;
        vsize += sizeof(aiVector3D);
    }
    for (unsigned int i = 0; m->HasVertexColors(i); ++i) {
        m_color_loc = vsize;
        vsize += sizeof(aiColor4D);
    }

    m_VAO = new char [vsize * m->mNumVertices];

    char* p = (char *)m_VAO;

    for (int n = 0; n < m->mNumVertices; ++n) {
        (aiVector3D)*p = m->mVertices[n];
        if (m->HasNormals()) {
            (aiVector3D)(*p + m_normal_loc) = m->mNormals[n];
        }

        if (m->HasTangentsAndBitangents()) {
            (aiVector3D)(*p + m_tangent_loc) = m->mTangents[n];
            (aiVector3D)(*p + m_bitangent_loc) = m->mBitangents[n];
        }

        for (unsigned int i = 0; m->HasTextureCoords(i); ++i) {
            (aiVector3D)(*p + m_texture_loc + i) = m->mTextureCoords[i][n];
        }

        for (unsigned int i = 0; m->HasVertexColors(i); ++i) {
            (aiColor4D)(*p + m_color_loc + i) = m->mColors[i][n];
        }
    }
    int isize = m->mNumFaces * 3; // Assume all faces are triangular (but verify!)
    m_Indices = new int [isize];
    int* ind = m_Indices;
    int nf = 0;
    for (aiFace* f = m->mFaces; nf < m->mNumFaces; nf++, f++) {
        unsigned int* f_ind = f->mIndices;
        if (f->mNumIndices != 3)
            qDebug() << "Error - non triangular face";
        *ind++ = *f_ind++;
        *ind++ = *f_ind++;
        *ind++ = *f_ind++;
    }
}
