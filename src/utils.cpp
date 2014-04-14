#include "utils.h"

// Assimp

QMatrix4x4 getNodeMatrix(aiNode *node)
{
    QMatrix4x4 nodeMatrix;
    if (node->mTransformation.IsIdentity())
        return nodeMatrix;
    aiQuaternion rotation;
    aiVector3D position;
    aiVector3D scale;
    node->mTransformation.Decompose(scale, rotation, position);
    QVector3D qscale(scale.x,scale.y, scale.z);
    QVector3D qposition(position.x, position.y, position.z);
    QQuaternion qrotation(rotation.w, rotation.x, rotation.y, rotation.z);
    if (!qscale.isNull())
        nodeMatrix.scale(qscale);
    if (!qposition.isNull())
        nodeMatrix.translate(qposition);
    if (!qrotation.isNull())
        nodeMatrix.rotate(qrotation);
    return nodeMatrix;
}

uint qHash(const aiVector3t<float> &v) { //
    return  ((int)(v.x * 73856093) ^
             (int)(v.y * 19349663) ^
             (int)(v.z * 83492791))
            % (uint)-1 ;
}

// Bullet

//check(QMatrix4x4 in) {
//    btTransform b = Qt2btTransform(&in);
//    QMatrix4x4 out = bt2QMatrix4x4(&b);
//    if (out != in) {
//        qDebug() << "in != out " << in << " != " << out;
//    }
//}

QMatrix4x4 bt2QMatrix4x4(btTransform *transform) {
    QMatrix4x4 m;
    transform->getOpenGLMatrix(m.data());
    return m;
}
btTransform Qt2btTransform(QMatrix4x4 *qmatrix) {
    btTransform  transform;
    transform.setFromOpenGLMatrix(qmatrix->data());
    return transform;
}

