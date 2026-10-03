#include "CSVOutput.h"
#include <iostream>
#include <memory>

int main()
{
    std::unique_ptr<Output> output =
        std::make_unique<CSVOutput>("data/test_output.csv");

    output->record({0.0, 100, 0, 100, 24, 0});
    output->record({0.1, 100, 50, 50, 20, 0});
    output->record({0.2, 100, 99, 1, 10, 2});

    output->close();

    std::cout << "Output written to data/test_output.csv\n";

    return 0;
}
