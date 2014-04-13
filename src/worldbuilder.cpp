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

    qDebug() << "Setup debug shader";
    m_world->m_debugShader = m_assetStore->makeShader(
                "debug",
                SailfishApp::pathTo("debug_vert.glsl"),
                SailfishApp::pathTo("debug_frag.glsl"));

}
