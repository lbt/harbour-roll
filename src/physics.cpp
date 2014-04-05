#include "physics.h"
#include "utils.h"
#include <QDebug>

Physics::Physics(QObject *parent) :
    QObject(parent)
{
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief BiMeshContainer::copyMeshes
/// \param scene
/// \param node
///
/// First create a btMesh then use it to create a suitable btShape
///
///
void Physics::addShape(QString modelType, btScalar r)
{
    if (modelType == "btSphere") {
        qDebug() <<"btSphereShape";
        btSphereShape* shape = new btSphereShape(r);
        m_shape = shape;
    } else {
        qDebug() <<"Unknown shape " << modelType;
        m_shape = NULL;
    }
}

void Physics::addShape(QString modelType, aiMesh* m) {

    if (! m) {
        qDebug() << "Mesh type "<< modelType << " requested and no mesh passed";
        return;
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
        m_shape = shape;
    } else if (modelType == "btGImpactMesh") {
        qDebug() <<"btGImpactMesh";
        btGImpactMeshShape* shape = new btGImpactMeshShape(btMesh);
        shape->setMargin(0.04f);
        shape->updateBound();
        m_shape = shape;
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
        m_shape = shape;
    } else {
        m_shape = NULL;
        qDebug() <<"Failed to make a btShape";
    }
}
