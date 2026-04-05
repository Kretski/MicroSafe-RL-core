# MicroSafe-RL

**A 24-byte safety interceptor for RL agents on embedded hardware.**

RL policies trained in simulation regularly produce out-of-range actuator commands when deployed on real hardware. This library clamps those commands deterministically, in O(1) time, before they reach the actuator.

---

## The problem

```
Trained policy output:  +4.7   (normalized)
Actual motor range:     ±1.5

Without interception → overcurrent → hardware damage
With MicroSafe-RL    → clipped to +1.5, penalty fed back to agent
```

This is not a novel problem. The novelty here is the interception layer being:
- **24 bytes RAM**, zero dynamic allocation
- **< 1.2 µs WCET** on Cortex-M3 @ 72 MHz (measured via DWT hardware counter)
- **MISRA-C:2012 compliant** — zero critical violations (Cppcheck 2.13.0)
- Usable bare-metal, no RTOS required

---

## How it works

The interceptor tracks a rolling baseline of recent signal behavior using an EMA. When the incoming command deviates significantly from this baseline, it applies a gravity factor that scales the command back toward the safe zone proportionally — not a hard binary cut.

```
penalty  = κ × (EMA_MAD + α × (1 − coherence) + 0.3 × velocity)
gravity  = max(0,  1 − penalty × g)
safe_out = clip(ai_action × gravity,  min_limit,  max_limit)
reward   = 1 − penalty
```

The hard clip (`min_limit` / `max_limit`) is always active from step 1 — even before the EMA has enough history to be meaningful. This is the "hard shield": no matter what state the statistics are in, the output is always within bounds.

The `reward` signal is passed back to the RL agent, so the agent learns over time to produce commands that don't require heavy correction.

---

## Integration

### Embedded C++ (Arduino / STM32)

```cpp
#include "MicroSafeRL_misra.h"

// MicroSafeRL(kappa, alpha, decay, beta, g, min, max, vel_weight)
MicroSafeRL safety(0.078f, 0.55f, 2.2f, 0.12f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float safe_val = safety.apply_safe_control(ai_action, sensor_val);
    float reward   = safety.get_current_reward();
    actuator.set(safe_val);
    agent.update(reward);
}
```

Three lines to integrate. The full Arduino example is in [`examples/MicroSafe_Demo.ino`](examples/MicroSafe_Demo.ino).

### Parameter tuning

Rather than manually tuning `kappa`, `alpha`, etc., run the profiler on a CSV of your signal:

```bash
python microsafe_profiler.py data/input_signal.csv
# Output:
#   MicroSafeRL safety(0.078f, 0.55f, 2.2f, 0.12f, 1.0f, -1.5f, 1.5f, 0.05f);
```

Copy the output line directly into your firmware.

### Python / Gymnasium

```python
from wrappers.microsafe_gym import MicroSafeWrapper
import gymnasium as gym

env = MicroSafeWrapper(gym.make("Pendulum-v1"),
                       safety_params="data/output_signature.csv")
```

Stable Baselines3 and RLLib examples in [`examples/`](examples/).

---

## Benchmark

Tested against a Kalman-filter detector and a PLC threshold system across 120 synthetic scenarios (runaway, adversarial, safe) — see [`paper_mode.py`](paper_mode.py) for the full benchmark code.

| System | Mean detection margin (steps before failure) | Std Dev |
|---|---|---|
| **MicroSafe-RL** | **19.2** | ±1.4 |
| Kalman-filter | 11.0 | ±1.6 |
| PLC threshold | 8.0 | ±0.7 |

All three systems achieve TPR=1.0, FPR=0.0 on this benchmark. The difference is *when* detection happens — MicroSafe-RL flags runaway conditions earlier, leaving more time to intervene.

These are synthetic benchmarks on controlled scenarios. Real-world performance will vary by application.

Hardware validation: Arduino Uno + MPU-6050, 10,000 steps @ 20 Hz. Mean interception latency: 3.2 ms end-to-end (including sensor read and actuator write). WCET of the safety calculation itself: 1.18 µs.

---

## Limitations

- Parameter selection (`kappa`, `alpha`, `g`) affects behavior significantly. The profiler gives a good starting point, but tuning for your specific hardware is recommended.
- The EMA baseline requires a warm-up period. During the first ~20 steps, only the hard clip is active.
- This is not a replacement for hardware-level protection (fuses, current limiters). It is a software layer that reduces the frequency of hitting those limits.
- Tested on Cortex-M3. Latency figures will differ on other architectures.

---

## Repository structure

```
MicroSafe-RL/
├── MicroSafeRL.h                  # Core implementation
├── MicroSafeRL_misra.h            # MISRA-C:2012 edition
├── SafetyBridge.h                 # LLM-to-hardware command bridge
├── microsafe_profiler.py          # Parameter tuner
├── paper_mode.py                  # Benchmark (120 runs)
├── MISRA_compliance_report.txt    # Cppcheck audit output
├── data/                          # Sample signals and signatures
├── examples/                      # Arduino, SB3, RLLib
├── tests/test_safety_core.cpp     # NaN, Inf, edge case tests
└── wrappers/microsafe_gym.py      # Gymnasium wrapper
```

---

## Academic work

Preprint (open access):
> Kretski, D. — *"ORAC-NT v5.x: Optimal and Stable FDIR Architecture for Autonomous Spacecraft and Critical Systems"* — Zenodo, March 2026. [DOI: 10.5281/zenodo.19019599](https://doi.org/10.5281/zenodo.19019599)

A paper based on this work has been submitted to IEEE TAES (under review).

---

## License

MIT for academic and non-commercial use. For production deployment in safety-critical systems, contact [kretski1@gmail.com](mailto:kretski1@gmail.com).