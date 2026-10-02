# Closed-Loop PID Speed Controller for a DC Motor (C++17)

Team: Vijay, Disen, Manas

## What it does

Simulates a DC motor and a PID controller that keeps its speed at a target
of 100 rad/s, even when a load torque is applied at t = 5 s.

Every timestep is logged to CSV for analysis and Excel plotting.

## Build and run

On Linux/macOS:

    make

On Windows with MinGW:

    mingw32-make

Run the simulation:

    ./motorsim

The simulation generates:

- data/pid.csv
- data/p_only.csv
- data/pi.csv

## Tests

Run the individual tests:

    mingw32-make test_controller
    mingw32-make test_motor
    mingw32-make test_output

Then run the generated test programs:

    ./test_controller
    ./test_motor
    ./test_output

## Project structure

- `include/` - header files
- `src/` - implementation files
- `tests/` - test programs
- `data/` - generated CSV output
- `main.cpp` - simulation entry point
- `Makefile` - build and test commands

## Simulation parameters

The main simulation parameters are defined at the top of `main.cpp`,
including:

- timestep: 0.001 s
- duration: 10 s
- setpoint: 100 rad/s
- motor inertia: 0.02 kg*m^2
- friction: 0.05 N*m*s/rad
- torque constant: 0.5 N*m/V
- load torque: 2 N*m
- load start time: 5 s
- PID gains: Kp = 1.5, Ki = 6.0, Kd = 0.02
