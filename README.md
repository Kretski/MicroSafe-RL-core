# MicroSafe-RL Core

**Runtime safety layer for AI/RL-assisted control systems.**

MicroSafe-RL Core is a lightweight deterministic safety layer designed to sit between an AI/RL controller and a physical actuator or constrained system.

It does not replace the controller.

It bounds, modulates, and monitors controller output before execution.

Purpose
AI and reinforcement learning controllers can produce numerically valid commands that are unsafe for real hardware.
Examples:
motor commands beyond actuator limits
unstable output during noisy sensor phases
sudden command spikes
recovery overshoot after disturbance
saturation without controller awareness
MicroSafe-RL Core provides a small runtime safety layer that applies deterministic correction before the command reaches hardware.
Core Features
Deterministic runtime safety layer
Soft attenuation before hard clipping
Penalty-based safety metric
Short-term signal history using EMA/MAD
Velocity-aware instability detection
No dynamic allocation in the embedded core
Small state footprint
Suitable for STM32 / Arduino-class control loops
C++ header-only implementation
Python profiler for simulation and tuning
How It Works
Each control cycle:
The controller proposes a raw action.
MicroSafe-RL evaluates sensor stability and command risk.
A penalty value is computed.
The command is softly attenuated when instability rises.
Final hard bounds are enforced.
The safe command is returned.
float raw_action = ai_model_output;
float sensor     = sensor_feedback;

float safe_action = safety.apply_safe_control(raw_action, sensor);
float penalty     = safety.get_penalty();
Safety Metric
MicroSafe-RL exposes a penalty value:
penalty = 0.0 -> stable / low intervention
penalty = 1.0 -> high instability / maximum penalty
A simple safety score can be derived as:
safety_score = 1.0 - penalty
SafetyBridge API
SafetyBridge.h provides a simple wrapper around the core controller.
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
SafetyResult contains:
struct SafetyResult {
    float safe_action;
    float penalty;
    bool  was_modified;
    bool  is_safe;
};
Current Implementation Status
Implemented:
Core safety modulation
Penalty calculation
Soft command attenuation
Hard output limits
Basic bridge result
Python simulation/profiling helper
Demonstration scripts
Planned / under development:
Full per-cycle telemetry records
JSON session reports
Explicit status labels such as INTERCEPTED
Hardware latency benchmark harness
NaN/Inf input guard in the embedded header
Pure C / MISRA-oriented variant
Target Use Cases
Embedded AI safety experiments
RL-controlled robotics
Motor-control test benches
Small rovers and balancing robots
STM32 / Arduino-class prototypes
Runtime monitoring for AI-assisted control loops
Research and evaluation of safety layers for constrained systems
Important Safety Notice
MicroSafe-RL Core is experimental software.
It is not certified for safety-critical deployment.
Do not use it as the only safety mechanism in systems where failure may cause injury, property damage, or regulatory non-compliance.
For production use, independent validation, hardware testing, and a commercial license agreement are required.
Repository Structure
MicroSafeRL.h              Core C++ safety layer
SafetyBridge.h             Simple runtime bridge API
microsafe_profiler.py      Python simulation / tuning helper
gemma_safety_demo.py       Example AI safety bridge demo
paper_mode.py              Experimental benchmark script
MicroSafeRL_misra.h        MISRA-oriented C++ header variant
MISRA_compliance_report.txt Static-analysis report notes
Licensing
Copyright (c) 2026 Dimitar Kretski.
This software is proprietary and commercially licensed.
You may not use, copy, modify, distribute, sublicense, or deploy this software for commercial or production purposes without a separate written license agreement from the copyright holder.
Allowed without a commercial license:
private review
academic reading
non-commercial evaluation
internal testing with permission
Not allowed without a commercial license:
commercial use
product integration
redistribution
sublicensing
production deployment
offering this software as part of a paid service or hardware product
For licensing inquiries:
kretski1@gmail.com
Commercial Positioning
MicroSafe-RL Core is intended as a runtime safety and observability layer for AI/RL-assisted control systems.
It is designed for teams that need deterministic control bounds, lightweight runtime monitoring, and deployable safety logic on constrained hardware without relying on large neural networks or specialized AI accelerators.
Status
Current version: experimental commercial core.
Recommended next validation steps:
run on STM32 target hardware
measure cycle latency using hardware timers
measure object/state size with compiler output
compare against hard clipping under noisy sensor conditions
log raw command, safe command, penalty, and intervention rate
AI / RL Policy -> MicroSafe-RL -> Hardware / System
