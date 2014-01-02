#ifndef GSWARM_H
#define GSWARM_H

#include <QList>
#include "gparticle.h"

class GSwarm : public QList<GParticle>
{
    Q_OBJECT
public:
    GSwarm();
};

#endif // GSWARM_H
