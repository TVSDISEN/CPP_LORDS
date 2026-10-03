#include "GainI.h"
#include <algorithm>

GainI::GainI(double ki, double limit)
    : Ki(ki), integralLimit(limit), integralSum(0.0) {}

double GainI::compute(double error, double dt) {
    integralSum += error * dt;
    integralSum = std::clamp(integralSum, -integralLimit, integralLimit);
    return Ki * integralSum;
}

void GainI::reset() {
    integralSum = 0.0;
}
