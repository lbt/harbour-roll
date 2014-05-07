#ifndef ROLLBUILDER_H
#define ROLLBUILDER_H

#include "worldbuilder.h"
#include "rollworld.h"

class RollBuilder : public WorldBuilder
{
    Q_OBJECT
public:
    explicit RollBuilder(RollWorld *parent = 0);

    void setup();
    void setTrack(QString track);

signals:

public slots:
protected:
    RollWorld* m_rollworld;
};

#endif // ROLLBUILDER_H
