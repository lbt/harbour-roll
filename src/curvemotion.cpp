#include "curvemotion.h"

CurveMotion::CurveMotion(WorldItem *parent) :
    BaseMotion(parent)
  , m_speed(10.0)
  , m_curveVAO(NULL)
  , m_lastSeenLine(0)
  , m_circumference(0)
  , m_currentDist(0)
{
}

void CurveMotion::setCurve(VAO* vao)
{
    m_curveVAO = vao;
    QVector3D vec[2];
    m_circumference = 0.0;
    m_distance << 0.0; // First point is at 0
    for (unsigned int n = 0; n < vao->m_numFaces; ++n) {
        vao->faceAt(n, vec);
//        qDebug() << "line " << n << " is " << vec[0] << " to " << vec[1];
        m_circumference += (vec[1] - vec[0]).length();
        m_distance << m_circumference;
    }
//    qDebug() << "curve is " << m_circumference << " long and has " << m_distance.size() << " points";
}

void CurveMotion::runStep(int deltaTms) {
    if (!m_curveVAO) return;
    m_currentDist += m_speed * (deltaTms/1000.0);
    if (m_currentDist > m_circumference) m_currentDist -= m_circumference;
    int numPoints = m_distance.size();
    int numLines = numPoints -1;
    float dist1;
    float dist2;
    do {
        dist1 = m_distance[m_lastSeenLine];
        dist2 = m_distance[m_lastSeenLine+1];
        if (dist1 <= m_currentDist and dist2 >= m_currentDist) // loop until we bracket the current distance
            break;
        m_lastSeenLine++ ;
        m_lastSeenLine %= numLines;
    } while (true);
    QVector3D vec[2];
    m_curveVAO->faceAt(m_lastSeenLine, vec);
    QVector3D vec2[2];
    m_curveVAO->faceAt((m_lastSeenLine + 1) % numLines , vec2);
    QVector3D res = vec[0] + (vec[1]-vec[0])* // line from point 1->2 * fraction of dist (always <1)
            ((m_currentDist - dist1)/(dist2 - dist1));
    QVector3D interp = vec2[0] + (vec2[1]-vec2[0])* // line from point 1->2 * fraction of dist (always <1)
            ((m_currentDist - dist1)/(dist2 - dist1));
    interp.setZ(interp.z()-(dist2-dist1)/10); // tilt down

//    qDebug() << "Point "<< m_lastSeenLine << " : distance " << m_currentDist<< "is between " <<  dist1 <<" and " << dist2 << " from " << vec[0] << " to " << vec[1];
    m_transform.setToIdentity();
    m_transform.lookAt(res, interp, QVector3D(0,0,1)); // look-at vec[1] is too jittery. Need to interpolate to vec[1]->vec[2]
    m_transform = m_transform.inverted();
//    m_transform.translate(res);
}
