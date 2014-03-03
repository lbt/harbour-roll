#include "bimesh.h"

#include <QDebug>

#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


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

    BibtGImpactMeshShape* bimesh = new BibtGImpactMeshShape(btMesh, scene, node);
    bimesh->setMargin(0.01f);
    bimesh->updateBound();

    m_biShapes[node->mName.C_Str()] = bimesh;

    return bimesh;
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
{
    // FIXME - only copes with the first mesh
    aiMesh* m = scene->mMeshes[node->mMeshes[0]];
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
            m_texture = global_hack_window ->
                    createTextureFromImage(QImage(path.data).mirrored());
            //            texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            //            texture->setMagnificationFilter(QOpenGLTexture::Linear);
        } else {
            qDebug() << "and has no path";
        }
    }
}
