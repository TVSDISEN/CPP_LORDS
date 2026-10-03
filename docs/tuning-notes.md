# Tuning notes

How we ended up with Kp = 1.5, Ki = 6.0, Kd = 0.02 and integral limit = 3.0.

Setup for every run: J = 0.02, b = 0.05, Kt = 0.5, 24 V supply, setpoint 100 rad/s,
dt = 0.001 s, 10 s long, 2 N*m load from t = 5 s. Between runs we only changed the
constants block at the top of main.cpp, ran `make && ./motorsim` and read the numbers
out of data/pid.csv.

How we measured:

- overshoot: highest speed minus 100. Setpoint is 100, so rad/s = %.
- settling: time until the speed stays between 98 and 102 (2% band), looking at 0-5 s only.
- dip: lowest speed after the load comes on at t = 5 s.
- recovery: time from t = 5 s until the speed is back between 99 and 101 and stays there.

| run | change (everything else = final) | overshoot | settling | dip | recovery |
|-----|----------------------------------|-----------|----------|-----|----------|
| a | nothing, final gains | 4.0% | 0.52 s | 98.00 | 0.29 s |
| b | Kd = 0 | 4.2% | 0.49 s | 97.89 | 0.28 s |
| c | Ki = 40 | 29.0% | 0.51 s | 98.61 | 0.08 s |
| d | integral limit = 100 (clamp basically off) | 24.2% | 0.93 s | 98.00 | 0.29 s |
| e | Ki = 40 and integral limit = 100 | 59.1% | 0.65 s | 98.61 | 0.08 s |
| f | Kp = 0.3 | 8.6% | 0.83 s | 95.69 | 0.55 s |
| g | P only, Ki = Kd = 0 (data/p_only.csv) | none | never reaches 100 | 91.25 | never |
| h | integral limit = 1 | none | never reaches 100 | 95.00 | never |
| i | Kd = 0.2 | - | never, stuck around 60 | - | never |

## What each run showed

a) final gains. At t = 0 the controller asks for about 150 V (1.5 * 100 plus a bit of I),
so it sits at the 24 V limit for the start of the climb. Speed peaks at 104.0 rad/s around
t = 0.32 s and stays inside the 2% band from 0.52 s. At steady state the voltage is 10 V,
which is exactly b*w/Kt = 0.05*100/0.5, so the model checks out by hand. When the load hits,
the speed bottoms out at 97.998 about 0.09 s after t = 5 (just touching the edge of the 2% band),
the voltage climbs to 14 V = (0.05*100 + 2)/0.5 and the I term pulls the speed back to 100.

b) Kd = 0. Almost no change (4.2% vs 4.0%). Our motor model is first order, only inertia
and friction, so there isn't much oscillation for D to damp. This run is the same as
data/pi.csv (we compared them, the files are identical).

c) Ki = 40. Overshoot goes up to 29%. The load dip actually gets better though (98.61,
back in 0.08 s), because a strong I term reacts faster to an error that doesn't go away.
So Ki is a trade-off: startup overshoot vs how fast we fight off a load. Also, the clamp is
on the sum and not on the I output, so with Ki = 40 the I term can still give 40 * 3 = 120 V.
The limit of 3 only makes sense together with Ki = 6 (max 18 V from I).

d) integral limit = 100. 24% overshoot and settling almost doubles. For roughly the first
0.2 s the output is stuck at 24 V anyway, but the sum keeps growing (windup). All that extra
sum has to be unwound after we pass 100, and that is the overshoot. The load part is identical
to (a): after the load the I term only needs a sum of 14 / 6 = 2.33, which is under 3, so the
clamp never kicks in there.

e) Ki = 40 and limit = 100. 59%. Both problems at once.

f) Kp = 0.3. Slower (0.83 s) and more overshoot (8.6%), because now the I term is doing
most of the work and I is the part that overshoots. Worst load dip of the PI/PID runs (95.69)
and 0.55 s to recover. Weak P means a weak immediate reaction.

g) P only. Never reaches 100. It settles at 93.75 before the load and 91.25 after.
Hand check, at steady state Kt * Kp * (100 - w) = b * w + load:
no load, 0.75(100 - w) = 0.05w gives w = 93.75;
with 2 N*m, 75 - 0.75w = 0.05w + 2 gives w = 91.25. Same as the CSV.
P needs some error to make any voltage, so it can never get to zero error. That is why we need I.

h) integral limit = 1. The limit can't be too small either. The I term can then give at
most 6 * 1 = 6 V, but holding 100 rad/s needs 10 V (14 V with the load), so P has to make up
the rest, and P only does that with an error. Speed sits at 97.5, then 95.0 after the load.
The limit has to leave room for what the I term needs at steady state.

i) Kd = 0.2. This one goes unstable. With dt = 0.001 the D term is 0.2 / 0.001 = 200 times
the change in error per step, so a tiny change in speed swings the voltage a lot, which causes
a bigger change next step. In the CSV the voltage flips between +24 V and about -12 V on almost
every step and the speed never gets anywhere near 100. Kd = 0.02 is well away from this.

## Final choice

Kp = 1.5, Ki = 6.0, Kd = 0.02, integral limit = 3.0. About 4% overshoot, settled in about half
a second, and after the 2 N*m load it dips only about 2% and is back within 1% in under 0.3 s.
A bigger Ki fights the load faster but overshoots badly at startup, so we stayed at 6.
D adds very little for this motor; we kept a small Kd so the D block is actually in use,
and run b shows what the response looks like without it.
