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

    qDebug() << "Setup";

    m_assetStore->load(SailfishApp::pathTo("curve1.obj").toLocalFile());

    // Simulate reading a json file:

    qDebug() << "Setup shader";
    Shader* defaultShader = m_assetStore->makeShader(
                "default",
                SailfishApp::pathTo("roll_vert.glsl.out"),
                SailfishApp::pathTo("roll_frag.glsl.out"));

    m_assetStore->getRenderable("gutter")->setShader(defaultShader);
    m_assetStore->getRenderable("gutterBot")->setShader(defaultShader);
    m_assetStore->getRenderable("Sphere")->setShader(defaultShader);

    WorldItem* wi;

    qDebug() << "Setup track";
    wi = new WorldItem("track", m_world);
    wi->add(m_assetStore->getRenderable("gutter"));
    wi->add(m_assetStore->getRenderable("gutterBot"));
    // Create a Shape and a Physics and add to wi
    wi->add(new Physics(m_assetStore->makeShape("gutter", "btBvhTriangleMesh",
                                                m_assetStore->getMesh("gutter")),
                        0.0, wi));
    wi->addToWorld();

    qDebug() << "Setup ball";
    wi = new WorldItem("ball", m_world);
    wi->add(m_assetStore->getRenderable("Sphere"));
    wi->add(new Physics(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                        0.1, wi));
    wi->addToWorld();

    qDebug() << "Setup lights";
    _DirectionalLight dlight;
    // Ensure that the first throw has a visible dlight
    dlight.Base.Color = QVector3D(1.0, 1.0, 1.0);
    dlight.Base.AmbientIntensity=0.4;
    dlight.Base.DiffuseIntensity=0.8;
    dlight.Direction = QVector3D(-1, 1, 4).normalized();

    DirectionalLight *dl = new DirectionalLight(m_world);
    dl->set(dlight);
    m_world->add("main", dl);

    dl = new DirectionalLight(m_world);
    dl->randomise();
    m_world->add("d2", dl);

    for (int i: {1, 2, 3}) {
        PointLight *pl = new PointLight(m_world);
        pl->randomise();
        pl->lightManager.setScale(QVector3D(4.0, 5.0, 4.0));
        qDebug() <<"Adding Light: " << pl->metaObject()->className();
        m_world->add(QString("p%1").arg(i), pl);
    }

    m_assetStore->load_finished();
}
