#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QMap>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

// When the 5.2 Qt hits use this:
// #include <QOpenGLTexture>
#include <QSGTexture>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "bullet/BulletCollision/Gimpact/btGImpactShape.h"

#include "vao.h"
#include "renderable.h"

#include "glprogram.h"

/////////////////////////////////////////////////////////////////////
/// \brief The AssetStore class
///
/// The AssetStore holds reusable assets:
/// * VAO
/// * Texture
/// * Shaders GLPrograms
/// * Renderables
/// * bullet collisionshapes
/// * curves
///


class AssetStore : public QObject
{
    class Asset
    {
    public:
        explicit Asset(Asset* parent, QMatrix4x4 transform);
    private:
        Asset* m_parent;
        QMatrix4x4 m_transform;
    };

    Q_OBJECT
public:
    explicit AssetStore(QObject *parent = 0);
    ~AssetStore();

    bool load(QString filename);
    void importChildren(const aiScene *scene, aiNode *node);

//    bool contains(QString name) { return m_assets.contains(name); }
    aiMesh* getMesh(QString name);
    QList<QString> getNames();

    btCollisionShape* addShape(QString name, QString modelType, aiMesh *m);
    btCollisionShape* addShape(QString name, QString modelType, btScalar r);
    QSGTexture* addTexture(QString name, QSGTexture* t);
    VAO* addVAO(QString name, aiMesh *m);
    Renderable* addRenderable(QString name, VAO *v, QSGTexture *t);

signals:

public slots:

private:
    QMap<QString, btCollisionShape*> m_shapes;
    QMap<QString, VAO*> m_vaos;
    QMap<QString, GLProgram*> m_shaders;
    QMap<QString, Renderable*> m_renderables;
    QMap<QString, QSGTexture*> m_textures;

};
#endif // ASSETSTORE_H
