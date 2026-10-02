#pragma once
struct SimulationResult {
    double time;
    double setpoint;
    double measuredSpeed;
    double error;
    double controlOutput;
    double loadTorque;
};