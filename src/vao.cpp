#include "vao.h"
#include <QDebug>

inline void copyV3toLocation(aiVector3D *v, void* p)
{ float* f = (float*)p; f[0] = v->x; f[1] = v->y; f[2] = v->z; }
inline void copyV4toLocation(aiColor4D *v, void* p)
{ float* f = (float*)p; f[0] = v->r; f[1] = v->g; f[2] = v->b; f[3] = v->a; }
inline QVector3D locationtoQt3D(void* p)
{ float* f = (float*)p; return QVector3D(f[0], f[1], f[2]); }

VAO::VAO(aiMesh* m) :
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

    m_numFaces = m->mNumFaces;
    m_isize = m->mNumFaces * 3; // Assume all faces are triangular (but verify!)
    m_Indices = new GLushort [m_isize];
    GLushort* ind = m_Indices;
    unsigned int nf = 0;
    m_isize=0;
    for (aiFace* f = m->mFaces; nf < m->mNumFaces; nf++, f++) {
        unsigned int* f_ind = f->mIndices;
        m_numVertPerFaces = f->mNumIndices;
        //        if (f->mNumIndices != 3)
        //            qDebug() << "Error - non triangular face";
        //        qDebug() << "Face (" << f_ind[0] << "," << f_ind[1] << "," << f_ind[2] << ")";
        for (unsigned int i=0; i < f->mNumIndices; i++ ) {
            m_isize++;
            *ind++ = *f_ind++;
        }
    }
}

void VAO::faceAt(int n, QVector3D *f) {
    Q_ASSERT(n<m_isize);
//    qDebug() << "base " << m_VAO+m_pos_loc;
//    qDebug() << "Indices base " << m_Indices;
//    qDebug() << "I offset " << m_Indices + n;
//    qDebug() << "base offset " << *(m_Indices + n);
//    qDebug() << "n " << n ;
    n *= m_numVertPerFaces;
    f[0] = locationtoQt3D(m_VAO+m_pos_loc +
                          (*(m_Indices + n) * m_vao_stride));
    if (m_numVertPerFaces == 1) return;
    f[1] = locationtoQt3D(m_VAO+m_pos_loc +
                          (*(m_Indices + n +1) * m_vao_stride));
    if (m_numVertPerFaces == 2) return;
    f[2] = locationtoQt3D(m_VAO+m_pos_loc +
                          (*(m_Indices + n +2) * m_vao_stride));
    if (m_numVertPerFaces == 3) return;
    f[3] = locationtoQt3D(m_VAO+m_pos_loc +
                          (*(m_Indices + n +3) * m_vao_stride));
    return;
}

