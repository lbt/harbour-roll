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
#include <QImage>

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
#include "texture.h"

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
    Q_OBJECT

    class dbgStream : public Assimp::LogStream // Used by load
    {
    public:
        dbgStream() {}
        void write(const char* message) { qDebug() << message; }
    };

public:
    explicit AssetStore(World* w, QObject *parent = 0);
    ~AssetStore();

    bool load(QString filename);
    void load_finished();
    void importChildren(const aiScene *scene, aiNode *node);

//    bool contains(QString name) { return m_assets.contains(name); }

    btCollisionShape* getShape(QString name) { return m_shapes[name]; }
    Shader*           getShader(QString name) { return m_shaders[name]; }
    Renderable*       getRenderable(QString name)  { return m_renderables[name]; }
    aiMesh*           getMesh(QString name) { return m_meshes[name]; }
    VAO*              getVAO(QString name) { return m_vaos[name]; }

    Renderable*       makeRenderable(QString name, VAO *v, Texture *t);
    Shader*           makeShader(QString name, QString v_glsl_path,
                                 QString s_glsl_path);
    btCollisionShape* makeShape(QString name, QString modelType, btScalar r);
    btCollisionShape *makeShape(QString name, QString modelType, btScalar r, btScalar h);
    btCollisionShape *makeShape(QString name, QString modelType, btVector3 sides);
    btCollisionShape* makeShape(QString name, QString modelType, aiMesh *m);
    Texture*          makeTexture(QString name, QImage img);
    VAO*              makeVAO(QString name, aiMesh *m);


    void setupGL();


signals:

public slots:

private:
    Assimp::Importer* m_importer;

    World* m_world;
    QMap<QString, btCollisionShape*> m_shapes;
    QMap<QString, VAO*> m_vaos;
    QMap<QString, Shader*> m_shaders;
    QMap<QString, Renderable*> m_renderables;
    QMap<QString, Texture*> m_textures;
    QMap<QString, aiMesh*> m_meshes;

};
#endif // ASSETSTORE_H
