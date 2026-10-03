#pragma once

#include "Motor.h"
#include "Controller.h"
#include "Output.h"
#include <memory>

class Simulator
{
public:
    Simulator(Motor motor,
              Controller controller,
              std::unique_ptr<Output> output,
              double dt,
              double setpoint);

    void run(double durationSeconds);
    double getFinalSpeed() const;

private:
    Motor motor;
    Controller controller;
    std::unique_ptr<Output> output;
    double dt;
    double setpoint;
};
