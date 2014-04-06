#include "worldbuilder.h"
#include "sailfishapp.h"

WorldBuilder::WorldBuilder(QObject *parent) :
    QObject(parent)
  , m_assetStore(new AssetStore())
  , m_world(new World())
{
}

bool WorldBuilder::load(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();

    }

    // Create an instance of the Importer class
    Assimp::Importer importer;
    importer.SetExtraVerbose(true);
    // Attaching myStream to the default logger
    Assimp::DefaultLogger::create("",Assimp::Logger::VERBOSE);

    const unsigned int severity = Assimp::Logger::Debugging|Assimp::Logger::Info|Assimp::Logger::Err|Assimp::Logger::Warn;
    Assimp::DefaultLogger::get()->attachStream( new dbgStream(), severity );
    Assimp::DefaultLogger::get()->setLogSeverity( Assimp::Logger::VERBOSE );
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.

    //    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT);
    const aiScene* scene = importer.ReadFile( filename.toStdString(),
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
        qDebug() << importer.GetErrorString();
        return false;
    }

    qDebug() << "There are " << scene->mNumTextures << " textures in the file and " << scene->mNumMaterials << " materials";

    // Now we can access the file's contents.
    importChildren(scene, scene->mRootNode);

    Assimp::DefaultLogger::kill();
    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

// Declare an external hack window :: FIXME
#include <QQuickWindow>
extern QQuickWindow* global_hack_window;
void WorldBuilder::importChildren(const aiScene *scene, aiNode *node)  {
    QMatrix4x4 transform;

    QString name = node->mName.C_Str();

    qDebug() << "Processing node " << name;

    if( node->mNumMeshes > 0)
    {
        qDebug() << "found a full node with " << node->mNumMeshes << " meshes";
        // copy the mesh : FIXME assuming only 1 mesh
        aiMesh* m = scene->mMeshes[node->mMeshes[0]];
        VAO* v = m_assetStore->addVAO(name, m);

        // Import any textures
        int nTex = m->GetNumUVChannels();
        if (nTex > 0) {// http://assimp.sourceforge.net/lib_html/materials.html
            qDebug() << "Found " << nTex << " texture (UV) channels in material[" << m->mMaterialIndex << "]";
            aiMaterial *mat = scene->mMaterials[m->mMaterialIndex];
            aiString matname, path;
            mat->Get(AI_MATKEY_NAME, matname);
            if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                qDebug() << "and lives at " << path.C_Str();
                // FIXME at 5.2:
                //            m_texture = new QOpenGLTexture(QImage(path.data).mirrored());
                glEnable(GL_TEXTURE_2D);
                QSGTexture* texture = global_hack_window ->
                        createTextureFromImage(QImage(SailfishApp::pathTo(path.data).toLocalFile()).mirrored());
                if (texture->isAtlasTexture()) { texture = texture->removedFromAtlas(); }
                texture->setHorizontalWrapMode(QSGTexture::Repeat);
                texture->setVerticalWrapMode(QSGTexture::Repeat);
                m_assetStore->addTexture(matname.C_Str(), texture);

                // This is a mesh with a texture - we can probably render that so link them as a Renderable
                m_assetStore->addRenderable(name, v, texture);
            } else {
                qDebug() << "and has no path";
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

void WorldBuilder::setup(){

    // for all in json

//    WorldItem* wi = new WorldItem();

}
