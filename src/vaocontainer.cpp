#include "vaocontainer.h"
#include <QDebug>

inline void copyV3toLocation(aiVector3D *v, void* p)
{ float* f = (float*)p; f[0] = v->x; f[1] = v->y; f[2] = v->z; }
inline void copyV4toLocation(aiColor4D *v, void* p)
{ float* f = (float*)p; f[0] = v->r; f[1] = v->g; f[2] = v->b; f[3] = v->a; }

VAOContainer::VAOContainer(aiMesh* m) :
    m_pos_loc(-1)
  , m_normal_loc(-1)
  , m_tangent_loc(-1)
  , m_bitangent_loc(-1)
  , m_texture_loc(-1)
  , m_color_loc(-1)
{

    // Determine the storage size of each interleaved array element.
    m_pos_loc = 0;
    m_vao_stride = sizeof(float[3]);
    if (m->HasNormals()) {
        m_normal_loc = m_vao_stride;
        m_vao_stride += sizeof(float[3]);
    }

    //    if (m->HasTangentsAndBitangents())  {
    //        m_tangent_loc = m_vao_stride;
    //        m_vao_stride += sizeof(float[3]);
    //        m_bitangent_loc = m_vao_stride;
    //        m_vao_stride += sizeof(float[3]);
    //    }
    for (unsigned int i = 0; m->HasTextureCoords(i); ++i) {
        m_texture_loc = m_vao_stride;
        m_vao_stride += sizeof(float[3]);
    }
    for (unsigned int i = 0; m->HasVertexColors(i); ++i) {
        m_color_loc = m_vao_stride;
        m_vao_stride += sizeof(float[4]);
    }
    m_vaosize = m_vao_stride * m->mNumVertices;
    m_vsize = m->mNumVertices;
    m_VAO = new char [m_vaosize];

    char* p = m_VAO;
    aiVector3D* v;
    for (unsigned int n = 0; n < m->mNumVertices; ++n) {
        v = m->mVertices+n;
        copyV3toLocation(v, p);
//        qDebug() << "Adding vertex ("<< v->x <<","<< v->y <<","<< v->z <<")";
        if (m->HasNormals()) {
            v = m->mNormals+n;
            copyV3toLocation(v, p + m_normal_loc);
//            qDebug() << "Adding normal ("<< v->x <<","<< v->y <<","<< v->z <<")";
        }

        //        if (m->HasTangentsAndBitangents()) {
        //            (aiVector3D)(*p + m_tangent_loc) = m->mTangents[n];
        //            (aiVector3D)(*p + m_bitangent_loc) = m->mBitangents[n];
        //            *v = m->mBitangents[n];
        //            qDebug() << "Adding bitan ("<< v->x <<","<< v->y <<","<< v->z <<")";
        //        }

        for (unsigned int i = 0; m->HasTextureCoords(i); ++i) {
            v = &(m->mTextureCoords[i][n]);
            copyV3toLocation(v, p + m_texture_loc + i*sizeof(float[3]));
//            qDebug() << "Adding texv ("<< v->x <<","<< v->y <<","<< v->z <<")";
        }

        for (unsigned int i = 0; m->HasVertexColors(i); ++i) {
            aiColor4D &v4 = m->mColors[i][n];
            copyV4toLocation(&v4, p + m_color_loc + i*sizeof(float[4]));
        }
        p += m_vao_stride;
    }

    m_isize = m->mNumFaces * 3; // Assume all faces are triangular (but verify!)
    m_Indices = new GLushort [m_isize];
    GLushort* ind = m_Indices;
    unsigned int nf = 0;
    for (aiFace* f = m->mFaces; nf < m->mNumFaces; nf++, f++) {
        unsigned int* f_ind = f->mIndices;
        if (f->mNumIndices != 3)
            qDebug() << "Error - non triangular face";
//        qDebug() << "Face (" << f_ind[0] << "," << f_ind[1] << "," << f_ind[2] << ")";
        *ind++ = *f_ind++;
        *ind++ = *f_ind++;
        *ind++ = *f_ind++;
    }
}

