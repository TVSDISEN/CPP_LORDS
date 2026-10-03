#pragma once

// I term: keeps adding error * dt, so an error that stays around
// slowly builds up more and more correction.
// The sum is limited to +/- integralLimit (anti-windup), otherwise it
// keeps growing while the motor is stuck at max voltage during startup.
class GainI {
public:
    GainI(double ki, double integralLimit);
    double compute(double error, double dt);
    void reset();

private:
    double Ki;
    double integralLimit;
    double integralSum;
};
