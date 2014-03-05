#ifndef BIMESH_H
#define BIMESH_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QMap>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

// When the 5.2 Qt hits use this:
// #include <QOpenGLTexture>
#include <QSGTexture>

//#include <btBulletDynamicsCommon.h>
//#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glprogram.h"
#include "vaocontainer.h"

class BiMesh : public QObject
{
    Q_OBJECT

public:
    explicit BiMesh(const aiScene *scene, aiNode *node, QObject *parent = 0);

    void render(GLProgram *p);
    void setup(GLProgram *p);
private:

    QString m_name;
    VAOContainer *m_vao;
    QSGTexture *m_texture;
    GLuint m_vboIds[3];
    GLuint m_texIds[1];
    void * m_texCoords;
    void * m_meshVertices;
};

class BiMeshContainer : public QObject
{
    Q_OBJECT
private:
//    QMap<QString, btCollisionShape*> m_biShapes;
    QMap<QString, BiMesh*> m_biShapes;
    BiMesh* m_top;

public:
    explicit BiMeshContainer(QObject *parent = 0);

    bool load(QString filename);
    BiMesh* importChildren(const aiScene *scene, aiNode *node, BiMesh *targetParent, QMatrix4x4 accTransform);
    BiMesh* nodeToMesh(const aiScene *scene, aiNode *node);

    btCollisionShape *getCollisionMesh(QString name);
    BiMesh *getBiMesh(QString name);
};

class BibtGImpactMeshShape : public btGImpactMeshShape, public BiMesh
{
public:
    BibtGImpactMeshShape(btStridingMeshInterface *meshInterface, const aiScene *scene, aiNode *node, QObject *parent = 0) :
        btGImpactMeshShape(meshInterface)
      , BiMesh(scene, node, parent) {}
//    void upd() { this->calcLocalAABB(); }
};

class BibtConvexHullShape : public btConvexHullShape, public BiMesh
{
    BiMesh* m_owner;
public:
    BibtConvexHullShape(const aiScene *scene, aiNode *node, QObject *parent =0) :
        btConvexHullShape()
      , BiMesh(scene, node, parent) {}
};

#endif // BIMESH_H
