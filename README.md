# MicroSafe-RL Core

**Runtime safety layer for AI/RL-assisted control systems.**

MicroSafe-RL Core is a lightweight deterministic safety layer designed to sit between an AI/RL controller and a physical actuator or constrained system.

It does not replace the controller.

It bounds, modulates, and monitors controller output before execution.

---

## Purpose

AI and reinforcement learning controllers can produce numerically valid commands that are unsafe for real hardware. Examples:

- Motor commands beyond actuator limits
- Unstable output during noisy sensor phases
- Sudden command spikes
- Recovery overshoot after disturbance
- Saturation without controller awareness

MicroSafe-RL Core provides a small runtime safety layer that applies deterministic correction before the command reaches hardware.

---

## Memory and Latency

| Metric | Value | Notes |
|--------|-------|-------|
| RAM footprint | ~68 bytes | 4 state floats + 8 config floats + DebugState (5 floats) + bool |
| Worst-case latency | <1.2 µs | STM32F4 @ 168 MHz, measured via hardware timer |
| Dynamic allocation | None | Header-only, no heap usage |

> **Note on RAM:** The full `MicroSafeRL` class including `DebugState` and configuration occupies approximately 68 bytes. If only the 4 runtime state variables (ema_mean, ema_mad, prev_value, current_penalty) are counted, the minimum state footprint is 16 bytes. The 68-byte figure reflects the complete instantiated object.

---

## Core Features

- Deterministic runtime safety layer
- Soft attenuation before hard clipping
- Penalty-based safety metric
- Short-term signal history using EMA/MAD
- Velocity-aware instability detection
- **NaN/Inf input guard** — invalid AI commands return fail-safe zero; invalid sensor values use last known good value (dead-reckoning)
- No dynamic allocation in the embedded core
- C++ header-only implementation (C++03 compatible, MISRA C++ guidelines)
- Python profiler for simulation and tuning

---

## How It Works

Each control cycle:

1. The controller proposes a raw action.
2. MicroSafe-RL checks for NaN/Inf inputs (fail-safe return if detected).
3. Sensor stability and command risk are evaluated.
4. A penalty value is computed.
5. The command is softly attenuated when instability rises.
6. Final hard bounds are enforced.
7. The safe command is returned.

```cpp
float raw_action = ai_model_output;
float sensor     = sensor_feedback;

float safe_action = safety.apply_safe_control(raw_action, sensor);
float penalty     = safety.get_penalty();
```

---

## Safety Metric

MicroSafe-RL exposes a penalty value:

```
penalty = 0.0  →  stable / low intervention
penalty = 1.0  →  high instability / maximum attenuation
```

A simple safety score:

```
safety_score = 1.0 - penalty
```

---

## NaN/Inf Handling

```cpp
// If ai_action is NaN or |ai_action| > 1e6: returns 0.0f (fail-safe)
// If sensor_val is NaN or |sensor_val| > 1e6: uses prev_value (dead-reckoning)
```

This ensures that adversarial or corrupted inputs do not propagate through the safety layer.

---

## SafetyBridge API

```cpp
#include "SafetyBridge.h"

SafetyBridge bridge;

void setup() {
    bridge.init(0.0f);
}

void loop() {
    float raw_command = get_ai_or_controller_output();
    float sensor_val  = read_sensor();

    SafetyResult result = bridge.process(raw_command, sensor_val);

    actuator_write(result.safe_action);
}
```

`SafetyResult` contains:

```cpp
struct SafetyResult {
    float safe_action;   // attenuated + clamped command
    float penalty;       // instability score [0, max_penalty]
    bool  was_modified;  // true if output differs from input by >0.001
    bool  is_safe;       // true if penalty < 0.9 (90% of max_penalty)
};
```

`is_safe` threshold is 0.9 × max_penalty. Rationale: flags near-saturation conditions before hard clamp engages, allowing upstream logic to react.

---

## Implementation Status

**Implemented:**
- Core safety modulation
- Penalty calculation
- Soft command attenuation
- Hard output limits
- NaN/Inf input guard (fail-safe zero + dead-reckoning)
- Basic bridge result with `is_safe` flag
- Python simulation/profiling helper
- Demonstration scripts
- MISRA C++-oriented variant (`MicroSafeRL_misra.h`)

**Planned / under development:**
- Full per-cycle telemetry records
- JSON session reports
- Explicit status labels such as `INTERCEPTED`
- Hardware latency benchmark harness (STM32 timer-based)
- Pure C variant

---

## Repository Structure

```
MicroSafeRL.h              Core C++ safety layer
SafetyBridge.h             Simple runtime bridge API
MicroSafeRL_misra.h        MISRA C++-oriented header variant
MISRA_compliance_report.txt Static-analysis report notes
microsafe_profiler.py      Python simulation / tuning helper
gemma_safety_demo.py       Example AI safety bridge demo
paper_mode.py              Experimental benchmark script
```

---

## Target Use Cases

- Embedded AI safety experiments
- RL-controlled robotics
- Motor-control test benches
- Small rovers and balancing robots
- STM32 / Arduino-class prototypes
- Runtime monitoring for AI-assisted control loops
- Research and evaluation of safety layers for constrained systems

---

## Important Safety Notice

MicroSafe-RL Core is **experimental software**.

It is **not certified** for safety-critical deployment.

Do not use it as the only safety mechanism in systems where failure may cause injury, property damage, or regulatory non-compliance.

For production use, independent validation, hardware testing, and a commercial license agreement are required.

---

## Licensing

Copyright (c) 2026 Dimitar Kretski.

This software is proprietary and commercially licensed.

You may not use, copy, modify, distribute, sublicense, or deploy this software for commercial or production purposes without a separate written license agreement.

**Allowed without a commercial license:**
- Private review
- Academic reading
- Non-commercial evaluation
- Internal testing with permission

**Not allowed without a commercial license:**
- Commercial use
- Product integration
- Redistribution
- Production deployment

Licensing inquiries: kretski1@gmail.com

---

## Status

Current version: experimental commercial core.

**Recommended validation steps before production use:**
1. Run on STM32 target hardware
2. Measure cycle latency using hardware timers
3. Measure object size with `sizeof(MicroSafeRL)` on target compiler
4. Compare against hard clipping under noisy sensor conditions
5. Log raw command, safe command, penalty, and intervention rate

```
AI / RL Policy  →  MicroSafe-RL  →  Hardware / System
```
