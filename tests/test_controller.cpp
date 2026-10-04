// Unit tests for the controller blocks.
// Run with: make test_controller
#include "Controller.h"
#include <bits/stdc++.h>
using namespace std;
int fails = 0;

void check(const char* name, double got, double expected) {
    bool ok =fabs(got - expected) < 1e-9;
    if (!ok) fails++;
    cout << (ok ? "PASS  " : "FAIL  ") << name<< "   got=" << got << " expected=" << expected << "\n";
}

int main() {
    Comparator comp;
    check("Comparator 100 - 80", comp.computeError(100, 80), 20);

    GainP p(2.0);
    check("GainP 2 * 5", p.compute(5), 10);

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

    Controller ctrl(1.5, 6.0, 0.02, 1.0, 3.0, 24.0);
    check("Controller clamps at +24 V", ctrl.computeOutput(100, 0, 0.001), 24.0);
    check("Controller lastError", ctrl.getLastError(), 100.0);

    Controller ctrl2(1.5, 6.0, 0.02, 1.0, 3.0, 24.0);
    check("Controller clamps at -24 V", ctrl2.computeOutput(0, 100, 0.001), -24.0);

    Controller pOnly(1.5, 0.0, 0.0, 1.0, 3.0, 24.0);
    check("P-only controller 1.5 * 10", pOnly.computeOutput(100, 90, 0.001), 15.0);

    if (fails == 0){
        cout << "\nALL CONTROLLER TESTS PASSED\n";
    }
    else{
        cout << "\n" << fails << " TEST(S) FAILED\n";
    }
    return fails == 0 ? 0 : 1;
}
