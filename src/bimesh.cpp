#include "bimesh.h"

#include <QHash>
#include <QDebug>

#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "sailfishapp.h"

class myStream :
        public Assimp::LogStream
{
public:
    myStream() {}
    ~myStream() {}

    void write(const char* message) { qDebug() << message; }
};

static inline QVector3D qv3d(const aiVector3D &v) {
    return QVector3D(v.x, v.y, v.z);
}

inline static QMatrix4x4 getNodeMatrix(aiNode *node)
{
    QMatrix4x4 nodeMatrix;
    if (node->mTransformation.IsIdentity())
        return nodeMatrix;
    aiQuaternion rotation;
    aiVector3D position;
    aiVector3D scale;
    node->mTransformation.Decompose(scale, rotation, position);
    QVector3D qscale(scale.x,scale.y, scale.z);
    QVector3D qposition(position.x, position.y, position.z);
    QQuaternion qrotation(rotation.w, rotation.x, rotation.y, rotation.z);
    if (!qscale.isNull())
        nodeMatrix.scale(qscale);
    if (!qposition.isNull())
        nodeMatrix.translate(qposition);
    if (!qrotation.isNull())
        nodeMatrix.rotate(qrotation);
    return nodeMatrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief BiMeshContainer::BiMeshContainer
/// \param parent
///
BiMeshContainer::BiMeshContainer(QObject *parent) :
    QObject(parent)
{
}

bool BiMeshContainer::load(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();

    }


    // Create an instance of the Importer class
    Assimp::Importer importer;
    importer.SetExtraVerbose(true);
    // Attaching myStream to the default logger
    Assimp::DefaultLogger::create("",Assimp::Logger::VERBOSE);

    const unsigned int severity = Assimp::Logger::Debugging|Assimp::Logger::Info|Assimp::Logger::Err|Assimp::Logger::Warn;
    //Assimp::DefaultLogger::get()->attachStream( new myStream(), severity );
    Assimp::DefaultLogger::get()->setLogSeverity( Assimp::Logger::VERBOSE );
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    const aiScene* scene = importer.ReadFile( filename.toStdString(),
                                              aiProcess_CalcTangentSpace |
                                              aiProcess_Triangulate |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_SortByPType|
                                              aiProcess_ValidateDataStructure|
                                              aiProcess_FindDegenerates
                                              );
    qDebug() << "Parsed " << filename;

    // If the import failed, report it
    if( !scene)
    {
        qDebug() << importer.GetErrorString();
        return false;
    }

    qDebug() << "There are " << scene->mNumTextures << " textures in the file and " << scene->mNumMaterials << " materials";

    // Now we can access the file's contents.
    m_top = importChildren(scene, scene->mRootNode, NULL, QMatrix4x4());

    Assimp::DefaultLogger::kill();
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

BiMesh * BiMeshContainer::importChildren(const aiScene *scene, aiNode *node, BiMesh *targetParent, QMatrix4x4 accTransform)  {
    QMatrix4x4 transform;
    BiMesh* parent = NULL;
    BiMesh* top = NULL;

    qDebug() << "Processing node " << node->mName.C_Str();

    // if node has meshes, create a new scene object for it
    if( node->mNumMeshes > 0)
    {
        qDebug() << "found a full node with " << node->mNumMeshes << " meshes";
        // targetParent.addChild( newObject); // violates threading tree rules
        // copy the meshes
        BiMesh *newObject = nodeToMesh(scene, node);
        // the new object is the parent for all child nodes
        if (!parent and !targetParent) top = newObject;
        parent = newObject;
    } else  {
        // if no meshes, skip the node, but keep its transformation
        parent = targetParent;
        transform = getNodeMatrix(node) * accTransform;
    }
    // continue for all child nodes
    for( unsigned int a = 0; a < node->mNumChildren; a++) {
        qDebug() << "found a child mesh";
        importChildren(scene, node->mChildren[a], parent, transform);
    }

    return top;

    //if (!m_diceShape[die]) {
    //    m_diceShape[die] = new btBoxShape(btVector3(.5,.5,.5));
    //    collisionShapes.push_back(m_diceShape[die]);
    //}
}

uint qHash(const aiVector3t<float> &v) { //
    return  ((int)(v.x * 73856093) ^
             (int)(v.y * 19349663) ^
             (int)(v.z * 83492791))
            % (uint)-1 ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief BiMeshContainer::copyMeshes
/// \param scene
/// \param node
///
/// First create a btMesh then use it to create a suitable BiMesh
///
BiMesh* BiMeshContainer::nodeToMesh(const aiScene *scene, aiNode *node)
{
    // Objective is to create a single VAO - look at the Vertex object (not in docs - see code/Vertex.h)

    // FIXME - only copes with the first mesh
    aiMesh* m = scene->mMeshes[node->mMeshes[0]];
    qDebug() << "Processing mesh of " << m->mNumVertices << " vertices and " << m->mNumFaces << " faces to : " << node->mName.C_Str();
    btTriangleMesh* btMesh = new btTriangleMesh(true,false);

    aiFace* f = m->mFaces;
    for (unsigned int nf = 0 ; nf < m->mNumFaces; nf++, f++) {
        unsigned int* ind = f->mIndices;
        if (f->mNumIndices != 3)
            qDebug() << "Error - non triangular face";
        btVector3 v1 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        ind++;
        btVector3 v2 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        ind++;
        btVector3 v3 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        btMesh->addTriangle(v1, v2, v3);
    }

    // we can now theoretically test the mesh to see if it's concave/convex and either make a btGimpactMesh
    // or a btConvexHull
    // For now we'll make a btGimpactMesh

//#define USE_IMPACT_MESH
#ifdef USE_IMPACT_MESH
    qDebug() <<"ImpactMesh";
    BibtGImpactMeshShape* bimesh = new BibtGImpactMeshShape(btMesh, scene, node);
    bimesh->setMargin(0.04f);
    bimesh->updateBound();
#else
    qDebug() <<"ConvexHull";
    BibtConvexHullShape* bimesh = new BibtConvexHullShape(scene, node, NULL);
    QHash<aiVector3D, bool> seen;
    for(unsigned int n=0; n < m->mNumVertices; n++) {
        if (seen.contains(m->mVertices[n])) { continue; }
        seen[m->mVertices[n]] = true;
        aiVector3D &v = m->mVertices[n];
        bimesh->addPoint(btVector3(v.x*0.92,v.y*0.92,v.z*0.92),false);
    }
    bimesh->recalcLocalAabb();
    bimesh->setMargin(0.04f);
#endif


    m_biShapes[node->mName.C_Str()] = bimesh;

    return bimesh;
}

QList<QString> BiMeshContainer::getNames(){
    return m_biShapes.keys();
}


btCollisionShape* BiMeshContainer::getCollisionMesh(QString name)
{
    return dynamic_cast<btCollisionShape*>(m_biShapes[name]);
}
BiMesh* BiMeshContainer::getBiMesh(QString name)
{
    return m_biShapes[name];
}

// Declare an external hack window
#include <QQuickWindow>
QQuickWindow* global_hack_window;
///////////////////////////////////////////////////////////////////////////////////
/// \brief BiMesh::BiMesh
/// \param parent
///
BiMesh::BiMesh(const aiScene *scene, aiNode *node, QObject *parent) :
    QObject(parent)
  , m_texture(NULL)
{
    // FIXME - only copes with the first mesh
    aiMesh* m = scene->mMeshes[node->mMeshes[0]];

    m_name = node->mName.C_Str();

    // construct an interleaved matrix for feeding to glVertexAttribPointer and glDrawElements
    m_vao = new VAOContainer(m);

    // Import a texture
    int nTex = m->GetNumUVChannels();
    if (nTex > 0) {// http://assimp.sourceforge.net/lib_html/materials.html
        qDebug() << "Found " << nTex << " texture (UV) channels in material[" << m->mMaterialIndex << "]";
        aiMaterial *mat = scene->mMaterials[m->mMaterialIndex];

        aiString name;
        mat->Get(AI_MATKEY_NAME, name);
        aiString path;
        qDebug() << "I think it's called " << name.C_Str();
        if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            qDebug() << "and lives at " << path.C_Str();
            // FIXME at 5.2:
            //            m_texture = new QOpenGLTexture(QImage(path.data).mirrored());
            glEnable(GL_TEXTURE_2D);
            m_texture = global_hack_window ->
                    createTextureFromImage(QImage(SailfishApp::pathTo(path.data).toLocalFile()).mirrored());
            if (m_texture->isAtlasTexture()) {
                qDebug() << "Removed texture from Atlas";
                m_texture = m_texture->removedFromAtlas();
            }

//            m_texture->setFiltering(QSGTexture::Linear);
//            m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
//            m_texture->setVerticalWrapMode(QSGTexture::Repeat);

//            // Set nearest filtering mode for texture minification
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            // Set bilinear filtering mode for texture magnification
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            // Wrap texture coordinates by repeating
//            // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //            texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            //            texture->setMagnificationFilter(QOpenGLTexture::Linear);
        } else {
            qDebug() << "and has no path";
        }
    }
}

void BiMesh::setup(GLProgram* p) {

    glGenBuffers(2, m_vboIds); // one for VAO, other for indices
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vao->m_vaosize, m_vao->VAO(), GL_DYNAMIC_DRAW);
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
    //             << " = " << (void *)((const char *)m_vao->VAO()+m_vao->m_pos_loc);
    glVertexAttribPointer(p->getA("posA"), 3, GL_FLOAT, GL_FALSE, m_vao->stride(),
                          m_vao->VAO()+m_vao->m_pos_loc);

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
                          m_vao->VAO()+m_vao->m_normal_loc);

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
                          m_vao->VAO()+m_vao->m_texture_loc);

    //    if (m_vao->m_texture_loc >= 0) {
    //        char* p = ((char *)m_vao->VAO()+m_vao->m_texture_loc);
    //        for (int c; c<m_vao->numVertices(); c++) {
    //            float* f = (float*)p;
    //            qDebug() << "texA (" << *f << "," << f[1] << "," << f[2] << ")";
    //            p += m_vao->stride();
    //        }
    //    }


    glEnable(GL_TEXTURE_2D);

}


// The approach that's likely to work is to define the app's model format and which attribs
// must be used. Then preset all the lighting in one area and then render each object

void BiMesh::render(GLProgram* p)
{
    if (!m_texture) {
        qDebug() << "no texture, not rendering " << m_name;
        return;
    }
    //    qDebug() << "rendering a " << m_name;
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

    m_texture->bind();

    glDrawElements(GL_TRIANGLES, m_vao->numIndices(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(p->getA("posA"));
    glDisableVertexAttribArray(p->getA("texA"));
    glDisableVertexAttribArray(p->getA("normalA"));
}
