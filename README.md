# Closed-Loop PID Speed Controller for a DC Motor

A C++17 program that keeps an electric motor spinning at a chosen speed, even when an outside force suddenly tries to slow it down.

The motor is not real hardware. We **simulate** it in code, connect a **PID controller** to it, run the system for 10 seconds, and record every millisecond to a file. We then turn that data into **graphs** to see and check how well the controller works.

---

## Contents

1. [The problem](#1-the-problem)
2. [The solution: closed-loop control](#2-the-solution-closed-loop-control)
3. [How a PID controller works](#3-how-a-pid-controller-works)
4. [The motor we simulate](#4-the-motor-we-simulate)
5. [Simulation settings](#5-simulation-settings)
6. [System workflow](#6-system-workflow)
7. [Class design (UML)](#7-class-design-uml)
8. [Results](#8-results)
9. [Extension: graph visualisation](#9-extension-graph-visualisation)
10. [Testing and output](#10-testing-and-output)
11. [Design decisions](#11-design-decisions)
12. [How to build and run](#12-how-to-build-and-run)
13. [Project structure](#13-project-structure)
14. [Glossary](#14-glossary)

---

## 1. The problem

If you connect a motor to a fixed voltage and leave it, its speed depends on everything around it. Add a load, and it slows down. Nothing notices, and nothing fixes it.

This is called **open-loop control**: the system gives a command but never checks the result.

## 2. The solution: closed-loop control

Think of **cruise control** in a car. You set 100 km/h. The car keeps checking its real speed. Too slow? It presses the accelerator harder. Too fast? It eases off. When the car goes uphill and slows down, it notices and pushes harder until it is back at 100.

Our program does the same for a motor. Every millisecond it:

1. **Measures** the motor's actual speed.
2. **Compares** it with the target speed. The difference is called the **error**.
3. **Adjusts** the voltage sent to the motor to reduce that error.

Because the measured speed is fed back into the next decision, this is called a **closed loop**.

## 3. How a PID controller works

The controller decides the voltage using three simple ideas. Each looks at the error in a different way.

| Part | What it looks at | Cruise-control example | What it does |
|---|---|---|---|
| **P** (Proportional) | How far off we are **right now** | Far below 100? Press hard. Close? Press gently. | Fast, strong first reaction |
| **I** (Integral) | How long we have **stayed** off target | "Still a bit slow after a while? Push more." | Removes the small error P leaves behind |
| **D** (Derivative) | How **fast** the error is changing | "Speeding up quickly? Ease off early." | Reduces overshoot and wobble |

```
voltage = (Kp × error) + (Ki × error added up over time) + (Kd × rate of change of error)
```

The result is limited to **±24 V**, because a real power supply cannot give more.

**Kp, Ki and Kd** are the "gains". They set how strongly each part reacts. Setting a gain to 0 switches that part off. This lets the same program also run a **P-only** and a **PI** controller, so we can compare all three.

## 4. The motor we simulate

The motor follows one physics rule:

> **change in speed = (push from voltage − friction − load) ÷ inertia**

| Property | What it means | Value |
|---|---|---|
| Inertia (J) | How hard it is to speed up or slow down | 0.02 kg·m² |
| Friction (b) | Drag that grows with speed | 0.05 N·m·s/rad |
| Torque constant (Kt) | How much push 1 volt creates | 0.5 N·m/V |
| Maximum voltage | Power supply limit | 24 V |
| Load disturbance | Extra force against the motor, switched on at 5 s | 2 N·m |

The program applies this rule in very small time steps of 0.001 s. Each step uses the current speed to calculate the next one. This method is called **Euler integration**. It is accurate here because each step is 400 times shorter than the time the motor needs to react (J ÷ b = 0.4 s).

## 5. Simulation settings

| Setting | Value |
|---|---|
| Target speed (setpoint) | 100 rad/s (about 955 RPM) |
| Time step | 0.001 s |
| Duration | 10 s (10,001 steps) |
| Load | 2 N·m, applied at t = 5 s |
| PID gains | Kp = 1.5, Ki = 6.0, Kd = 0.02 |
| Integral limit (anti-windup) | ±3.0 |
| Sensor gain (H) | 1.0 (a perfect sensor) |

All of these values are kept in one block at the top of `main.cpp`, so they can be changed without touching any class.

---

## 6. System workflow

![System workflow](images/system_workflow.jpeg)

The top row shows the closed loop. The **target speed** enters the **comparator**, which subtracts the **measured speed** to get the error. The **PID controller** turns the error into a **control signal** (a voltage). The **motor** responds with a new speed. The **simulator** coordinates the loop, and **Output** writes every step to a **CSV file**. The measured speed goes back to the comparator along the feedback line, closing the loop.

The bottom row is what happens in every time step:

| Step | What happens |
|---|---|
| **1. Measure** | Read the current motor speed |
| **2. Calculate error** | Error = target speed − measured speed |
| **3. Controller** | Compute the voltage using P, I and D |
| **4. Update motor** | Apply the voltage; the motor speed changes by one time step |
| **5. Record output** | Save one row (time, speed, error, voltage, load) to the CSV file |
| **6. Repeat** | Continue until all 10,001 steps are done |

## 7. Class design (UML)

![UML class diagram](images/uml_class_diagram.png)

The design has **one coordinator (Simulator)** that uses **two main parts (Motor and Controller)** and sends results to **one recorder (Output)**.

| Class | Its one job |
|---|---|
| `Simulator` | Owns the Motor, Controller and Output, and runs the time loop |
| `Motor` | Stores the motor's speed and applies the physics rule each step. Also models the load. |
| `Controller` | Turns the error into a voltage using P, I and D, limited to ±24 V |
| `Output` | An **interface**: it says *what* a recorder must do (`record`, `close`), not *how* |
| `CSVOutput` | One kind of recorder: writes each row to a CSV file |
| `SimulationResult` | One row of data for one time step |

**Inside the Controller.** In the code, the Controller is built from five small classes, each with one job: `Comparator` (calculates the error), `GainP`, `GainI` and `GainD` (the three PID parts), and `FeedbackGain` (the speed sensor). The Controller combines their results.

**Reading the diagram:**

- **Filled diamond (composition):** the Simulator *owns* the Motor, Controller and Output. They are created with it and removed with it.
- **Hollow triangle (implementation):** `CSVOutput` *is a* kind of `Output`.
- **Dashed arrow (dependency):** the Simulator *creates* a `SimulationResult` each step and passes it to Output.

The **System Workflow** panel at the bottom-left of the image shows the same loop as Section 6, step by step through the classes.

---

## 8. Results

We ran the same motor with three controllers. Each run is saved to its own CSV file: `data/pid.csv`, `data/pi.csv` and `data/p_only.csv`.

![Motor speed response](images/speed_response.jpeg)

| Controller | Overshoot | Time to settle (within 2%) | Final speed | When the load hits at 5 s |
|---|---|---|---|---|
| **PID** | 4.0% | 0.52 s | **100 rad/s** | dips to 98.0, recovers within 0.3 s |
| **PI** | 4.2% | 0.49 s | **100 rad/s** | dips to 97.9, recovers |
| **P only** | none | never reaches target | **91.25 rad/s** | drops and never recovers |

**What the graph shows:**

- **PID and PI reach the target.** Both rise quickly, go slightly past 100, and settle in about half a second.
- **P-only gets stuck below the target.** P only creates voltage when there is an error. But the motor always needs some voltage just to keep spinning against friction. So it settles at a point where a small error gives "just enough" voltage: 93.75 rad/s, then 91.25 rad/s once the load is added.
- **The load at 5 s.** All three controllers see a dip. PID and PI recover fully because the I part keeps pushing until the error is gone. P-only cannot recover.

![Final speed comparison](images/final_speed_comparison.png)

This bar chart summarises the final speed of each controller after 10 seconds. The dashed line is the target of 100 rad/s.

**Checking the results by hand.** To hold 100 rad/s, the motor needs a voltage of friction × speed ÷ Kt = 0.05 × 100 ÷ 0.5 = **10 V**. With the 2 N·m load it needs (5 + 2) ÷ 0.5 = **14 V**. The simulation settles at exactly 10 V and then 14 V. P-only's final speeds of 93.75 and 91.25 rad/s also match the hand calculation exactly.

## 9. Extension: graph visualisation

The core program produces CSV files with 10,001 rows each. Numbers alone are hard to read, so as our extension we turned this data into graphs:

- **Speed vs time** for PID, PI and P-only on one chart, with the load disturbance marked at 5 s.
- **Final speed comparison** as a bar chart against the 100 rad/s target.

These graphs make the behaviour of each controller visible at a glance: how fast it rises, how much it overshoots, how it reacts to the load, and whether it reaches the target. They also let us check the simulation against our hand calculations.

## 10. Testing and output

![Testing and output](images/testing_output.jpeg)

Each main part has its own test program:

| Test | What it checks |
|---|---|
| `test_controller` | Each PID part does its maths correctly; the ±24 V limit and the integral limit work; P-only mode works (14 checks) |
| `test_motor` | The motor physics matches hand-calculated values (for example, 10 V with a 2 N·m load settles at 60 rad/s) |
| `test_output` | The recorder writes a correct CSV file |

**CSV format.** Every row is one time step. These are real rows from `data/pid.csv`:

| time | setpoint | measured_speed | error | control_output | load_torque |
|---|---|---|---|---|---|
| 0 | 100 | 0 | 100 | 24 | 0 |
| 0.001 | 100 | 0.6 | 99.4 | 24 | 0 |
| 0.002 | 100 | 1.1985 | 98.8015 | 24 | 0 |
| 5.001 | 100 | 99.9 | 0.1 | 12.15 | 2 |

At the start the controller asks for more than 24 V, so the output is capped at 24 V. Just after 5 s the load appears and the controller raises the voltage to fight it.

---

## 11. Design decisions

**Object-oriented design**

- **One job per class.** Every class does one thing, which makes each part easy to build, test and change.
- **Private data (encapsulation).** No class changes another's data directly. The motor's speed changes only through its own `update()` step.
- **Built from parts (composition).** The Controller *has* a comparator and three gain parts. It is not a *kind of* any of them.
- **Swappable output (abstraction and polymorphism).** The Simulator only knows the `Output` interface. A new output type, such as printing to the screen, needs one new class and no change to the Simulator.
- **Automatic cleanup.** The Simulator owns the Output through `std::unique_ptr`, so memory is freed and the file is closed automatically.
- **Clear errors.** If the CSV file cannot be opened, the program stops with a clear message instead of silently writing nothing.

**Safety limits, as in real controllers**

- **Voltage limit (±24 V).** A real power supply cannot give unlimited voltage.
- **Integral limit (anti-windup).** At startup the motor is already at full voltage, but the I part keeps adding up error anyway. Without a limit, this stored-up value causes a large overshoot later. In our tests, removing the limit raised the overshoot from 4% to 24%.
- **No spike from D on the first step.** On the first step there is no earlier error to compare with. Without care, D would see the error jump from 0 to 100 in one millisecond and demand a huge voltage. So D returns 0 on its first step.



## 12. How to build and run

**Requirements:** a C++17 compiler (g++ 8 or newer) and `make`. On Windows, use the **Git Bash** or **WSL** terminal.

```bash
make                    # build (Windows with MinGW: mingw32-make)
./motorsim              # runs PID, P-only and PI
```

This creates three files: `data/pid.csv`, `data/p_only.csv` and `data/pi.csv`.

**Run the tests:**

```bash
make test_controller && ./test_controller
make test_motor      && ./test_motor
make test_output     && ./test_output
```

**No make?** Build with one command:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude main.cpp src/*.cpp -o motorsim
```

**To experiment:** change any value in the settings block at the top of `main.cpp`, then build and run again.

## 13. Project structure

```
include/     header files (.h): what each class offers
src/         source files (.cpp): how each class works
tests/       test programs for the controller, motor and output
data/        CSV results (created when the program runs)
docs/        tuning notes
images/      diagrams and graphs used in this README
main.cpp     program entry point and all settings
Makefile     build and test commands
```

## 14. Glossary

| Term | Meaning |
|---|---|
| **Setpoint** | The target speed (100 rad/s) |
| **Error** | Target speed minus measured speed |
| **rad/s** | Radians per second, a unit of spinning speed. 100 rad/s ≈ 955 RPM. |
| **Closed loop** | A system that measures its own result and corrects itself |
| **Overshoot** | How far the speed goes past the target before settling |
| **Settling time** | Time until the speed stays within 2% of the target |
| **Steady-state error** | An error that never goes away (the P-only problem) |
| **Saturation** | The controller asks for more than 24 V, so the output is capped |
| **Integral windup** | The I part keeps growing during saturation and causes overshoot |
| **Load disturbance** | An outside force that suddenly resists the motor |
| **CSV** | A simple spreadsheet file where values are separated by commas |


## Team

| Member | Contribution |
|---|---|
| **Vijay** | Controller building blocks (Comparator, GainP, GainI, GainD, FeedbackGain) and repository setup |
| **Disen** | Motor and load model, Controller, and the motor and controller tests |
| **Manas** | Simulator, output system (Output, CSVOutput), `main.cpp`, Makefile, output test, and integration |

We wrote the header files first and treated them as a contract between us. This let all three of us work in parallel without conflicts.
