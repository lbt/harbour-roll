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

QMap<QString, btConvexHullShape*> BiMesh::c_bShape;

BiMesh::BiMesh(QObject *parent) :
    QObject(parent)
{
}

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
bool BiMesh::load(QString filename)
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
    Assimp::DefaultLogger::get()->attachStream( new myStream(), severity );
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
    // Now we can access the file's contents.
    importChildren(scene, scene->mRootNode, this, QMatrix4x4());

    Assimp::DefaultLogger::kill();
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

void BiMesh::importChildren(const aiScene *scene, aiNode *node, BiMesh *targetParent, QMatrix4x4 accTransform)  {
    BiMesh *parent;
    QMatrix4x4 transform;

    qDebug() << "Processing node " << node->mName.C_Str();

    // if node has meshes, create a new scene object for it
    if( node->mNumMeshes > 0)
    {
        qDebug() << "found a full node with " << node->mNumMeshes << " meshes";
        BiMesh* newObject = new BiMesh(targetParent); // targetParent.addChild( newObject);
        // copy the meshes
        //        newObject->copyMeshes(scene, node);
        copyMeshes(scene, node);
        // the new object is the parent for all child nodes
        parent = newObject;
        // transform.SetUnity(); // Qt Matrix is identity by default.
    } else  {
        // if no meshes, skip the node, but keep its transformation
        parent = targetParent;
        transform = getNodeMatrix(node) * accTransform;
    }
    // continue for all child nodes
    for( int a = 0; a < node->mNumChildren; a++) {
        qDebug() << "found a child mesh";
        importChildren(scene, node->mChildren[a], parent, transform);
    }


    //if (!m_diceShape[die]) {
    //    m_diceShape[die] = new btBoxShape(btVector3(.5,.5,.5));
    //    collisionShapes.push_back(m_diceShape[die]);
    //}
}

void BiMesh::copyMeshes(const aiScene *scene, aiNode *node)
{
    // FIXME - only copes with the first mesh
    aiMesh* m = scene->mMeshes[node->mMeshes[0]];
    qDebug() << "Processing mesh of " << m->mNumVertices << " vertices and " << m->mNumFaces << " faces to : " << node->mName.C_Str();
    btConvexHullShape* btShape = new btConvexHullShape();

    aiFace* f = m->mFaces;
    for (int nf = 0 ; nf < m->mNumFaces; nf++, f++) {
        unsigned int* ind = f->mIndices;
        for (int vi = 0; vi < f->mNumIndices; vi++, ind++) {
            aiVector3D* v = m->mVertices+*ind;
            btShape->addPoint(btVector3(v->x, v->y, v->z));
        }
    }
    btShape->recalcLocalAabb();
    c_bShape[node->mName.C_Str()] = btShape;
}

btConvexHullShape* BiMesh::getMesh(QString name)
{
    return c_bShape[name];
}


