#include "rollbuilder.h"

#include "rollball.h"

#include "orbitmotion.h"
#include "cameraflyermotion.h"
#include "followmotion.h"
#include "curvemotion.h"
#include "lookatmotion.h"
#include "delayedmotion.h"
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
    m_assetStore->getRenderable("track3Curve")->setShader(trackShader);
    m_assetStore->getRenderable("track3Curve")->setTranslucent(true);

    WorldItem* wi;
    FollowMotion *follower;

    qDebug() << "Setup track";
    // Create a Shape and a PhysicsMotion and add to a new wi
    PhysicsMotion* pm = new PhysicsMotion(
                m_assetStore->makeShape(
                    "gutter", "btBvhTriangleMesh",
                    m_assetStore->getMesh("gutter")),
                0.0);
    wi = new WorldItem("track", pm);
    wi->addRenderable(m_assetStore->getRenderable("gutter"));
    wi->addRenderable(m_assetStore->getRenderable("gutterBot"));

    Transform initialPos;
    initialPos.translate(QVector3D(0,0,-2.8));
    wi->setTransform(initialPos);
    //    wi->addToWorld(m_rollworld);

    qDebug() << "Setup track2";
    // Create a Shape and a PhysicsMotion and add to wi
    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "track2Curve", "btBvhTriangleMesh",
                                        m_assetStore->getMesh("track2Curve")),
                                    0.0);
    wi = new WorldItem("track2", pm);
    wi->addRenderable(m_assetStore->getRenderable("track2Curve"));

    initialPos.setToIdentity();
    initialPos.translate(QVector3D(0,0,0));
    wi->setTransform(initialPos);
    //    wi->addToWorld(m_rollworld);

    qDebug() << "Setup track3";
    // Create a Shape and a PhysicsMotion and add to wi
    pm =new PhysicsMotion(m_assetStore->makeShape(
                                        "track3Curve", "btBvhTriangleMesh",
                                        m_assetStore->getMesh("track3Curve")),
                                    0.0);
    wi = new WorldItem("track3", pm);
    wi->addRenderable(m_assetStore->getRenderable("track3Curve"));

    initialPos.setToIdentity();
    initialPos.translate(QVector3D(0,0,0));
    wi->setTransform(initialPos);
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup ball";
    pm = new PhysicsMotion(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                                    0.1);
    RollBall* ball = new RollBall("ball", pm);
    ball->setCollisionType(WorldItem::WORLD_COLLISONS);
    ball->addRenderable(m_assetStore->getRenderable("Sphere"));

    // #define START    btVector3(2.0,-0.0,0)
    ball->setStart(QVector3D(3.5, -2.5, 1));
    ball->reset();
    ball->addToWorld(m_rollworld);

    // Tell the rollworld that this is the ball
    m_rollworld->m_ball = ball;

    OrbitMotion* o;
    o = new OrbitMotion();
    o->setup(QVector3D(0,0,1), QVector3D(0,1,0), 1, 300 );

    follower = new FollowMotion();
    follower->follow(wi, QVector3D(0,0,1));
    follower->setMotion(o);

    CurveMotion* curvy = new CurveMotion();
    curvy->setCurve(m_assetStore->getVAO("camera3curve"));
    curvy->setSpeed(10);
    LookAtMotion* looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));
    curvy->setMotion(looker);

    wi = new WorldItem("ball2", curvy);
    wi->addRenderable(m_assetStore->getRenderable("Sphere"));
    wi->addToWorld(m_rollworld);

    // This is oriented to point up (z=1) and set at offset (z=) -10
    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor", "btStaticPlane",
                                        btVector3( 0, 0, 1 ), -10), 0.0);
    wi = new WorldItem("floor", pm);
    wi->addToWorld(m_rollworld);
    m_rollworld->m_floor = wi;
    wi->setCollisionType(WorldItem::WORLD_COLLISONS);

    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor2", "btStaticPlane",
                                        btVector3( 0, 0,-1 ), -10), 0.0);
    wi = new WorldItem("floor2", pm);
    wi->addToWorld(m_rollworld);

    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor3", "btStaticPlane",
                                        btVector3( 0, 1, 0 ), -10), 0.0);
    wi = new WorldItem("floor3", pm);
    wi->addToWorld(m_rollworld);

    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor4", "btStaticPlane",
                                        btVector3( 0,-1, 0 ), -10), 0.0);
    wi = new WorldItem("floor4", pm);
    wi->addToWorld(m_rollworld);

    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor5", "btStaticPlane",
                                        btVector3( 1, 0, 0 ), -10), 0.0);
    wi = new WorldItem("floor5", pm);
    wi->addToWorld(m_rollworld);

    pm = new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor6", "btStaticPlane",
                                        btVector3(-1, 0, 0 ), -10), 0.0);
    wi = new WorldItem("floor6", pm);
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup lights";
    // Ensure that the first throw has a visible dlight
    DirectionalLight *dl = new DirectionalLight("main", new BaseMotion());
    dl->setBaseLight(QVector3D(1.0, 1.0, 1.0), 0.4, 0.8);
    dl->setDirectionalLight(QVector3D(-1, 1, 4).normalized());

    dl->addToWorld(m_rollworld);

    dl = new DirectionalLight("d2", new BaseMotion());
    dl->randomise();
    dl->addToWorld(m_rollworld);

    for (int i: {1, 2, 3}) {
        QString name = QString("p%1").arg(i);
        PointLight *pl = new PointLight(name, new OrbitMotion());
        pl->randomise();
        qDebug() <<"Adding Light: " << pl->metaObject()->className();
        pl->addToWorld(m_rollworld);
    }
//    follower = new FollowMotion();
    follower = new DelayedMotion(10);
    follower->follow(m_rollworld->m_ball, QVector3D(0,0,1)); // follow the ball from inside
    o = new OrbitMotion();
    o->setup(QVector3D(0,0,1), QVector3D(0,1,0), 1.5, 300 );
    follower->setMotion(o);
    m_world->getLight("p1")->setMotion(follower);

    qDebug() << "Setup cameras";
    CameraManager::Display display(540, 960, 50);
    CameraManager* flyCam = new CameraManager("flycam", new CameraFlyerMotion(), display);
    flyCam->motion()->lookAt(QVector3D(0,-0.1,32), QVector3D(), QVector3D(0, 0, 1)); // top
    flyCam->addToWorld(m_rollworld);

    follower = new FollowMotion();
    follower->follow(m_rollworld->m_ball, QVector3D(0.5,0.5,10)); // follow the ball from 8 away
    CameraManager* followCam = new CameraManager("followcam", follower, display);
    followCam->addToWorld(m_rollworld);

    looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));
    curvy = new CurveMotion();
    curvy->setCurve(m_assetStore->getVAO("camera3curve"));
    curvy->setSpeed(10);
    curvy->setMotion(looker);

    CameraManager* curveCam = new CameraManager("curvecam", curvy, display);
    curveCam->addToWorld(m_rollworld);

    m_rollworld->setActiveCamera(followCam);

    m_assetStore->load_finished();

}
