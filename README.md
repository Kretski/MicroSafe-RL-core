<p align="center">
  <a href="https://www.producthunt.com/products/github-330?embed=true&amp;utm_source=badge-featured&amp;utm_medium=badge&amp;utm_campaign=badge-github-da53abcb-ebd6-4e2a-9c65-611f6ca15861" target="_blank" rel="noopener noreferrer">
    <img alt="GitHub - Preventing AI Failures in the Physical World in Microseconds | Product Hunt" width="250" height="54" src="https://api.producthunt.com/widgets/embed-image/v1/featured.svg?post_id=1115408&amp;theme=light&amp;t=1775275489205">
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/latency-1.18_µs_WCET-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/RAM-24_bytes-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Logic-Deterministic_O(1)-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-IEEE_TAES_Under_Review-blue?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL</h1>
<p align="center">
  <b>Deterministic Sub-Microsecond Safety Layer for Edge AI & Robotics</b><br/>
  <i>Designed for industrial-grade embedded safety in mission-critical environments.</i>
</p>

---

## 📈 Executive Summary
**MicroSafe-RL** is an ultra-lightweight, bare-metal C++ interceptor that protects physical hardware from Reinforcement Learning (RL) instability and LLM hallucinations. It acts as a **deterministic safety shield** between AI-generated actions (Gemma 4, Llama 3, etc.) and electronic actuators — intercepting unsafe commands in under **1.18 µs**.

| Metric | Value |
|---|---|
| **Worst-Case Latency (WCET)** | 1.18 µs on Cortex-M3 @ 72 MHz |
| **RAM Footprint** | 24 bytes, zero dynamic allocation |
| **Algorithm Complexity** | $O(1)$ per step |
| **Safety Type** | Deterministic / Proactive (EMA + MAD) |

---

## 🧠 How It Works — Three Pillars

### 1. Proactive Guidance (Dynamic Gravity)
The system tracks **signal entropy** in real-time. When it detects abnormal vibration or sensor drift, it applies a "gravity" factor — softly pulling AI actions back toward the safe zone.

$$penalty = \kappa \times [EMA\_MAD + \alpha \times (1 - coherence) + 0.3 \times velocity]$$
$$gravity = 1 - penalty \times g\_factor$$
$$safe\_out = clip(ai\_action \times gravity, min, max)$$

### 2. Last-Microsecond Shielding (Hard Clamp)
A mathematically guaranteed hard clamp blocks any action exceeding physical limits. Implemented via bare-metal registers — **no OS, no RTOS latency**.

### 3. Hardware Risk Translator (Reward Shaping)
RL agents understand only rewards. MicroSafe-RL translates physical wear and drift into a negative reward signal, teaching the agent to be **safe-by-design**.
$$reward = 1.0 - penalty$$

---

## 📊 Real-World Evidence — Hardware Log (STM32)
```text
[ STABLE  ] AI: 1.31 | Safe: 1.31 | Reward: 1.00 | Latency: 1.18 µs
[ STABLE  ] AI: 1.53 | Safe: 1.50 | Reward: 1.00 | SHIELD ACTIVE ← Clamp
[! CHAOS !] AI: 2.10 | Safe: 1.42 | Reward: 0.36 | INTERCEPTED ← Hallucination/Drift
Measured with DWT hardware cycle counter (direct clock register access).

🔬 Academic Status & Verifiability
Submitted to peer review: "A Control Lyapunov Metric for Autonomous Fault Recovery in Embedded and Aerospace Systems" IEEE Transactions on Aerospace and Electronic Systems — ID: TAES-2026-1001.

Published Preprint (Zenodo): DOI: 10.5281/zenodo.19019599.

📂 Repository Structure
Plaintext
.
├── MicroSafeRL.h        # Production Core: O(1) Deterministic Shield
├── SafetyBridge.h       # C++ Bridge: Maps AI actions to physical limits
├── tools/
│   └── microsafe_profiler.py # Auto-tuner: CSV → C++ Constants
├── examples/
│   ├── MicroSafe_Demo.ino    # Arduino/ESP32 Benchmark
│   └── advanced/
│       └── real_gemma_integration.py # Ollama + Gemma 4 Safe Bridge
└── wrappers/
    └── microsafe_gym.py      # Gymnasium wrapper for Safe RL Training
🛠️ Quick Start (The Safety Bridge)
Integrate Gemma 4 (Ollama) or other Edge LLMs with real-time hardware protection:

C++
#include "MicroSafeRL.h"

// Initialize with hardware signature (Tuned for MAD 0.58)
MicroSafeRL safety(0.088f, 0.55f, 0.95f, 0.85f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float ai_action = get_command_from_ollama(); 
    float sensor_val = read_hardware_telemetry();

    // Intercept and stabilize in 1.18 microseconds
    float safe_action = safety.apply_safe_control(ai_action, sensor_val);
    
    motor.write(safe_action);
}
🐍 Python — Auto-Tuner & Training
Profile your hardware to generate optimal constants for the C++ core:

Bash
# Profile from real telemetry CSV
python tools/microsafe_profiler.py data/input_signal.csv

# Optimal parameters found: kappa=0.088, beta=0.85...
📬 Licensing
MicroSafe-RL is released under the MIT License for research and prototyping.

For production use in safety-critical, certified, or commercial environments (e.g., aerospace, automotive, industrial control), licensing and support agreements are available.

Contact: kretski1@gmail.com (Commercial Licensing & Partnerships)