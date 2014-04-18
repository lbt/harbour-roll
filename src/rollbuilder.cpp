#include "rollbuilder.h"
#include "lightorbiter.h"

#include "sailfishapp.h"

RollBuilder::RollBuilder(RollWorld *parent) :
    WorldBuilder(parent)
  , m_rollworld(parent)
{
}

void RollBuilder::setup(){

    WorldBuilder::setup();

    qDebug() << "Setup";

    m_assetStore->load(SailfishApp::pathTo("curve1.obj").toLocalFile());

    // Simulate reading a json file:

    qDebug() << "Setup shader";
    Shader* defaultShader = m_assetStore->makeShader(
                "default",
                SailfishApp::pathTo("roll_vert.glsl.out"),
                SailfishApp::pathTo("roll_frag.glsl.out"));

    Shader* trackShader = m_assetStore->makeShader(
                "track",
                SailfishApp::pathTo("track_vert.glsl.out"),
                SailfishApp::pathTo("track_frag.glsl.out"));

    m_assetStore->getRenderable("gutter")->setShader(defaultShader);
    m_assetStore->getRenderable("gutterBot")->setShader(defaultShader);
    m_assetStore->getRenderable("Sphere")->setShader(defaultShader);

    m_assetStore->getRenderable("track2Curve")->setShader(trackShader);

    WorldItem* wi;

    qDebug() << "Setup track";
    wi = new WorldItem("track", m_world);
    wi->add(m_assetStore->getRenderable("gutter"));
    wi->add(m_assetStore->getRenderable("gutterBot"));
    // Create a Shape and a Physics and add to wi
    wi->add(new Physics(m_assetStore->makeShape("gutter", "btBvhTriangleMesh",
                                                m_assetStore->getMesh("gutter")),
                        0.0, wi));
    Transform trackPos;
    trackPos.translate(QVector3D(0,0,-2.8));
    wi->setTransform(trackPos);
    wi->addToWorld();

    qDebug() << "Setup track2";
    wi = new WorldItem("track2", m_world);
    wi->add(m_assetStore->getRenderable("track2Curve"));
    // Create a Shape and a Physics and add to wi
    wi->add(new Physics(m_assetStore->makeShape("track2Curve", "btBvhTriangleMesh",
                                                m_assetStore->getMesh("track2Curve")),
                        0.0, wi));

    trackPos.setToIdentity();
    trackPos.translate(QVector3D(0,0,0));
    wi->setTransform(trackPos);
    wi->addToWorld();

    qDebug() << "Setup ball";
    wi = new WorldItem("ball", m_world);
    wi->add(m_assetStore->getRenderable("Sphere"));
    wi->add(new Physics(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                        0.1, wi));

    // #define START    btVector3(2.0,-0.0,0)

    trackPos.setToIdentity();
    trackPos.translate(m_rollworld->m_ballStartPos);
    wi->setTransform(trackPos);
    wi->addToWorld();

    // Tell the rollworld that this is the ball
    m_rollworld->m_ball = wi;

    wi = new WorldItem("floor", m_world);
    // This is oriented to point up (z=1) and set at offset (z=) -10
    m_rollworld->m_floor = wi;
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor", "btStaticPlane",
                            btVector3( 0, 0, 1 ), -10), 0.0, wi));
    wi->addToWorld();
    wi = new WorldItem("floor2", m_world);
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor2", "btStaticPlane",
                            btVector3( 0, 0,-1 ), -10), 0.0, wi));
    wi->addToWorld();
    wi = new WorldItem("floor3", m_world);
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor3", "btStaticPlane",
                            btVector3( 0, 1, 0 ), -10), 0.0, wi));
    wi->addToWorld();
    wi = new WorldItem("floor4", m_world);
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor4", "btStaticPlane",
                            btVector3( 0,-1, 0 ), -10), 0.0, wi));
    wi->addToWorld();
    wi = new WorldItem("floor5", m_world);
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor5", "btStaticPlane",
                            btVector3( 1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld();
    wi = new WorldItem("floor6", m_world);
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor6", "btStaticPlane",
                            btVector3(-1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld();

    qDebug() << "Setup lights";
    _DirectionalLight dlight;
    // Ensure that the first throw has a visible dlight
    dlight.Base.Color = QVector3D(1.0, 1.0, 1.0);
    dlight.Base.AmbientIntensity=0.4;
    dlight.Base.DiffuseIntensity=0.8;
    dlight.Direction = QVector3D(-1, 1, 4).normalized();

    DirectionalLight *dl = new DirectionalLight("main", m_world);
    dl->set(dlight);
    dl->setLightManager(new LightOrbiter());
    m_rollworld->add("main", dl);

    dl = new DirectionalLight("d2", m_world);
    dl->setLightManager(new LightOrbiter());
    dl->randomise();
    m_rollworld->add("d2", dl);

    for (int i: {1, 2, 3}) {
        QString name = QString("p%1").arg(i);
        PointLight *pl = new PointLight(name, m_world);
        LightOrbiter* lm = new LightOrbiter();
        lm->setScale(QVector3D(4.0, 5.0, 4.0));
        lm->active(true);
        pl->setLightManager(lm);
        pl->randomise();
        qDebug() <<"Adding Light: " << pl->metaObject()->className();
        m_rollworld->add(name, pl);
    }

    m_assetStore->load_finished();

}
