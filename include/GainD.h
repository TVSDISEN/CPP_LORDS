#pragma once

// D term: reacts to how fast the error is changing.
// First call returns 0 because there is no previous error yet.
// (if prevError just started at 0, step 1 would be (100 - 0) / 0.001
//  = 100000 -> one huge spike, the "derivative kick")
class GainD {
public:
    explicit GainD(double kd);
    double compute(double error, double dt);
    void reset();

private:
    double Kd;
    double prevError;
    bool firstCall;
};
