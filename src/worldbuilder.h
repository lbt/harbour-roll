#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

#include <QObject>
#include <QFile>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assetstore.h"
#include "world.h"
#include "worlditem.h"


class WorldBuilder : public QObject
{
    Q_OBJECT
    class dbgStream : public Assimp::LogStream // Used by load
    {
    public:
        dbgStream() {}
        void write(const char* message) { qDebug() << message; }
    };

public:
    explicit WorldBuilder(QObject *parent = 0);

    bool load(QString filename);

    AssetStore* getAssetStore() const {return m_assetStore; }
    World* getWorld() const {return m_world; }

    void importChildren(const aiScene *scene, aiNode *node);
    void setup();
signals:

public slots:

private:
    AssetStore *m_assetStore;
    World *m_world;

};

#endif // WORLDBUILDER_H
