#pragma once

// Feedback path gain H (basically the speed sensor).
// H = 1.0 means the controller sees the true speed.
class FeedbackGain {
public:
    explicit FeedbackGain(double h);
    double apply(double measuredSpeed) const;

private:
    double H;
};
