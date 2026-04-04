<p align="center"> <img src="https://img.shields.io/badge/latency-~1µs%20(measured)-brightgreen?style=for-the-badge"/> <img src="https://img.shields.io/badge/RAM-24_bytes-brightgreen?style=for-the-badge"/> <img src="https://img.shields.io/badge/Complexity-O(1)-orange?style=for-the-badge"/> <img src="https://img.shields.io/badge/Status-Research%20Prototype-blue?style=for-the-badge"/> <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/> </p> <h1 align="center">🛡️ MicroSafe-RL</h1> <p align="center"> <b>Deterministic Safety Layer for Reinforcement Learning on Edge Hardware</b><br/> <i>Ultra-lightweight runtime protection for embedded AI systems</i> </p>
📈 Overview

MicroSafe-RL is a minimalistic C++ safety module designed to monitor, evaluate, and constrain AI-generated control signals in real time on embedded systems.

It operates as a deterministic safety bridge between:

AI policies (Reinforcement Learning / LLM-based control)
Physical systems (actuators, motors, valves)

The system reduces the risk of unsafe or unstable behavior using constant-time statistical validation and constraint logic.

⚠️ This is a research prototype and NOT a certified safety system.

⚙️ Key Properties
Metric	Value
Latency (measured)	~1 µs (Cortex-M3, DWT counter)
Memory footprint	24 bytes (no dynamic allocation)
Time complexity	O(1) per step
Architecture	Deterministic, statistical

Benchmarks are hardware-dependent and provided for reference only.

🧠 Core Mechanism

MicroSafe-RL computes a stability signature of incoming signals using lightweight statistical features:

short-term dispersion (variance / MAD-like behavior)
deviation from rolling baseline
optional dynamic component (signal velocity)

This produces a penalty term:

penalty = κ × (instability + α × deviation + β × dynamics)

The penalty is then used to:

attenuate unsafe outputs
enforce hard safety bounds
reshape reinforcement learning rewards
🔧 Functional Components
1. Signal Monitoring
Fixed-size buffer (no heap)
Tracks recent system state
2. Stability Estimation
Constant-time statistical evaluation
Noise-tolerant deviation metrics
3. Output Constraint Layer
Soft control (attenuation / scaling)
Hard constraints (clipping / bounding)
4. RL Integration (Optional)
Converts instability into negative reward
Enables safer policy convergence
📊 Example Runtime Output
[ STABLE ] AI: 1.31 | Safe: 1.31 | Reward: 1.00
[ STABLE ] AI: 1.53 | Safe: 1.50 | Clamp applied
[ ALERT  ] AI: 2.10 | Safe: 1.42 | Attenuated

Measured using on-chip cycle counter (DWT).

📂 Repository Structure
.
├── MicroSafeRL.h
├── SafetyBridge.h
├── tools/
│   └── microsafe_profiler.py
├── examples/
│   ├── MicroSafe_Demo.ino
│   └── advanced/
│       └── real_gemma_integration.py
└── wrappers/
    └── microsafe_gym.py
🛠️ Example Usage
#include "MicroSafeRL.h"

MicroSafeRL safety;

void loop() {
    float ai_action = get_ai_output();
    float sensor = read_sensor();

    float safe_action = safety.apply_safe_control(ai_action, sensor);

    actuator.write(safe_action);
}
⚠️ Limitations
Not certified under standards (e.g. DO-178C, ISO 26262)
No formal verification
Not intended as a replacement for industrial safety systems
🎯 Intended Use

This project is designed for:

research
prototyping
experimentation
🔬 Academic Status
Preprint available via Zenodo
Under peer review

No validated safety guarantees are claimed at this stage.

📬 Licensing

MicroSafe-RL is released under the MIT License for research and prototyping use.

For production deployment in safety-critical or regulated environments, commercial licensing and support agreements are available.

📩 Contact:
kretski1@gmail.com

(Commercial Licensing & Partnerships)