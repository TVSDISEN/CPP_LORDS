#pragma once
class Controller {
    private:
    double Ki;
    double Kp;
    double Kd;

    double integral;
    double previousError;
    double integralLimit;
    public:
    Controller(double Kp,double Ki,double Kd);
    double calculate(double setPoint,double measuredSpeed,double dt);
};