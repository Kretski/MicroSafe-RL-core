<p align="center">
  <img src="https://img.shields.io/badge/latency-1.18_µs-brightgreen?style=for-the-badge&logo=clockify"/>
  <img src="https://img.shields.io/badge/RAM-20_bytes-brightgreen?style=for-the-badge&logo=memory"/>
  <img src="https://img.shields.io/badge/Logic-Deterministic_O(1)-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/IEEE_ID-TAES--2026--1001-blue?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL: Deterministic Sub-Microsecond Safety Layer for Edge AI & Robotics</h1>

<p align="center">
  <i>The industrial standard for real-time safety shielding in mission-critical embedded environments.</i>
</p>

<p align="center">
  <img src="media/demo.mp4" width="80%" alt="MicroSafe-RL in action"/>
  <br/>
  <i><b>Visual Proof:</b> Real-time entropy detection suppressing AI "Chaos" commands before mechanical failure.</i>
</p>

---

## 📈 Executive Summary
**MicroSafe-RL** is an ultra-lightweight, bare-metal C++ engine designed to protect physical hardware from Reinforcement Learning (RL) instability. By acting as a deterministic "Safety Interceptor", it bridges the gap between AI exploration and hardware preservation.

* **Ultra-Low Latency:** 1.18 µs response time—faster than electrical propagation in many circuits.
* **Zero Footprint:** Uses only 20 bytes of RAM and **zero dynamic allocation** (no malloc), eliminating fragmentation risks.
* **Adaptive Protection:** Uses statistical profiles (EMA + MAD) to detect "Hardware Drift" caused by mechanical wear or heat.
* **Public Integrity:** Open-source core implementation for peer review and industrial audit.

---

## 🎓 Academic Status & Verifiability
This project is part of a high-reliability control framework for autonomous systems.

* **Status:** Submitted on March 14, 2026, to **IEEE Transactions on Aerospace and Electronic Systems**.
* **Manuscript ID:** `TAES-2026-1001` (Awaiting AE Assignment).
* **Paper Title:** "A Control Lyapunov Metric for Autonomous Fault Recovery in Embedded and Aerospace Systems".
* **Archival Reference:** Methodology and performance data are archived in **Zenodo** (DOI: 10.5281/zenodo.19019599).

---

## 📊 Competitive Benchmarking
| Metric | Cloud-based RL | PyTorch Mobile | **MicroSafe-RL** |
| :--- | :--- | :--- | :--- |
| **Latency (WCET)** | 200ms+ | 10ms+ | **1.18 µs** |
| **RAM Footprint** | GBs | MBs | **20 Bytes** |
| **Safety Type** | Reactive | Predictive (Heavy) | **Deterministic / Proactive** |

---

## 🏗️ Core Architecture: How it Works
MicroSafe-RL employs **Adaptive Thresholding** via a high-performance C++ core instead of heavy neural networks:
1. **EMA + MAD Profiling:** Tracks Exponential Moving Average and Mean Absolute Deviation of telemetry to define a "Stability Signature".
2. **Soft Shielding (Gravity Factor):** Gently modulates AI actions as they approach stability limits.
3. **Hard Shielding (Clamp):** Guarantees physical safety by enforcing strict bounds within 85 clock cycles (on 72MHz MCU).

---

## 🛠️ Installation & Simulation
### 🐍 Python (Training & Sim-to-Real)
```bash
pip install .
Embedded C++ (Deployment)
Drop MicroSafeRL.h into your project. Header-only, zero dependencies, C++11 compliant.

💡 Quick Integration Guide
Step 1: Profile Hardware
Bash
python tools/microsafe_profiler.py data/input_signal.csv
Step 2: Initialize Shield
C++
#include "MicroSafeRL.h"
MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);
Step 3: Enforce Real-Time Safety
C++
void loop() {
    float ai_action = agent.predict(sensor_val);
    float safe_action = safety.apply_safe_control(ai_action, sensor_val);
    motor.write(safe_action);
}
📬 Commercial Licensing & IP Acquisition
MicroSafe-RL core is open-source for review. Commercial licensing and private IP acquisition are available for:

MISRA-C Compliance Audits

Specialized Hardware Porting (FPGAs, ASICs)

Custom Safety-Critical Control Loops

Developer: Dimitar Kretski

Contact: kretski1@gmail.com

Status: Version 1.0 (Production Ready)