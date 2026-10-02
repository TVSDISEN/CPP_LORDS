#include "CSVOutput.h"
#include <stdexcept>

CSVOutput::CSVOutput(const std::string& filepath) : file(filepath) {
    if (!file.is_open())
        throw std::runtime_error("Cannot open " + filepath + " (does data/ exist?)");

    file << "time,setpoint,measured_speed,error,control_output,load_torque\n";
}

void CSVOutput::record(const SimulationResult& r) {
    file << r.time << ',' << r.setpoint << ',' << r.measuredSpeed << ','
         << r.error << ',' << r.controlOutput << ',' << r.loadTorque << '\n';
}

void CSVOutput::close() {
    if (file.is_open()) {
        file.flush();
        file.close();
    }
}

CSVOutput::~CSVOutput() {
    close();
}