#include "assetstore.h"
#include "utils.h"
#include "sailfishapp.h"

AssetStore::AssetStore(World *w, QObject *parent) :
    QObject(parent)
  , m_world(w)
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

Renderable *AssetStore::makeRenderable(QString name, VAO *v, Texture *t)
{
    qDebug() <<"Make renderable " << name << " from T:" << t->objectName() << " and V:" << v;
    if (m_renderables.contains(name)) {
        qDebug() <<"Existing renderable " << name;
    }
    Renderable *r = new Renderable(name, v, t);
    m_renderables[name] = r;
    return r;
}

Shader* AssetStore::makeShader(QString name, QUrl v_glsl_path, QUrl f_glsl_path) {
    qDebug() <<"Make shader " << name;
    if (m_shaders.contains(name)) {
        qDebug() <<"Existing shader " << name;
    }
    Shader *s = new Shader(v_glsl_path, f_glsl_path, m_world);
    m_shaders[name] = s;
    return s;
}

btCollisionShape* AssetStore::makeShape(QString name, QString modelType, btScalar r)
{
    qDebug() <<"Make "<< modelType << " shape " << name;
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

btCollisionShape* AssetStore::makeShape(QString name, QString modelType, btScalar r, btScalar h)
{
    qDebug() <<"Make "<< modelType << " shape " << name;
    btCollisionShape* shape = NULL;
    if (m_shapes.contains(name)) {
        qDebug() <<"Existing shape " << name;
    }
    if (modelType == "btCapsuleShapeX") {
        qDebug() <<"btCapsuleShapeX";
        shape = new btCapsuleShapeX(r, h);
        if (shape) m_shapes[name] = shape;
    } else if (modelType == "btCapsuleShapeZ") {
        qDebug() <<"btCapsuleShapeZ";
        shape = new btCapsuleShapeZ(r, h);
        if (shape) m_shapes[name] = shape;
    } else if (modelType == "btConeShapeZ") {
        qDebug() <<"btConeShapeZ";
        shape = new btConeShapeZ(r, h);
        if (shape) m_shapes[name] = shape;
    } else if (modelType == "btConeShapeZ") {
        qDebug() <<"btConeShapeZ";
        shape = new btConeShapeZ(r, h);
        if (shape) m_shapes[name] = shape;
    } else {
        qDebug() <<"Unknown shape " << modelType;
    }
    return shape;
}

btCollisionShape* AssetStore::makeShape(QString name, QString modelType, btVector3 sides)
{
    qDebug() <<"Make "<< modelType << " shape " << name;
    btBoxShape* shape = NULL;
    if (m_shapes.contains(name)) {
        qDebug() <<"Existing shape " << name;
    }
    if (modelType == "btBox") {
        qDebug() <<"btBoxShape";
        shape = new btBoxShape(sides);
        if (shape) m_shapes[name] = shape;
    } else {
        qDebug() <<"Unknown shape " << modelType;
    }
    return shape;
}

btCollisionShape* AssetStore::makeShape(QString name, QString modelType, aiMesh* m) {
    qDebug() <<"Make "<< modelType << " shape " << name;
    btCollisionShape* rshape= NULL;
    if (m_shapes.contains(name)) {
        qDebug() <<"Existing shape " << name;
    }
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
            qDebug() << "This is a curve with " << f->mNumIndices << " indices for this face";
        } else {
            aiVector3D* av = (m->mVertices+*ind++);
            btVector3 v1 = btVector3(av->x, av->y, av->z);
            av = (m->mVertices+*ind++);
            btVector3 v2 = btVector3(av->x, av->y, av->z);
            av = (m->mVertices+*ind++);
            btVector3 v3 = btVector3(av->x, av->y, av->z);
            btMesh->addTriangle(v1, v2, v3);
        }
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

Texture *AssetStore::makeTexture(QString name, QImage img)
{
    qDebug() <<"Make texture " << name;
    if (m_textures.contains(name)) {
        qDebug() <<"Existing texture " << name;
    }
    Texture* t = new Texture(name, img);
    m_textures[name] = t;
}

VAO* AssetStore::makeVAO(QString name, aiMesh* m) {
    qDebug() <<"Make VAO " << name;
    VAO* v = new VAO(m);
    m_vaos[name] = v;
    return v;
}

/////////////////////////////////////////////////////////////////////////
/// \brief AssetStore::load
/// \param filename
/// \return
///
bool AssetStore::load(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
    }

    // Create an instance of the Importer class
    m_importer = new Assimp::Importer();
    m_importer->SetExtraVerbose(true);
    // Attaching myStream to the default logger
    Assimp::DefaultLogger::create("",Assimp::Logger::VERBOSE);

    // This next linew enables/disables assimp logging
//    const unsigned int severity = Assimp::Logger::Debugging|Assimp::Logger::Info|Assimp::Logger::Err|Assimp::Logger::Warn;
//    Assimp::DefaultLogger::get()->attachStream( new dbgStream(), severity );
    Assimp::DefaultLogger::get()->setLogSeverity( Assimp::Logger::VERBOSE );
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.

    //    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    m_importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT);
    const aiScene* scene = m_importer->ReadFile( filename.toStdString(),
                                              aiProcess_CalcTangentSpace |
                                              aiProcess_Triangulate |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_SortByPType|
                                              aiProcess_ValidateDataStructure|
                                              aiProcess_FindDegenerates
                                              );
    qDebug() << "Parsed " << filename;

    // If the import failed, report it
    if( !scene)
    {
        qDebug() << m_importer->GetErrorString();
        return false;
    }

    qDebug() << "There are " << scene->mNumTextures << " textures in the file and " << scene->mNumMaterials << " materials";

    // Now we can access the file's contents.
    importChildren(scene, scene->mRootNode);

    Assimp::DefaultLogger::kill();
    // We're done. Everything will be cleaned up by the importer destructor when
    // we call load_finished()
    return true;
}

void AssetStore::load_finished()
{
    delete m_importer;
}

// Declare an external hack window :: FIXME
#include <QQuickWindow>
extern QQuickWindow* global_hack_window;

void AssetStore::importChildren(const aiScene *scene, aiNode *node)  {
    QMatrix4x4 transform;

    QString name = node->mName.C_Str();

    qDebug() << "Processing node " << name;

    if( node->mNumMeshes > 0)
    {
//        qDebug() << "found a full node with " << node->mNumMeshes << " meshes";
        // copy the mesh : FIXME assuming only 1 mesh
        aiMesh* m = scene->mMeshes[node->mMeshes[0]];
        qDebug() <<"Make Mesh " << name;
        m_meshes[name] = m;
        VAO* v = makeVAO(name, m);

        // Import any textures
        int nTex = m->GetNumUVChannels();
        if (nTex > 0) {// http://assimp.sourceforge.net/lib_html/materials.html
//            qDebug() << "Found " << nTex << " texture (UV) channels in material[" << m->mMaterialIndex << "]";
            aiMaterial *mat = scene->mMaterials[m->mMaterialIndex];
            aiString matname, path;
            mat->Get(AI_MATKEY_NAME, matname);
            if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
//                qDebug() << "and lives at " << path.C_Str();
                // FIXME at 5.2:
                //            m_texture = new QOpenGLTexture(QImage(path.data).mirrored());
                Texture* texture = makeTexture(matname.C_Str(), QImage(SailfishApp::pathTo(path.data).toLocalFile()).mirrored());

                // This is a mesh with a texture - we can probably render that so link them as a Renderable
                if (! texture->isNull()) makeRenderable(name, v, texture);
            } else {
                qDebug() << "UV has no path";
            }
        }
    } else  {
        qDebug() << "found an empty node with no meshes";
        // if no meshes, skip the node, but keep its transformation
        // transform = getNodeMatrix(node) * accTransform;
    }

    // continue for all child nodes
    for( unsigned int a = 0; a < node->mNumChildren; a++) {
        qDebug() << "found a child mesh";
        importChildren(scene, node->mChildren[a]);
    }
}

/////////////////////////////////////////////////
/// \brief AssetStore::setupGL
///
/// called from the render thread
void AssetStore::setupGL(){
    qDebug() << "Setup GL";
    for (auto k : m_textures.keys()) {
        qDebug() << "GL for texture " << k;
        Texture *t = m_textures[k];
        t->setupGL();
    }
    qDebug() << "Setup GL done";
}
