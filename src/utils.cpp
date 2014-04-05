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

QMatrix4x4 bt2QMatrix4x4(btTransform *transform) {
    float ft[16];
    transform->getOpenGLMatrix(ft);

    return QMatrix4x4(ft[0], ft[1], ft[2], ft[3],
            ft[4],  ft[5],  ft[6],  ft[7],
            ft[8],  ft[9],  ft[10], ft[11],
            ft[12], ft[13], ft[14], ft[15]).transposed();
}

