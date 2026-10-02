#include "Motor.h"
Motor::Motor(double inertia,double friction,
    double torqueConstant,double initialSpeed,double loadTorque)
    : speed(initialSpeed),
      inertia(inertia),
      friction(friction),
      torqueConstant(torqueConstant),
      loadStartTime(0.0),
      loadTorque(loadTorque){}
void Motor::setLoadDisturbance(double startTime){
    loadStartTime=startTime;
}
double Motor::getLoadAt(double t) const {
    if (t>=loadStartTime){
        return loadTorque;
    }
    return 0.0;
}
void Motor::update(double controlSignal,double t,double dt){
    double acc=(torqueConstant * controlSignal-friction*speed-getLoadAt(t))/inertia;
    speed+=acc*dt;
}
double Motor::getSpeed() const{
    return speed;
}
