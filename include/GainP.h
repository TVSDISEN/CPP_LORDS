#pragma once

// P term: reacts to the error right now
class GainP {
public:
    explicit GainP(double kp);
    double compute(double error) const;

private:
    double Kp;
};
