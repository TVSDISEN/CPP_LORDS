#pragma once
#include "Comparator.h"
#include "GainP.h"
#include "GainI.h"
#include "GainD.h"
#include "FeedbackGain.h"

// PID controller made out of the small blocks above (has-a, no inheritance).
// Setting a gain to 0 switches that term off, so the same class works
// as P, PI, PD or PID.
class Controller {
public:
    Controller(double kp, double ki, double kd, double h,
               double integralLimit, double maxOutput);

    double computeOutput(double setpoint, double measuredSpeed, double dt);
    double getLastError() const;
    void reset();

private:
    double Kp, Ki, Kd;
    double maxOutput;   // supply voltage limit
    double lastError;   // saved so the Simulator can log it

    Comparator comparator;
    GainP gainP;
    GainI gainI;
    GainD gainD;
    FeedbackGain feedbackGain;
};
