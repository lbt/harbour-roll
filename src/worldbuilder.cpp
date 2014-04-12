#include "worldbuilder.h"
#include "world.h"

WorldBuilder::WorldBuilder(World *w, QObject *parent) :
    QObject(parent)
  , m_assetStore(new AssetStore(w))
  , m_world(w)
{
}
