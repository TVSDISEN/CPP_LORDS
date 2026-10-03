#include "FeedbackGain.h"

FeedbackGain::FeedbackGain(double h) : H(h) {}

double FeedbackGain::apply(double measuredSpeed) const {
    return H * measuredSpeed;
}
