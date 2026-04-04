<p align="center">
  <img src="https://img.shields.io/badge/latency-~1µs%20(measured)-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/RAM-24_bytes-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Complexity-O(1)-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-Research%20Prototype-blue?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL</h1>
<p align="center">
  <b>Deterministic Safety Layer for Reinforcement Learning on Edge Hardware</b><br/>
  <i>Lightweight runtime protection for embedded AI systems.</i>
</p>

---

## 📈 Overview

**MicroSafe-RL** is a lightweight C++ module designed to **monitor and constrain AI-generated control signals** in real time on embedded systems.

It acts as an intermediate layer between:
- AI policies (RL / LLM-driven control)
- Physical actuators (motors, valves, etc.)

The goal is to reduce the risk of unstable or unsafe actions using **deterministic, low-latency logic**.

> ⚠️ This is a research prototype and not a certified safety system.

---

## ⚙️ Key Properties

| Metric | Value |
|---|---|
| **Latency (measured)** | ~1 µs (Cortex-M3, DWT counter) |
| **Memory** | 24 bytes, no dynamic allocation |
| **Complexity** | O(1) per step |
| **Design** | Deterministic, statistical |

> Measurements are hardware-specific and provided for reference only.

---

## 🧠 Core Idea

The system evaluates a **stability signature** of incoming signals using:

- short-term statistical dispersion (e.g. deviation / MAD-like behavior)
- distance from a rolling baseline
- optional dynamic terms (velocity)

This produces a penalty signal:


penalty = κ × (instability + α × deviation + β × dynamics)


which can be used to:

- attenuate unsafe actions
- clamp outputs to safe bounds
- shape reinforcement learning rewards

---

## 🔧 Functional Components

### 1. Signal Monitoring
Tracks recent sensor values using a fixed-size buffer.

### 2. Stability Estimation
Computes a lightweight instability metric (variance / deviation-based).

### 3. Output Constraint
Applies:
- soft attenuation (scaling)
- hard bounds (clipping)

### 4. Optional RL Integration
Transforms instability into a negative reward signal.

---

## 📊 Example Output (STM32 Demo)


[ STABLE ] AI: 1.31 | Safe: 1.31 | Reward: 1.00
[ STABLE ] AI: 1.53 | Safe: 1.50 | Clamp applied
[ ALERT ] AI: 2.10 | Safe: 1.42 | Attenuated


Measured using on-chip cycle counter (DWT).

---

## 🔬 Academic Status

- Preprint available via Zenodo: DOI 10.5281/zenodo.19019599  
- Submitted for peer review (under evaluation)

> No claims of validated safety guarantees are made at this stage.

---

## 📂 Repository Structure


.
├── MicroSafeRL.h
├── SafetyBridge.h
├── tools/
│ └── microsafe_profiler.py
├── examples/
│ ├── MicroSafe_Demo.ino
│ └── advanced/
│ └── real_gemma_integration.py
└── wrappers/
└── microsafe_gym.py


---

## 🛠️ Example Usage

```cpp
#include "MicroSafeRL.h"

MicroSafeRL safety;

void loop() {
    float ai_action = get_ai_output();
    float sensor = read_sensor();

    float safe_action = safety.apply_safe_control(ai_action, sensor);

    actuator.write(safe_action);
}
⚠️ Limitations
Not certified under standards such as DO-178C or ISO 26262
Not formally verified
Not a replacement for industrial safety systems

This project is intended for:

research
prototyping
experimentation
📬 Licensing

MicroSafe-RL is released under the MIT License for research and prototyping.

For production use in safety-critical or regulated environments, licensing and support agreements are available.

Contact: kretski1@gmail.com
 (Commercial Licensing & Partnerships)