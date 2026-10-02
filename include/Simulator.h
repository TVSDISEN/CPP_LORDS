#pragma once

#include "Motor.h"
#include "Controller.h"
#include "Output.h"
#include <memory>

// Top-level owner. ALL wiring happens in the constructor.
class Simulator {
public:
    Simulator(Motor motor, Controller controller, std::unique_ptr<Output> output,
              double dt, double setpoint);

    void run(double durationSeconds);          // serial loop
    void runThreaded(double durationSeconds);  // stretch goal: 3 threads in lockstep

    double getFinalSpeed() const;

private:
    Motor motor;
    Controller controller;
    std::unique_ptr<Output> output;
    double dt;
    double setpoint;
};