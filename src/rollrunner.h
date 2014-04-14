#ifndef ROLLRUNNER_H
#define ROLLRUNNER_H

class RollRunner : public WorldRunner
{
    Q_OBJECT
public:
    explicit RollRunner(RollWorld *w);

public slots:
    void setup();
    void setDebugDraw(bool state);
    void runStep();

    void gravity(bool state);
    void fly(bool state) { m_fly = state; }

protected:
    RollWorld*  m_rollWorld;

private:
    QAccelerometer m_sensor;
    bool m_gravity;
    bool m_fly;
};


#endif // ROLLRUNNER_H
