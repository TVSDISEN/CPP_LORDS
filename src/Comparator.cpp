#include "Comparator.h"

double Comparator::computeError(double setpoint, double feedbackSignal) const {
    return setpoint - feedbackSignal;
}
