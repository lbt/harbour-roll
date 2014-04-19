#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <QObject>
#include <QList>
#include <QHash>

// When the 5.2 Qt hits use this:
// #include <QOpenGLTexture>
#include <QImage>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/LogStream.hpp>

#include "bullet/btBulletDynamicsCommon.h"

#include "vao.h"
#include "renderable.h"
#include "shader.h"
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

class World;

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

    Renderable*       makeRenderable(QString name, VAO *v);
    Renderable*       makeRenderable(QString name, VAO *v, Texture *t);
    Shader*           makeShader(QString name, QUrl v_glsl_path,
                                 QUrl f_glsl_path);

    btCollisionShape* makeShape(QString name, QString modelType, btScalar r);
    btCollisionShape *makeShape(QString name, QString modelType, btScalar r, btScalar h);
    btCollisionShape *makeShape(QString name, QString modelType, btVector3 sides);
    btCollisionShape *makeShape(QString name, QString modelType, btVector3 normal, btScalar offset);
    btCollisionShape* makeShape(QString name, QString modelType, aiMesh *m);

    Texture*          makeTexture(QString name, QImage img);
    VAO*              makeVAO(QString name, aiMesh *m);


    void setupGL();

signals:

public slots:

private:
    Assimp::Importer* m_importer;

    World* m_world;
    QHash<QString, btCollisionShape*> m_shapes;
    QHash<QString, VAO*> m_vaos;
    QHash<QString, Shader*> m_shaders;
    QHash<QString, Renderable*> m_renderables;
    QHash<QString, Texture*> m_textures;
    QHash<QString, aiMesh*> m_meshes;

};
#endif // ASSETSTORE_H
