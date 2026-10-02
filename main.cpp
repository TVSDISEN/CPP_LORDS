#include "Simulator.h"
#include "CSVOutput.h"
#include <iostream>
#include <memory>
#include <string>

// ===== TUNABLE CONSTANTS =====
const double DT           = 0.001;
const double DURATION     = 10.0;
const double SETPOINT     = 100.0;
const double INERTIA      = 0.02;
const double FRICTION     = 0.05;
const double TORQUE_CONST = 0.5;
const double LOAD_TIME    = 5.0;
const double LOAD_TORQUE  = 2.0;

const double KP = 1.5;
const double KI = 6.0;
const double KD = 0.02;
// ==============================

void runScenario(const std::string& name,
                 double kp, double ki, double kd) {

    Motor motor(
        INERTIA,
        FRICTION,
        TORQUE_CONST,
        0.0,
        LOAD_TORQUE
    );

    motor.setLoadDisturbance(LOAD_TIME);

    Controller controller(kp, ki, kd);

    std::string path = "data/" + name + ".csv";

    Simulator sim(
        motor,
        controller,
        std::make_unique<CSVOutput>(path),
        DT,
        SETPOINT
    );

    sim.run(DURATION);

    std::cout << name
              << ": final speed = "
              << sim.getFinalSpeed()
              << " rad/s (target "
              << SETPOINT
              << ") -> "
              << path
              << "\n";
}

int main() {
    try {
        runScenario("pid", KP, KI, KD);
        runScenario("p_only", KP, 0.0, 0.0);
        runScenario("pi", KP, KI, 0.0);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}