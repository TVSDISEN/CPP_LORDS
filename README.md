# PID Speed Controller for a DC Motor (C++17)

**Team CPP_LORDS:** Vijay · Disen · Manas

A C++ program that keeps a motor spinning at exactly the speed we want, even when something tries to slow it down. The motor is not real hardware. We simulate it in code, run the controller against it, and record every millisecond so we can see and check what happened.

---

## The idea in 30 seconds

Think of **cruise control** in a car. You set 100 km/h. The car keeps checking its real speed. If it is too slow, it presses the accelerator harder. If it is too fast, it eases off. When the car starts going uphill and slows down, it notices and pushes harder until it is back at 100.

Our program does the same thing for an electric motor:

1. We choose a **target speed**: 100 rad/s (about 955 revolutions per minute).
2. Every millisecond, the program **measures** the motor's actual speed.
3. It works out **how far off** the motor is.
4. It **adjusts the voltage** sent to the motor to fix the difference.
5. Halfway through, at 5 seconds, we suddenly put an extra **load** on the motor, like a car hitting a hill. The controller has to notice and recover.

---

## Why this is needed

If you just connect a motor to a fixed voltage and walk away, its speed is at the mercy of everything around it. Add a load and it slows down. Nothing notices and nothing corrects it. This is called **open-loop** control.

**Closed-loop** control fixes this by feeding the measured speed back into the decision. The system keeps comparing "where am I?" with "where should I be?" and corrects itself. That feedback is the loop.

---

## How the loop works

![Control loop](docs/diagrams/2_control_loop_block.png)

Read the diagram from left to right:

- The **target speed** goes into the **Comparator**, which subtracts the measured speed. The result is the **error**: how far off we are.
- The error goes to three correction parts, **P**, **I** and **D** (explained below). Their outputs are added together.
- The total is limited to **±24 V**, because a real power supply can't give more than that.
- That voltage drives the **Motor**. A **load** can push against the motor at the same time.
- The motor's new speed goes back through the **sensor** (FeedbackGain) to the Comparator, and the loop starts again.

Each box in this diagram is one C++ class in our code.

---

## What P, I and D mean

The controller is called **PID** because it combines three simple ideas. Each one looks at the error in a different way.

| Part | Looks at | In the cruise-control picture | What it fixes |
|---|---|---|---|
| **P**, Proportional | How far off we are **right now** | Far below 100? Press hard. Nearly there? Press gently. | Gives a fast, strong first reaction |
| **I**, Integral | How long we have **stayed** off target | "We've been slightly slow for a while, push a bit more." | Removes the small error P always leaves behind |
| **D**, Derivative | How **fast** the error is changing | "Speed is rising quickly, ease off before we overshoot." | Reduces overshoot and wobble |

The final voltage is simply `P + I + D`, limited to ±24 V. Our tuned values are **Kp = 1.5, Ki = 6.0, Kd = 0.02**. These numbers set how strongly each part reacts.

Setting a value to 0 switches that part off. That is how the same program also runs a **P-only** and a **PI** controller for comparison.

---

## The motor we simulate

The motor follows one physics rule: **the speed changes based on the push minus everything resisting it.**

```
change in speed = (push from voltage − friction − load) ÷ inertia
```

| Value | What it means | Our number |
|---|---|---|
| Inertia (J) | How hard it is to speed up or slow down. Heavier = slower to react. | 0.02 kg·m² |
| Friction (b) | Drag that grows with speed | 0.05 N·m·s/rad |
| Torque constant (Kt) | How much push 1 volt creates | 0.5 N·m/V |
| Max voltage | Power supply limit | 24 V |
| Load | Extra force against the motor, switched on at 5 s | 2 N·m |

The program applies this rule in tiny steps of **0.001 seconds**, for **10 seconds** (10,001 steps). Each step uses the current speed to work out the next one. This method is called **Euler integration**. The steps are 400 times shorter than the time the motor needs to react, so it is accurate. Our tests confirm it matches hand calculations.

---

## What happens when you run it

`./motorsim` runs three experiments with the same motor and saves each one to a spreadsheet file (CSV):

| Run | Controller | File |
|---|---|---|
| 1 | Full PID | `data/pid.csv` |
| 2 | P only | `data/p_only.csv` |
| 3 | PI (no D) | `data/pi.csv` |

Each row of a CSV file is one millisecond: time, target speed, measured speed, error, voltage, and load.

---

## Results

![P vs PI vs PID](docs/plots/p_pi_pid_comparison.png)

| Controller | Overshoot | Time to settle (within 2%) | Final speed | When the load hits at 5 s |
|---|---|---|---|---|
| P only | none | never reaches 100 | **91.25** | drops and never recovers |
| PI | 4.2% | 0.49 s | **100** | dips to 97.9, recovers |
| **PID (final)** | **4.0%** | **0.52 s** | **100** | **dips to 98.0, back within 1% in 0.29 s** |

**What this shows:**

- **P alone is not enough.** P only produces voltage when there is an error. But the motor needs some voltage just to keep spinning. So it settles where "a little error" makes "just enough voltage", and gets stuck below the target (93.75, then 91.25 with the load).
- **Adding I fixes this.** I keeps building up while any error remains, so it pushes the speed all the way to 100.
- **PID handles the load well.** When the 2 N·m load hits, speed drops by only 2% and recovers in under a third of a second.

![PID response](docs/plots/pid_response.png)

The voltage graph below is our sanity check. The controller starts at the 24 V limit, settles at **10 V** to hold 100 rad/s, then rises to **14 V** to fight the load. Both numbers match what we calculate by hand.

![Control voltage](docs/plots/control_output.png)

How we chose the gains (9 experiments, one change at a time): see [`docs/tuning-notes.md`](docs/tuning-notes.md).

---

## How the code is organised

![Class diagram](docs/diagrams/1_class_diagram.png)

The structure is simple: **one manager (Simulator) runs two main parts (Motor and Controller) and sends results to one recorder (Output).**

| Class | Its one job |
|---|---|
| `Simulator` | Owns everything and runs the step-by-step loop |
| `Motor` | Holds the motor's speed and applies the physics rule each step. Also handles the load. |
| `Controller` | Combines the five blocks below into one voltage, limited to ±24 V |
| `Comparator` | Error = target − measured speed |
| `GainP` | The P part: Kp × error |
| `GainI` | The I part: adds up the error over time (with a safety limit) |
| `GainD` | The D part: how fast the error is changing |
| `FeedbackGain` | The speed sensor. Set to 1.0, a perfect sensor. |
| `Output` | A general "recorder" interface. Says *what* a recorder must do, not *how*. |
| `CSVOutput` | One kind of recorder: writes rows to a CSV file |
| `SimulationResult` | One row of data (one millisecond) |

### What happens in one step

![One timestep](docs/diagrams/3_sequence_one_timestep.png)

Every millisecond, in this order:

1. Read the motor's current speed.
2. The Controller turns that speed into a voltage.
3. Record the row (time, speed, error, voltage, load).
4. Apply the voltage to the motor, which moves it forward by 0.001 s.

We record **before** moving the motor, so each row shows the speed and the voltage from the same moment.

---

## Why we designed it this way

- **One job per class.** The controller is five small classes instead of one big one. Each piece can be built and tested on its own.
- **Private data.** No class can reach in and change another's values. The motor's speed only changes through the motor's own `update()` step.
- **Built from parts ("has-a"), not inherited.** The Controller *has* a Comparator and three gains. It isn't a *kind of* any of them.
- **Swappable output.** The Simulator only knows "something that records results". To print to the screen instead of a file, we write one new recorder class. The Simulator doesn't change.
- **Automatic cleanup.** The Simulator owns the recorder through a smart pointer (`std::unique_ptr`), so memory is freed and the file is closed automatically.
- **Safety limits, like real controllers:**
  - **Voltage limit (±24 V):** a real supply can't give unlimited power.
  - **Integral limit (anti-windup):** at startup the motor is already at full power, but I keeps adding up error anyway. Without a cap, that built-up value causes a big overshoot later. Our tests show 24% overshoot without it, 4% with it.
  - **No first-step spike from D:** on the very first step there is no earlier error to compare with. Without care, D would see a jump from 0 to 100 in one millisecond and demand a huge voltage. So D returns 0 on its first step.
- **All settings in one place.** Every number (gains, motor values, timing) sits at the top of `main.cpp`.

---

## Multithreading (extra goal)

`./motorsim --threaded` runs the same PID experiment, but the controller, the recorder and the motor each run on **their own thread** (three parts of the program running at the same time).

![Threading](docs/diagrams/5_threading_lockstep.png)

The three threads pass a "turn" between them, like a baton in a relay race: controller, then recorder, then motor, then the next millisecond. A **mutex** (a lock) and a **condition variable** (a wake-up signal) make sure only one thread touches the shared data at a time. This prevents threads from reading half-updated values.

**Proof it works:** the threaded output file is byte-for-byte identical to the normal one (`cmp` prints nothing).

To be honest about it: a control loop has to happen in order. The controller needs the latest speed, and the motor needs the latest voltage. So threading here shows *safe* teamwork between threads. It doesn't make the program faster.

---

## How we know it's correct

| Check | What it proves |
|---|---|
| `make test_controller` | 14 checks: each PID block does its math correctly, the limits work, P-only works |
| `make test_motor` | 5 checks against hand calculations: 10 V gives 100 rad/s, 10 V with load gives 60 rad/s, the load starts at the right moment |
| `make test_output` | The recorder writes a correct CSV file |
| Hand calculations | Voltage settles at exactly 10 V and 14 V; P-only stops at exactly 93.75 and 91.25 |
| `cmp` threaded vs normal | Multithreaded version gives the identical result |

---

## Build and run

You need a C++ compiler with C++17 support (g++ 8 or newer) and `make`.

```bash
make                      # build (on Windows with MinGW: mingw32-make)
./motorsim                # run PID, P-only and PI → data/*.csv
./motorsim --threaded     # run PID on 3 threads → data/pid_threaded.csv
cmp data/pid.csv data/pid_threaded.csv   # no output = identical
make test                 # run all unit tests
```

No `make`? Build with one command:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -pthread main.cpp src/*.cpp -o motorsim
```

Windows users: run these in **Git Bash** or **WSL**, not PowerShell.

**To experiment:** change any number in the settings block at the top of `main.cpp`, then run `make && ./motorsim` again. Open the CSV files in Excel to plot them.

---

## Project structure

```
include/     header files (.h): what each class offers
src/         source files (.cpp): how each class works
tests/       unit tests for the controller, motor and output
data/        CSV results (created when you run the program)
docs/
  REPORT.md         full project report
  tuning-notes.md   the 9 gain experiments and what we learned
  class-diagram.md  all UML diagrams (render on GitHub)
  diagrams/         diagram images
  plots/            result graphs
main.cpp     program entry point + all settings
Makefile     build, run and test commands
```

---

## Glossary

| Term | Meaning |
|---|---|
| **Setpoint** | The target speed (100 rad/s) |
| **Error** | Target minus actual speed |
| **rad/s** | Radians per second, a unit of spinning speed. 100 rad/s ≈ 955 RPM. |
| **Overshoot** | How far the speed goes past the target before settling |
| **Settling time** | How long until the speed stays within 2% of the target |
| **Steady-state error** | Error that never goes away (P-only's problem) |
| **Saturation** | The controller asks for more voltage than the supply can give, so it gets capped at 24 V |
| **Integral windup** | The I part keeps growing during saturation, causing overshoot later |
| **Load disturbance** | An outside force that suddenly resists the motor |
| **Closed loop** | A system that measures its own output and corrects itself |

---

## Team

| Member | Built |
|---|---|
| **Vijay** | The controller parts(Comparator, GainP, GainI, GainD, FeedbackGain), gain-tuning experiments, repository setup |
| **Disen** | The motor and load model, |motor tests, |the controller |controller_tests
| **Manas** | The Simulator, the output system (Output, CSVOutput), `main.cpp`, the Makefile, output test, integration |

We wrote the header files first and treated them as a contract between us. That let all three of us code in parallel without stepping on each other's work.

For the full write-up, see [`docs/REPORT.md`](docs/REPORT.md).
