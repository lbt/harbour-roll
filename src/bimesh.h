#ifndef BIMESH_H
#define BIMESH_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QMap>
#include <QMatrix4x4>

//#include <btBulletDynamicsCommon.h>
//#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class BiMesh : public QObject
{
    Q_OBJECT
private:
    static QMap<QString, btCollisionShape*> c_bShape;

public:
    explicit BiMesh(QObject *parent = 0);

    bool load(QString filename);
    void importChildren(const aiScene *scene, aiNode *node, BiMesh *targetParent, QMatrix4x4 accTransform);
    void copyMeshes(const aiScene *scene, aiNode *node);
    btCollisionShape* getMesh(QString name);

signals:

public slots:

protected:
    void copyMesh(aiNode* node);

private:
    // glMesh m_glMesh;
};

#endif // BIMESH_H
