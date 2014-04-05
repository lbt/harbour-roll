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

#include "glprogram.h"

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

    bool load(QString filename);
    void importChildren(const aiScene *scene, aiNode *node);

    bool contains(QString name) { return m_assets.contains(name); }
    aiMesh* getMesh(QString name);
    QList<QString> getNames();

signals:

public slots:

private:
    QMap<QString, Asset*> m_assets;

};
#endif // ASSETSTORE_H
