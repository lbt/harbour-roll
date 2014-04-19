#include "rollbuilder.h"
#include "lightorbiter.h"
#include "cameraflyer.h"
#include "camerafollower.h"

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
    wi = new WorldItem("track");
    wi->add(m_assetStore->getRenderable("gutter"));
    wi->add(m_assetStore->getRenderable("gutterBot"));
    // Create a Shape and a Physics and add to wi
    wi->add(new Physics(m_assetStore->makeShape("gutter", "btBvhTriangleMesh",
                                                m_assetStore->getMesh("gutter")),
                        0.0, wi));
    Transform trackPos;
    trackPos.translate(QVector3D(0,0,-2.8));
    wi->setTransform(trackPos);
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup track2";
    wi = new WorldItem("track2");
    wi->add(m_assetStore->getRenderable("track2Curve"));
    // Create a Shape and a Physics and add to wi
    wi->add(new Physics(m_assetStore->makeShape("track2Curve", "btBvhTriangleMesh",
                                                m_assetStore->getMesh("track2Curve")),
                        0.0, wi));

    trackPos.setToIdentity();
    trackPos.translate(QVector3D(0,0,0));
    wi->setTransform(trackPos);
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup ball";
    wi = new WorldItem("ball");
    wi->add(m_assetStore->getRenderable("Sphere"));
    wi->add(new Physics(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                        0.1, wi));

    // #define START    btVector3(2.0,-0.0,0)

    trackPos.setToIdentity();
    trackPos.translate(m_rollworld->m_ballStartPos);
    wi->setTransform(trackPos);
    wi->addToWorld(m_rollworld);

    // Tell the rollworld that this is the ball
    m_rollworld->m_ball = wi;

    wi = new WorldItem("floor");
    // This is oriented to point up (z=1) and set at offset (z=) -10
    m_rollworld->m_floor = wi;
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor", "btStaticPlane",
                            btVector3( 0, 0, 1 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor2");
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor2", "btStaticPlane",
                            btVector3( 0, 0,-1 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor3");
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor3", "btStaticPlane",
                            btVector3( 0, 1, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor4");
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor4", "btStaticPlane",
                            btVector3( 0,-1, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor5");
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor5", "btStaticPlane",
                            btVector3( 1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor6");
    wi->add(new Physics(m_assetStore->makeShape(
                            "Floor6", "btStaticPlane",
                            btVector3(-1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup lights";
    // Ensure that the first throw has a visible dlight
    DirectionalLight *dl = new DirectionalLight("main");
    dl->setBaseLight(QVector3D(1.0, 1.0, 1.0), 0.4, 0.8);
    dl->setDirectionalLight(QVector3D(-1, 1, 4).normalized());
    dl->setLightManager(new LightOrbiter());
    dl->addToWorld(m_rollworld);

    dl = new DirectionalLight("d2");
    dl->setLightManager(new LightOrbiter());
    dl->randomise();
    dl->addToWorld(m_rollworld);

    for (int i: {1, 2, 3}) {
        QString name = QString("p%1").arg(i);
        PointLight *pl = new PointLight(name);
        LightOrbiter* lm = new LightOrbiter();
        lm->setScale(QVector3D(4.0, 5.0, 4.0));
        lm->active(true);
        pl->setLightManager(lm);
        pl->randomise();
        qDebug() <<"Adding Light: " << pl->metaObject()->className();
        pl->addToWorld(m_rollworld);
    }

    qDebug() << "Setup cameras";
    CameraManager::Display display(540, 960, 50);
    CameraFlyer* flyCam = new CameraFlyer("flycam", display);
    CameraFollower* followCam = new CameraFollower("followcam", display);
    flyCam->lookAt(QVector3D(0,-0.1,32), QVector3D(), QVector3D(0, 0, 1)); // top
    followCam->follow(m_rollworld->m_ball, 8); // follow the ball from 8 away

    flyCam->addToWorld(m_rollworld);
    followCam->addToWorld(m_rollworld);

    m_rollworld->setActiveCamera(followCam);

    m_assetStore->load_finished();

}
