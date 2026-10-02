#pragma once

#include "Output.h"
#include <fstream>
#include <string>

class CSVOutput : public Output {
public:
    explicit CSVOutput(const std::string& filepath);
    void record(const SimulationResult& result) override;
    void close() override;
    ~CSVOutput() override;

private:
    std::ofstream file;
};