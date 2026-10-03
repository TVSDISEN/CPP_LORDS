#include "GainP.h"

GainP::GainP(double kp) : Kp(kp) {}

double GainP::compute(double error) const {
    return Kp * error;
}
