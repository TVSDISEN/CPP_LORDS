#include "Controller.h"
Controller::Controller(double Kp,double Ki,double Kd):Kp(Kp),Ki(Ki),Kd(Kd),integral(0.0),previousError(0.0),integralLimit(3.0){}
double Controller::calculate(double setpoint,double measuredSpeed,double dt){
    double error=setpoint-measuredSpeed;

    integral+=error*dt;
    if (integral > integralLimit){
        integral = integralLimit;
    }
    if (integral < -integralLimit){
        integral = -integralLimit;
    }
    double derivative=(error-previousError)/dt;

    double output=Kp*error+Ki*integral+Kd*derivative;

    previousError=error;
    return output;
}