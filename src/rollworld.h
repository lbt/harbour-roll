#ifndef ROLLWORLD_H
#define ROLLWORLD_H

#include "world.h"
#include <QJsonDocument>

class RollWorld : public World
{
    Q_OBJECT
public:
    explicit RollWorld(QObject *parent = 0);

    void load();

signals:

public slots:

};

#endif // ROLLWORLD_H
