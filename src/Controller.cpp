#include "Controller.h"
#include <algorithm>

Controller::Controller(double kp, double ki, double kd, double h,
                       double integralLimit, double maxOut)
    : Kp(kp), Ki(ki), Kd(kd), maxOutput(maxOut), lastError(0.0),
      gainP(kp), gainI(ki, integralLimit), gainD(kd), feedbackGain(h) {}

double Controller::computeOutput(double setpoint, double measuredSpeed, double dt) {
    double feedback = feedbackGain.apply(measuredSpeed);
    double error = comparator.computeError(setpoint, feedback);
    lastError = error;

    // only add the terms that are switched on
    double output = 0.0;
    if (Kp != 0.0) output += gainP.compute(error);
    if (Ki != 0.0) output += gainI.compute(error, dt);
    if (Kd != 0.0) output += gainD.compute(error, dt);

    // can't give the motor more than the supply voltage
    return std::clamp(output, -maxOutput, maxOutput);
}

double Controller::getLastError() const {
    return lastError;
}

void Controller::reset() {
    gainI.reset();
    gainD.reset();
    lastError = 0.0;
}
