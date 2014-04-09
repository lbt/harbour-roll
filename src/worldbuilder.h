#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

#include <QObject>
#include <QFile>

#include "assetstore.h"
#include "shader.h"
#include "world.h"
#include "worlditem.h"


class WorldBuilder : public QObject
{
    Q_OBJECT

public:
    explicit WorldBuilder(World *w, QObject *parent = 0);

    AssetStore* getAssetStore() const {return m_assetStore; }
    World* getWorld() const {return m_world; }

    void setup();
    void setupGL() { m_assetStore->setupGL(); }

signals:

public slots:

private:
    AssetStore *m_assetStore;
    World *m_world;

};

#endif // WORLDBUILDER_H
