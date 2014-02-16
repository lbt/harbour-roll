#include "bimesh.h"

#include <QDebug>

BiMesh::BiMesh(QObject *parent) :
    QObject(parent)
{
    m_bShape = new btConvexHullShape();
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
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( filename.toStdString(),
                                              aiProcess_CalcTangentSpace |
                                              aiProcess_Triangulate |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_SortByPType);
    qDebug() << "Parsed " << filename;

    // If the import failed, report it
    if( !scene)
    {
        qDebug() << importer.GetErrorString();
        return false;
    }
    // Now we can access the file's contents.
    importChildren(scene, scene->mRootNode, this, QMatrix4x4());

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
    qDebug() << "Processing mesh " << m->mNumVertices;

    for (int i = 0; i < m->mNumVertices; i++) {
        qDebug() << "Add v " << i;
        aiVector3D v = m->mVertices[i];
        qDebug() << "v " << v.x << ", " << v.y << ", " << v.z;
        btVector3 vv = btVector3(v.x, v.y, v.z);
        qDebug() << "vv " << vv.x() << ", " << vv.y() << ", " << vv.z();
        m_bShape->addPoint(vv);
















    }
qDebug() << "recalc";
    m_bShape->recalcLocalAabb();
}

btConvexHullShape* BiMesh::getMesh(QString name)
{
    return m_bShape;
}


