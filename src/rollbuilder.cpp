#include "rollbuilder.h"
#include "orbitmotion.h"
#include "cameraflyermotion.h"
#include "followmotion.h"
#include "curvemotion.h"
#include "lookatmotion.h"

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

    WorldItem* wi;
    FollowMotion *follower;

    qDebug() << "Setup track";
    wi = new WorldItem("track");
    wi->addRenderable(m_assetStore->getRenderable("gutter"));
    wi->addRenderable(m_assetStore->getRenderable("gutterBot"));
    // Create a Shape and a PhysicsMotion and add to wi
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "gutter", "btBvhTriangleMesh",
                                        m_assetStore->getMesh("gutter")),
                                    0.0, wi));
    Transform initialPos;
    initialPos.translate(QVector3D(0,0,-2.8));
    wi->setTransform(initialPos);
    //    wi->addToWorld(m_rollworld);

    qDebug() << "Setup track2";
    wi = new WorldItem("track2");
    wi->addRenderable(m_assetStore->getRenderable("track2Curve"));
    // Create a Shape and a PhysicsMotion and add to wi
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "track2Curve", "btBvhTriangleMesh",
                                        m_assetStore->getMesh("track2Curve")),
                                    0.0, wi));

    initialPos.setToIdentity();
    initialPos.translate(QVector3D(0,0,0));
    wi->setTransform(initialPos);
    //    wi->addToWorld(m_rollworld);

    qDebug() << "Setup track3";
    wi = new WorldItem("track3");
    wi->addRenderable(m_assetStore->getRenderable("track3Curve"));
    // Create a Shape and a PhysicsMotion and add to wi
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "track3Curve", "btBvhTriangleMesh",
                                        m_assetStore->getMesh("track3Curve")),
                                    0.0, wi));

    initialPos.setToIdentity();
    initialPos.translate(QVector3D(0,0,0));
    wi->setTransform(initialPos);
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup ball";
    wi = new WorldItem("ball");
    wi->addRenderable(m_assetStore->getRenderable("Sphere"));
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                                    0.1, wi));

    // #define START    btVector3(2.0,-0.0,0)

    initialPos.setToIdentity();
    initialPos.translate(m_rollworld->m_ballStartPos);
    wi->setTransform(initialPos);
    wi->addToWorld(m_rollworld);

    // Tell the rollworld that this is the ball
    m_rollworld->m_ball = wi;

    OrbitMotion* o;
    o = new OrbitMotion();
    o->setup(QVector3D(0,0,1), QVector3D(0,1,0), 1, 300 );

    follower = new FollowMotion();
    follower->follow(wi, 1);
    follower->setMotion(o);

    CurveMotion* curvy = new CurveMotion();
    curvy->setCurve(m_assetStore->getVAO("camera3curve"));
    curvy->setSpeed(10);
    LookAtMotion* looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));
    curvy->setMotion(looker);

    wi = new WorldItem("ball2");
    wi->addRenderable(m_assetStore->getRenderable("Sphere"));
    wi->setMotion(curvy);
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor");
    // This is oriented to point up (z=1) and set at offset (z=) -10
    m_rollworld->m_floor = wi;
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor", "btStaticPlane",
                                        btVector3( 0, 0, 1 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor2");
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor2", "btStaticPlane",
                                        btVector3( 0, 0,-1 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor3");
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor3", "btStaticPlane",
                                        btVector3( 0, 1, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor4");
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor4", "btStaticPlane",
                                        btVector3( 0,-1, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor5");
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor5", "btStaticPlane",
                                        btVector3( 1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);
    wi = new WorldItem("floor6");
    wi->setMotion(new PhysicsMotion(m_assetStore->makeShape(
                                        "Floor6", "btStaticPlane",
                                        btVector3(-1, 0, 0 ), -10), 0.0, wi));
    wi->addToWorld(m_rollworld);

    qDebug() << "Setup lights";
    // Ensure that the first throw has a visible dlight
    DirectionalLight *dl = new DirectionalLight("main");
    dl->setBaseLight(QVector3D(1.0, 1.0, 1.0), 0.4, 0.8);
    dl->setDirectionalLight(QVector3D(-1, 1, 4).normalized());
    dl->setMotion(new BaseMotion());
    dl->addToWorld(m_rollworld);

    dl = new DirectionalLight("d2");
    dl->setMotion(new BaseMotion());
    dl->randomise();
    dl->addToWorld(m_rollworld);

    for (int i: {1, 2, 3}) {
        QString name = QString("p%1").arg(i);
        PointLight *pl = new PointLight(name);
        pl->setMotion(new OrbitMotion());
        pl->randomise();
        qDebug() <<"Adding Light: " << pl->metaObject()->className();
        pl->addToWorld(m_rollworld);
    }
    follower = new FollowMotion();
    follower->follow(m_rollworld->m_ball, 1); // follow the ball from inside
    o = new OrbitMotion();
    o->setup(QVector3D(0,0,1), QVector3D(0,1,0), 1.5, 300 );
    follower->setMotion(o);
    delete(m_world->getLight("p1")->setMotion(follower));

    qDebug() << "Setup cameras";
    CameraManager::Display display(540, 960, 50);
    CameraManager* flyCam = new CameraManager("flycam", display);
    flyCam->setMotion(new CameraFlyerMotion());
    flyCam->motion()->lookAt(QVector3D(0,-0.1,32), QVector3D(), QVector3D(0, 0, 1)); // top
    flyCam->addToWorld(m_rollworld);

    CameraManager* followCam = new CameraManager("followcam", display);
    follower = new FollowMotion();
    follower->follow(m_rollworld->m_ball, 8); // follow the ball from 8 away
    followCam->setMotion(follower);
    followCam->addToWorld(m_rollworld);

    CameraManager* curveCam = new CameraManager("curvecam", display);
    looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));
//    looker->setLookAt(m_rollworld->m_ball);
    curvy = new CurveMotion();
    curvy->setCurve(m_assetStore->getVAO("camera3curve"));
    curvy->setSpeed(10);
    curvy->setMotion(looker);

    curveCam->setMotion(curvy);
    curveCam->addToWorld(m_rollworld);

    m_rollworld->setActiveCamera(curveCam);

    m_assetStore->load_finished();

}
