#ifndef UTILS_H
#define UTILS_H

#include <QVector3D>
#include <QMatrix4x4>

#include <assimp/scene.h>
#include "bullet/btBulletDynamicsCommon.h"

#define PI 3.1415926535
#define PI2 6.283185307
#define PI180 (180/PI)

// Assimp conversions
static inline QVector3D qv3d(const aiVector3D &v) {return QVector3D(v.x, v.y, v.z); }

// Assimp utils
QMatrix4x4 getNodeMatrix(aiNode *node);
uint qHash(const aiVector3t<float> &v);

// Bullet conversions
// To Qt
QMatrix4x4 bt2QMatrix4x4(btTransform *transform);
static inline QVector3D bt2QtVector3D(const btVector3 &bv) { return QVector3D(bv.x(), bv.y(), bv.z()); }
// To bullet
static inline btVector3 Qt2btVector3 (const QVector3D  &qv) { return btVector3(qv.x(), qv.y(), qv.z()); }
btTransform Qt2btTransform(QMatrix4x4 *qmatrix);

// General utils
namespace { float rnd(float max) { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX/max); } }

#endif // UTILS_H
