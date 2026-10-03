#include "CSVOutput.h"
#include <stdexcept>

CSVOutput::CSVOutput(const std::string& filename) : file(filename)
{
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open " + filename);
    }

    file << "time,setpoint,measured_speed,error,control_output,load_torque\n";
}

void CSVOutput::record(const SimulationResult& result)
{
    file << result.time << ","
         << result.setpoint << ","
         << result.measuredSpeed << ","
         << result.error << ","
         << result.controlOutput << ","
         << result.loadTorque << "\n";
}

void CSVOutput::close()
{
    if (file.is_open())
    {
        file.close();
    }
}

CSVOutput::~CSVOutput()
{
    close();
}
