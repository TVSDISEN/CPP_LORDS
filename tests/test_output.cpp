#include "CSVOutput.h"
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<Output> out =
        std::make_unique<CSVOutput>("data/test_output.csv");

    out->record({0.0, 100, 0,   100, 24, 0});
    out->record({0.1, 100, 50,   50, 20, 0});
    out->record({0.2, 100, 99,    1, 10, 2});

    out->close();

    std::cout << "Wrote data/test_output.csv - open it, expect 1 header + 3 rows\n";

    return 0;
}