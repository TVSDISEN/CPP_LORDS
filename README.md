# DC Motor Speed Controller (PID) in C++

A C++ program that simulates a DC motor and uses a PID controller to keep its speed at **100 rad/s**.
At **t = 5 s** a 2 N·m load hits the motor, and the controller pulls the speed back up.
Results are saved as CSV files for plotting in Excel.

**Team CPP_LORDS:** Vijay, Disen, Manas

---

## How it works

```
setpoint ─►(+)─► [ P + I + D ] ─► [ Motor ] ─┬─► speed
  100       ▲-     max 24 V          ▲ load   │
            └────────── [ H ] sensor ◄────────┘
```

Every 1 ms: read speed → find error (`100 - speed`) → P, I, D suggest a voltage → cap at ±24 V → update motor → save a row.

## Classes

| Class | Job |
|---|---|
| `Comparator` | Error = setpoint - feedback |
| `GainP` / `GainI` / `GainD` | The three PID terms |
| `FeedbackGain` | The sensor (H) |
| `Controller` | Holds and wires all the blocks above |
| `Motor` | Speed, inertia, friction and load |
| `SimulationResult` | One row of data |
| `Output` → `CSVOutput` | Saves rows to a CSV file |
| `Simulator` | Runs the loop |

Set any gain to `0` to switch that term off.

---

## Build and run

Needs `g++` (C++17) and `make`. On Windows use MinGW and type `mingw32-make` instead of `make`.

```bash
make
./motorsim
```

Output:

```
pid: final speed = 100 rad/s (target 100) -> data/pid.csv
p_only: final speed = 91.25 rad/s (target 100) -> data/p_only.csv
pi: final speed = 100 rad/s (target 100) -> data/pi.csv
```

Run it from the main project folder, or it can't find `data/`.

## Plot in Excel

1. Open `data/pid.csv`.
2. Select the `time` and `measured_speed` columns.
3. **Insert → Scatter → Scatter with Straight Lines.**

CSV columns: `time, setpoint, measured_speed, error, control_output, load_torque`

## Tests

```bash
make test_controller && ./test_controller   # should say ALL CONTROLLER TESTS PASSED
make test_motor && ./test_motor             # should print 60
make test_output && ./test_output           # makes data/test_output.csv
```

## Change settings

All numbers are at the top of `main.cpp` (gains, load, setpoint, time step). Change one, then run `make` and `./motorsim` again.

---

## Results

Final gains: **Kp = 1.5, Ki = 6.0, Kd = 0.02**

- About **4% overshoot**, settles in **0.52 s**
- Load dips speed to about **98**, back in under **0.3 s**
- **P only** never reaches 100 (stops at 91.25). That's why we need I.
- Without anti-windup, overshoot jumps to **24%**

Full tuning runs are in [`docs/tuning-notes.md`](docs/tuning-notes.md).

## Folder layout

```
main.cpp     settings + runs 3 scenarios (PID, P only, PI)
include/     headers
src/         code
tests/       tests
data/        CSV output
docs/        tuning notes and plots
```

## Not done yet

Putting the output csv files data in a graph format (stretch goal).
