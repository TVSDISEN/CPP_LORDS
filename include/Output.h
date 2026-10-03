#pragma once

#include "SimulationResult.h"

class Output
{
public:
    virtual void record(const SimulationResult& result) = 0;
    virtual void close() = 0;
    virtual ~Output() = default;
};
