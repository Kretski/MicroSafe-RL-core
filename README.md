
# 🛡️ MicroSafe-RL

![Build](https://img.shields.io/badge/build-passing-brightgreen)
![Latency](https://img.shields.io/badge/latency-535ns-blue)
![Memory](https://img.shields.io/badge/RAM-24B-lightgrey)
![Complexity](https://img.shields.io/badge/complexity-O(1)-orange)
![MISRA](https://img.shields.io/badge/MISRA--C-2012-success)
![License](https://img.shields.io/badge/license-MIT-green)
![DOI](https://img.shields.io/badge/DOI-10.5281%2Fzenodo.19019599-blue)

**Deterministic Safety Filter for Real-Time Control Systems**

MicroSafe-RL is a constant-time safety layer placed between AI/control logic and physical actuators.  
It enforces bounded, predictable behavior on every control cycle.

---

## 🚀 Quick Start

```cpp
#include "MicroSafeRL_misra.h"

MicroSafeRL safety(...);

float safe = safety.apply_safe_control(ai_action, sensor_val);
actuator.set(safe);
No allocation. No async. Runs every cycle.

📍 Pipeline Position
[ AI / RL / LLM ]
        ↓
[ MicroSafe-RL ]
        ↓
[ Actuator ]
📌 Properties
O(1) execution
535 ns latency (STM32 @ 84 MHz)
24 bytes RAM
no dynamic allocation
fixed-point arithmetic (Q-format)
MISRA-C:2012 compliant
🧠 Behavior

At each control step, the system evaluates:

signal instability
deviation from baseline
rate of change

If unsafe behavior is detected, a deterministic constraint is applied.

🛡️ Core Mechanism
penalty = κ × (EMA_MAD + α × (1 - coherence) + v_weight × velocity)

gravity = max(0, 1 - penalty × g)

safe_out = clip(ai_action × gravity, min_limit, max_limit)

Hard clipping is always active.

⚙️ Configuration
MicroSafeRL safety(
    0.078f,   // kappa
    0.55f,    // alpha
    2.2f,     // decay
    0.12f,    // beta
    1.0f,     // gravity
    -1.5f,    // min
    1.5f,     // max
    0.05f     // velocity weight
);
📊 Benchmarks
Metric	Value
Latency	535 ns
Memory	24 bytes
Complexity	O(1)
False Positive Rate	0.05
📊 Comparison
Method	Deterministic	Latency	Adaptivity	Safe by Default
Threshold	✅	✅	❌	⚠️
Kalman Filter	❌	❌	✅	❌
PID Controller	✅	✅	⚠️	❌
MicroSafe-RL	✅	✅	✅	✅
🔬 Failure Modes Covered
signal spikes
drift
oscillations
noisy inputs
invalid AI outputs
⚙️ Design Constraints
no malloc / free
no recursion
constant-time execution
fixed memory footprint
predictable branching
❌ Not Designed For
offline ML
cloud inference
non-physical systems
🧪 Validation
Measured via DWT cycle counter (STM32)
DOI: 10.5281/zenodo.19019599
📦 Integration Example
float safe_val = safety.apply_safe_control(ai_action, sensor_val);
actuator.set(safe_val);
🧭 Positioning

Think of it as:

PID controller — but for enforcing safety constraints

📬 License

## 📬 License

All rights reserved.

This software may not be used, modified, or distributed without explicit permission from the author.

Contact for licensing:
kretski1@gmail.com