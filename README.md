<p align="center">
  <img src="https://img.shields.io/badge/latency-1.18_µs_WCET-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/RAM-24_bytes-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Logic-Deterministic_O(1)-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-IEEE_TAES_Under_Review-blue?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL</h1>
<p align="center">
  <b>Deterministic Sub-Microsecond Safety Layer for Edge AI</b><br/>
  <i>Designed for industrial-grade embedded safety in mission-critical environments.</i>
</p>

---

## 📈 Executive Summary

**MicroSafe-RL** is an ultra-lightweight, bare-metal C++ interceptor that protects physical hardware from Reinforcement Learning (RL) instability. It acts as a **deterministic safety shield** between AI-generated actions and electronic actuators — intercepting unsafe commands in under 1.18 µs, before they reach the hardware.

| Metric | Value |
|---|---|
| **Worst-Case Latency (WCET)** | 1.18 µs on Cortex-M3 @ 72 MHz |
| **RAM Footprint** | 24 bytes, zero dynamic allocation |
| **Algorithm Complexity** | O(1) per step |
| **Hard Shielding** | Active from step 1, before any statistics |
| **C++ Standard** | C++11, header-only, zero dependencies |

---

## 🧠 How It Works — Three Pillars

### 1. Proactive Guidance (Dynamic Gravity)
The plugin tracks **signal entropy** in real time using EMA (Exponential Moving Average) and MAD (Mean Absolute Deviation). When it detects abnormal vibration or sensor drift, it applies a "gravity" factor — softly pulling AI actions back toward the safe zone **before** a critical event occurs.

```
penalty  = κ × [EMA_MAD + α × (1 − coherence) + 0.3 × velocity]
gravity  = 1 − penalty × g_factor
safe_out = clip(ai_action × gravity, min_limit, max_limit)
```

### 2. Last-Microsecond Shielding (Hard Shielding)
A mathematically guaranteed hard clamp blocks any action that exceeds configured physical limits. Implemented via bare-metal register access — **no OS, no RTOS, no interrupt latency**.

```cpp
// Hard shielding active before any statistics (step 1)
if (ai_action > max_limit) return max_limit;
if (ai_action < min_limit) return min_limit;
```

### 3. Hardware Risk Translator
RL agents understand only rewards. MicroSafe-RL translates **physical wear, vibration, and drift** into a negative reward signal (Penalty), teaching the agent to execute tasks in the **least destructive way possible**.

```
reward = 1.0 − penalty
```

---

## 📊 Real-World Evidence — Hardware Log (STM32F4, COM5)

```
[ STABLE  ] AI: 1.31 | Safe: 1.31 | Reward: 1.00 | Cycles: 119
[ STABLE  ] AI: 1.49 | Safe: 1.49 | Reward: 1.00 | Cycles: 119
[ STABLE  ] AI: 1.53 | Safe: 1.50 | Reward: 1.00 | Cycles: 110  ← safety clamp
[ STABLE  ] AI: 1.68 | Safe: 1.50 | Reward: 1.00 | Cycles: 110  ← AI exceeds limit
[! CHAOS !] Signal: -0.226 | Penalty: 0.637 | RL_Reward: 0.363
[! CHAOS !] Signal:  0.787 | Penalty: 0.807 | RL_Reward: 0.193
[ STABLE  ] Signal:  0.311 | Penalty: 0.021 | RL_Reward: 0.979  ← full recovery
```

Latency measured with **DWT hardware cycle counter** (not millis() — direct clock register access).

---

## ⚖️ Benchmarking

| Metric | Cloud-based RL | PyTorch Mobile | **MicroSafe-RL** |
|---|---|---|---|
| Latency (WCET) | 200 ms+ | 10 ms+ | **1.18 µs** |
| RAM Footprint | GBs | MBs | **24 bytes** |
| Dynamic Allocation | Yes | Yes | **Zero** |
| Safety Type | Reactive | Predictive | **Deterministic / Proactive** |
| Works without OS | No | No | **Yes** |

---

## 🔬 Academic Status & Verifiability

This work is grounded in a formally published theoretical framework.

> **Published preprint (Zenodo, Open Access):**
> *"ORAC-NT v5.x: Optimal and Stable FDIR Architecture for Autonomous Spacecraft and Critical Systems"*
> Kretski, Dimitar — Version v2, March 14, 2026
> DOI: [10.5281/zenodo.19019599](https://doi.org/10.5281/zenodo.19019599)
> **71 views · 4 downloads**

> **Submitted to peer review:**
> *"A Control Lyapunov Metric for Autonomous Fault Recovery in Embedded and Aerospace Systems"*
> *IEEE Transactions on Aerospace and Electronic Systems* — submitted March 14, 2026, currently under review.

### Theoretical Foundation — Control Lyapunov Function

The instability metric at the core of MicroSafe-RL derives from the CLF formulation in the published paper:

```
V = T − Q · D

where:
  T  = accumulated system stress
  Q  = detection reliability
  D  = decision capability

Stability condition:  V < 0  (capability exceeds stress)
Recovery law:         u* = argmin_u V(x, u)
```

**Validated results from the paper (9,000 adversarial simulation missions):**

| Fault Scenario | Detection Rate | False Alarms |
|---|---|---|
| Silent Drift | 100% | 0 |
| Byzantine | 100% | 0 |
| Cascading | 100% | 0 |
| Final Boss | 100% | 0 |

**Hardware validation:** Arduino Uno Q + MPU-6050 IMU, 10,000 continuous steps @ 20 Hz (8.3 minutes)
- Mean detection latency: **3.2 ms**
- Recovery rate under mechanical shocks: **95.7%**
- JAXOpt energy optimization: **35.6% reduction** on ResNet18 under thermal stress (87°C)

---

## 🛠️ Installation & Integration

### 🐍 Python — Auto-Tuner & Gymnasium Wrapper

Profile your hardware and auto-generate optimal constants:

```bash
# Profile from real telemetry CSV
python tools/microsafe_profiler.py data/telemetry.csv

# Output example:
# ✅ Optimal parameters found:
#   kappa = 0.078
#   alpha = 0.55
#   decay = 0.95
#   beta  = 0.84
# MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);
```

Use the Gymnasium wrapper for sim-to-real parity during training:

```python
import gymnasium as gym
from wrappers.microsafe_gym import MicroSafeWrapper

env = gym.make("CartPole-v1")
env = MicroSafeWrapper(env, safety_params="config/motor_profile.json")
# Safety layer active during training — same constraints as deployment
```

### 🔌 Embedded C++ — Drop-in Deployment

Header-only. Drop `MicroSafeRL.h` into your project. C++11, zero dependencies, zero dynamic allocation.

```cpp
#include "MicroSafeRL.h"

// Use auto-tuner output directly:
MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float ai_action  = rl_agent.get_action();
    float sensor_val = read_sensor();

    float safe_action = safety.apply_safe_control(ai_action, sensor_val);
    float reward      = safety.get_current_reward(sensor_val);

    actuator.set(safe_action);
    rl_agent.update_reward(reward);
}
```

### 💡 Quick Start — 3 Steps

1. **Profile:** `python tools/microsafe_profiler.py data/telemetry.csv`
2. **Initialize:** Copy the generated `MicroSafeRL safety(...)` line into your `.ino` or `.cpp`
3. **Shield:** Replace `actuator.set(ai_action)` with `actuator.set(safety.apply_safe_control(ai_action, sensor_val))`

---

## 📦 Repository Structure

```
MicroSafe-RL/
├── MicroSafeRL.h                  # Production: EMA-based, O(1), 24 bytes
├── MicroSafeRL_v3_buffer.h        # Research: 20-step history buffer, O(N)
├── MyQuantumProject.ino           # STM32 benchmark with DWT cycle counter
├── tools/
│   └── microsafe_profiler.py      # Auto-tuner: CSV → optimal C++ constants
├── wrappers/
│   └── microsafe_gym.py           # Gymnasium wrapper for RL training
└── config/
    └── motor_profile.json         # Example hardware profile
```

---

## 🔄 Two Variants

| | `MicroSafeRL.h` (v1) | `MicroSafeRL_v3_buffer.h` (v3) |
|---|---|---|
| **Algorithm** | EMA + MAD + velocity | σ(history) + f_familiarity |
| **Memory** | 24 bytes, O(1) | 80 bytes, O(N) |
| **Hard Shielding** | ✅ Active step 1 | ❌ Not included |
| **Best for** | Production deployment | Research / offline analysis |

**Recommendation:** Use v1 for all deployment. Use v3 for research and parameter validation.

---

## 📬 Licensing

- **Open Source (MIT):** Academic and non-commercial research. Attribution required.
- **Commercial:** For private IP acquisition, MISRA-C compliance audits, safety certification (IEC 61508, DO-178), or hardware porting (FPGAs / ASICs):

📧 **kretski1@gmail.com**

---

## 🔗 Related Work

- **ORAC-NT v5.x — FDIR Architecture for Autonomous Spacecraft** (published)
  Formal theoretical foundation for this safety stack. CLF-based fault detection, Byzantine consensus, CubeSat validation.
  DOI: [10.5281/zenodo.19019599](https://doi.org/10.5281/zenodo.19019599)

- **ORAC-NT v7e — Quantum Node Guardian**
  Extends MicroSafe-RL with a 30-step uncertainty window (U(t) = σ(E) + α·(1−f_familiarity)) and autonomous sacrifice protocol for NMR quantum processors (SpinQ Gemini).

- **Autonomous FDIR for CubeSat Systems**
  Full spacecraft fault recovery stack. Combines MicroSafe-RL (actuator layer) with ORAC-NT (system vitality layer).
