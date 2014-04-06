#include "assetstore.h"
#include "utils.h"

AssetStore::AssetStore(QObject *parent) :
    QObject(parent)
{
}
AssetStore::~AssetStore() {
    for (auto key: m_shapes.keys()){
        btCollisionShape* shape = m_shapes[key];
        m_shapes[key] = 0;
        delete shape;
    }
    for (auto key: m_vaos.keys()){
        VAO* v = m_vaos[key];
        m_vaos[key] = 0;
        delete v;
    }
    for (auto key: m_renderables.keys()){
        Renderable* r = m_renderables[key];
        m_renderables[key] = 0;
        delete r;
    }
}

btCollisionShape* AssetStore::addShape(QString name, QString modelType, btScalar r)
{
    btSphereShape* shape = NULL;
    if (m_shapes.contains(name)) {
        qDebug() <<"Existing shape " << name;
    }
    if (modelType == "btSphere") {
        qDebug() <<"btSphereShape";
        shape = new btSphereShape(r);
        if (shape) m_shapes[name] = shape;
    } else {
        qDebug() <<"Unknown shape " << modelType;
    }
    return shape;
}

QSGTexture *AssetStore::addTexture(QString name, QSGTexture *t)
{
    if (m_textures.contains(name)) {
        qDebug() <<"Existing texture " << name;
    }
    if (t) m_textures[name] = t;
    return t;
}

btCollisionShape* AssetStore::addShape(QString name, QString modelType, aiMesh* m) {
    btCollisionShape* rshape= NULL;

    if (! m) {
        qDebug() << "Mesh type "<< modelType << " requested and no mesh passed";
        return rshape;
    }

    qDebug() << "Processing mesh of " << m->mNumVertices << " vertices and " << m->mNumFaces << " faces as type " << modelType;
    btTriangleMesh* btMesh = new btTriangleMesh(true,false);

    aiFace* f = m->mFaces;
    for (unsigned int nf = 0 ; nf < m->mNumFaces; nf++, f++) {
        unsigned int* ind = f->mIndices;
        if (f->mNumIndices != 3){
            qDebug() << "This is a curve";
        }
        // Note that y/z are transposed. I think this matches a blender view
        btVector3 v1 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        ind++;
        btVector3 v2 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        ind++;
        btVector3 v3 = btVector3((m->mVertices+*ind)->x,(m->mVertices+*ind)->y,(m->mVertices+*ind)->z);
        btMesh->addTriangle(v1, v2, v3);
    }

    // we can now theoretically test the mesh to see if it's concave/convex and either make a btGimpactMesh
    // or a btConvexHull

    if (modelType == "btBvhTriangleMesh") {
        qDebug() <<"btBvhTriangleMeshShape";
        btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(btMesh, true);
        rshape = shape;
    } else if (modelType == "btGImpactMesh") {
        qDebug() <<"btGImpactMesh";
        btGImpactMeshShape* shape = new btGImpactMeshShape(btMesh);
        shape->setMargin(0.04f);
        shape->updateBound();
        rshape = shape;
    } else if (modelType == "btConvexHull") {
        qDebug() <<"btConvexHull";
        btConvexHullShape* shape = new btConvexHullShape();
        QHash<aiVector3D, bool> seen;
        for(unsigned int n=0; n < m->mNumVertices; n++) {
            if (seen.contains(m->mVertices[n])) { continue; }
            seen[m->mVertices[n]] = true;
            aiVector3D &v = m->mVertices[n];
            shape->addPoint(btVector3(v.x*0.92,v.y*0.92,v.z*0.92),false);
        }
        shape->recalcLocalAabb();
        shape->setMargin(0.04f);
        rshape = shape;
    } else {
        qDebug() <<"Failed to make a btShape";
    }
    if (rshape) m_shapes[name] = rshape;
    return rshape;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

VAO* AssetStore::addVAO(QString name, aiMesh* m) {
    VAO* v = new VAO(m);
    m_vaos[name] = v;
    return v;
}

Renderable *AssetStore::addRenderable(QString name, VAO *v, QSGTexture *t)
{
    Renderable *r = new Renderable(v, t);
    return r;
}
