// Unit tests for the controller blocks.
// Run with: make test_controller
#include "Controller.h"
#include <cmath>
#include <iostream>

int fails = 0;

void check(const char* name, double got, double expected) {
    bool ok = std::fabs(got - expected) < 1e-9;
    if (!ok) fails++;
    std::cout << (ok ? "PASS  " : "FAIL  ") << name
              << "   got=" << got << " expected=" << expected << "\n";
}

int main() {
    Comparator comp;
    check("Comparator 100 - 80", comp.computeError(100, 80), 20);

    GainP p(2.0);
    check("GainP 2 * 5", p.compute(5), 10);

    // Ki = 1, limit = 3. Error 10 for 0.1 s adds 1 to the sum each call.
    GainI i(1.0, 3.0);
    check("GainI first step", i.compute(10, 0.1), 1.0);
    for (int k = 0; k < 10; k++) i.compute(10, 0.1);
    check("GainI stops at +limit", i.compute(10, 0.1), 3.0);    // would be 12 with no clamp
    for (int k = 0; k < 10; k++) i.compute(-10, 0.1);
    check("GainI stops at -limit", i.compute(-10, 0.1), -3.0);
    i.reset();
    check("GainI after reset", i.compute(10, 0.1), 1.0);

    GainD d(1.0);
    check("GainD first call = 0", d.compute(10, 0.1), 0.0);
    check("GainD (12 - 10) / 0.1", d.compute(12, 0.1), 20.0);
    d.reset();
    check("GainD = 0 again after reset", d.compute(50, 0.1), 0.0);

    FeedbackGain h(1.0);
    check("FeedbackGain 1 * 50", h.apply(50), 50);

    // whole controller with our final gains. Error 100 at the start
    // asks for ~150 V, so it must clamp to the 24 V supply.
    Controller ctrl(1.5, 6.0, 0.02, 1.0, 3.0, 24.0);
    check("Controller clamps at +24 V", ctrl.computeOutput(100, 0, 0.001), 24.0);
    check("Controller lastError", ctrl.getLastError(), 100.0);

    Controller ctrl2(1.5, 6.0, 0.02, 1.0, 3.0, 24.0);
    check("Controller clamps at -24 V", ctrl2.computeOutput(0, 100, 0.001), -24.0);

    // Ki = Kd = 0 -> plain P controller. Error 10 -> 1.5 * 10 = 15 V
    Controller pOnly(1.5, 0.0, 0.0, 1.0, 3.0, 24.0);
    check("P-only controller 1.5 * 10", pOnly.computeOutput(100, 90, 0.001), 15.0);

    if (fails == 0)
        std::cout << "\nALL CONTROLLER TESTS PASSED\n";
    else
        std::cout << "\n" << fails << " TEST(S) FAILED\n";

    return fails == 0 ? 0 : 1;
}
