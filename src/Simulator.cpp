#include "Simulator.h"
#include <utility>

Simulator::Simulator(Motor m, Controller c, std::unique_ptr<Output> out,
                     double dtIn, double sp)
    : motor(std::move(m)),
      controller(std::move(c)),
      output(std::move(out)),
      dt(dtIn),
      setpoint(sp) {}

void Simulator::run(double durationSeconds) {
    int steps = static_cast<int>(durationSeconds / dt + 0.5);

    for (int i = 0; i <= steps; ++i) {
        double t = i * dt;
        double measuredSpeed = motor.getSpeed();

        double error = setpoint - measuredSpeed;

        double u = controller.calculate(
            setpoint, measuredSpeed, dt
        );

        SimulationResult r{
            t,
            setpoint,
            measuredSpeed,
            error,
            u,
            motor.getLoadAt(t)
        };

        output->record(r);
        motor.update(u, t, dt);
    }

    output->close();
}

double Simulator::getFinalSpeed() const {
    return motor.getSpeed();
}
