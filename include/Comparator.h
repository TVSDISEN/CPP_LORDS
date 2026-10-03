#pragma once

// Comparator block of the loop: error = setpoint - feedback
class Comparator {
public:
    double computeError(double setpoint, double feedbackSignal) const;
};
