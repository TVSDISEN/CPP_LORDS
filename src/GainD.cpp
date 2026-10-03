#include "GainD.h"

GainD::GainD(double kd) : Kd(kd), prevError(0.0), firstCall(true) {}

double GainD::compute(double error, double dt) {
    if (firstCall) {
        prevError = error;
        firstCall = false;
        return 0.0;
    }

    double derivative = (error - prevError) / dt;
    prevError = error;
    return Kd * derivative;
}

void GainD::reset() {
    prevError = 0.0;
    firstCall = true;
}
