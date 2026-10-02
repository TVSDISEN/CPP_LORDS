#pragma once
class Motor{
    private:
    double speed;
    double inertia;
    double friction;
    double torqueConstant;
    double loadStartTime;
    double loadTorque;

    public:
    Motor(double inertia,double friction,double torqueConstant,double initialSpeed,double loadTorque);
    void setLoadDisturbance(double startTime);
    double getLoadAt(double t) const;
    double getSpeed() const;
    void update(double controlSignal,double t,double dt);
};