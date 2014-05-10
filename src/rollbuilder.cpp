#include "rollbuilder.h"

#include "rollball.h"

#include "orbitmotion.h"
#include "cameraflyermotion.h"
#include "followmotion.h"
#include "curvemotion.h"
#include "lookatmotion.h"
#include "delayedmotion.h"
#include "sailfishapp.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    m_assetStore->makeShader(
                "track",
                SailfishApp::pathTo("track_vert.glsl.out"),
                SailfishApp::pathTo("track_frag.glsl.out"));

    m_assetStore->getRenderable("Sphere")->setShader(defaultShader);

    WorldItem* wi;
    FollowMotion *follower;

    qDebug() << "Setup ball";
    PhysicsMotion* pm = new PhysicsMotion(m_assetStore->makeShape("Sphere", "btSphere", 0.4),
                                          0.1);
    RollBall* ball = new RollBall("ball", pm);
    ball->setCollisionType(WorldItem::WORLD_COLLISONS);
    ball->addRenderable(m_assetStore->getRenderable("Sphere"));

    ball->reset();
    ball->addToWorld(m_rollworld);

    // Tell the rollworld that this is the ball
    m_rollworld->m_ball = ball;

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
    OrbitMotion* o;

    //    follower = new FollowMotion();
    follower = new DelayedMotion(10);
    follower->follow(m_rollworld->m_ball, QVector3D(0,0,1)); // follow the ball from just above
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
    follower->follow(m_rollworld->m_ball, QVector3D(0.5,0.5,10)); // follow the ball from 10 away
    CameraManager* followCam = new CameraManager("followcam", follower, display);
    followCam->addToWorld(m_rollworld);

    LookAtMotion* looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));

    CameraManager* curveCam = new CameraManager("curvecam", new BaseMotion(), display);
    curveCam->addToWorld(m_rollworld);

    m_rollworld->setActiveCamera(followCam);

    //    m_assetStore->load_finished();

}

bool RollBuilder::loadTracks(QString jsonFile){
    QFile loadFile(jsonFile);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open tracks file.");
        return false;
    }
    qDebug() << "Found tracks.json";

    QJsonDocument loadDoc(QJsonDocument::fromJson(loadFile.readAll()));

    if (loadDoc.isArray())
        qDebug() << "tracks.json is an array";
    else if (loadDoc.isObject())
        qDebug() << "tracks.json is an object";
    else if (loadDoc.isNull())
        qDebug() << "tracks.json is null";


    QJsonObject roll = loadDoc.object();
    if (! roll.contains("tracks")) {
        return false;
    }
    qDebug() << "Found tracks";

    m_tracks.clear();
    QJsonArray tracksArray = roll["tracks"].toArray();
    foreach (QJsonValue trackJ, tracksArray) {
        Track* t = new Track();
        t->read(trackJ.toObject());
        m_tracks[t->name()] = t ;
        qDebug() << "Found track " << t->name();
    }
    return true;
}

void RollBuilder::setTrack(QString trackname)
{
    Track* t = m_tracks[trackname];
    if (!t) {
        qDebug() << "No track " << trackname;
        return;
    }
    qDebug() << "Setup track " << trackname;

    WorldItem* wi = m_rollworld->getItem(m_currentTrack);
    if (wi) {
        wi->removeFromWorld();
        delete wi;
    }

    // Create a Shape and a PhysicsMotion and add to a new wi
    m_assetStore->getRenderable(t->mesh())->setShader(m_assetStore->getShader("track"));
    m_assetStore->getRenderable(t->mesh())->setTranslucent(true);

    PhysicsMotion* pm =new PhysicsMotion(m_assetStore->makeShape(
                                             t->mesh(), "btBvhTriangleMesh",
                                             m_assetStore->getMesh(t->mesh())),
                                         0.0);
    wi = new WorldItem(t->name(), pm);
    wi->addRenderable(m_assetStore->getRenderable(t->mesh()));
    wi->addToWorld(m_rollworld);

    m_rollworld->getBall()->setStart(t->start());

    CurveMotion* curvy = new CurveMotion();
    curvy->setCurve(m_assetStore->getVAO(t->cameraCurve()));
    curvy->setSpeed(10);
    LookAtMotion* looker = new LookAtMotion();
    looker->setLookAt(QVector3D(0,0,0));
    curvy->setMotion(looker);

    m_world->getCamera("curvecam")->setMotion(curvy);

    m_currentTrack = t->name();
}

QStringList RollBuilder::getTrackNames()
{
    QStringList l(m_tracks.keys());
    return l;
}
