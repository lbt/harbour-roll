#include "worldbuilder.h"
#include "world.h"

#include "sailfishapp.h"

WorldBuilder::WorldBuilder(World *w, QObject *parent) :
    QObject(parent)
  , m_assetStore(new AssetStore(w))
  , m_world(w)
{
}

void WorldBuilder::setup(){

    // for all in json

    //    WorldItem* wi = new WorldItem();

    m_assetStore->makeShader("default",
                             SailfishApp::pathTo("roll_vert.glsl.out").toLocalFile(),
                             SailfishApp::pathTo("roll_frag.glsl.out").toLocalFile());
}
