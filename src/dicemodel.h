#ifndef DICEMODEL_H
#define DICEMODEL_H
#include "bullet.h"

class DiceModel : public Bullet
{
public:
    DiceModel();

public slots:
    void addD6();

private:
    btCollisionShape *m_d4Shape;
    btCollisionShape *m_d6Shape;
    btCollisionShape *m_d8Shape;
    btCollisionShape *m_d10Shape;
    btCollisionShape *m_d12Shape;
    btCollisionShape *m_d20Shape;
};

#endif // DICEMODEL_H
