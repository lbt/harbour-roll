#include "dicemodel.h"

DiceModel::DiceModel()
{
}

void DiceModel::addD6()
{
    if (! m_d6Shape) {
        m_d6Shape = new btBoxShape(btVector3(.5,.5,.5));
        collisionShapes.push_back(m_d6Shape);
    }
}
