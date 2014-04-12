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

    virtual AssetStore* getAssetStore() const {return m_assetStore; }
    virtual World* getWorld() const {return m_world; }

    virtual void setup() = 0;
    virtual void setupGL() { m_assetStore->setupGL(); }

signals:

public slots:

protected:
    AssetStore *m_assetStore;
    World *m_world;

};

#endif // WORLDBUILDER_H
