#include "worlddebugdrawer.h"

#include <QDebug>
#include <QFile>
#include <QVector>
#include <QStringList>

#include <sailfishapp.h>

#include "utils.h"

WorldDebugDrawer::WorldDebugDrawer(World *parent) :
    QObject((QObject*)parent)
  , m_worldLines(NULL)
  , m_qlinepoints(500)
{
}

// This sets up the GL objects/meshes that *can* be used in bullet
void WorldDebugDrawer::setupGL()
{
    m_program_debug = new GLProgram(SailfishApp::pathTo("debug_vert.glsl"), SailfishApp::pathTo("debug_frag.glsl"));
}

void WorldDebugDrawer::newFrame()
{
    if (m_worldLines)
        delete m_worldLines;
    m_worldLines = new QHash<Color, QList<Line> >;
}

void WorldDebugDrawer::render(QMatrix4x4 projViewMatrix)
{
    if (m_debug_mode == DBG_NoDebug ) return;

    if (! m_worldLines) return;

    //  Setup shader for debug draw
    QMatrix4x4 worldMatrix; // null atm
    m_program_debug->bind();
    m_program_debug->setUniformValue(m_program_debug->getU("projViewMatrixU"), projViewMatrix);
    m_program_debug->setUniformValue(m_program_debug->getU("worldMatrixU"), worldMatrix);
    glEnableVertexAttribArray(m_program_debug->getA("posA"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // iterate over worldlines
    glLineWidth(2);
    m_worldLineMutex.lock();
    auto i = m_worldLines->constBegin();
    while (i != m_worldLines->constEnd()) {
        Color color = i.key();
        m_program_debug->setUniformValue(m_program_debug->getU("colU"), color.m_c);
        auto list = i++.value();
        if (list.size()*2 > m_qlinepoints.size()) {
            m_qlinepoints.resize(list.size()*2);
        }
        auto j = list.constBegin();
        int n=0;
        while (j != list.constEnd()) {
            Line line = *j++;
            m_qlinepoints[n++] = line.from;
            m_qlinepoints[n++] = line.to;
        }
        glVertexAttribPointer(m_program_debug->getA("posA"), 3, GL_FLOAT, GL_FALSE, 0, m_qlinepoints.data());
        glDrawArrays(GL_LINES, 0, n);
    }
    m_worldLineMutex.unlock();

    glDisableVertexAttribArray(m_program_debug->getA("posA"));
    //    p->bind(); // should do this but we're only going to drop it again

}

void WorldDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    QVector3D qfrom(from.x(), from.y(), from.z());
    QVector3D qto(to.x(), to.y(), to.z());
    QVector4D qcol(color.x(), color.y(), color.z(), 1.0);
    m_worldLineMutex.lock();
    (*m_worldLines)[Color(qcol)].append({qfrom, qto});
    m_worldLineMutex.unlock();
}

void WorldDebugDrawer::reportErrorWarning(const char* warningString) {
    qDebug()<< warningString;
}

void WorldDebugDrawer::draw3dText(const btVector3& location,const char* textString) {
    Q_UNUSED(location);
    qDebug()<< "draw3dText " << textString;
}

void WorldDebugDrawer::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color){
    Q_UNUSED(PointOnB); Q_UNUSED(normalOnB); Q_UNUSED(distance); Q_UNUSED(lifeTime); Q_UNUSED(color);
    qDebug()<< "drawContactPoint()";
}


uint qHash(WorldDebugDrawer::Color c) { return c.m_c.x()*10000 + c.m_c.y()*100 + c.m_c.z(); }
