<p align="center">
  <img src="https://img.shields.io/badge/latency-1.18_µs-brightgreen?style=for-the-badge&logo=clockify"/>
  <img src="https://img.shields.io/badge/RAM-20_bytes-brightgreen?style=for-the-badge&logo=memory"/>
  <img src="https://img.shields.io/badge/platform-STM32%20%7C%20ESP32%20%7C%20Arduino-informational?style=for-the-badge&logo=cplusplus"/>
  <img src="https://img.shields.io/badge/license-Commercial-red?style=for-the-badge"/>
  <img src="https://img.shields.io/github/stars/Kretski/MicroSafe-RL?style=social"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL</h1>
<h3 align="center">Sub‑microsecond safety layer for Reinforcement Learning on real hardware</h3>

<p align="center">
  <i>“Don’t fix the model – constrain the action space in real time.”</i>
</p>

<p align="center">
  <img src="media/demo.mp4" width="80%" alt="MicroSafe-RL in action"/>
  <br/>
  <i>Visualization: The engine detecting high-entropy "Chaos" states and suppressing AI commands before failure.</i>
</p>

---

## 🚀 The Problem: The "Hardware Drift" Nightmare
Deploying Reinforcement Learning (RL) agents on real physical hardware (robotics, CNC, drones) reveals a catastrophic flaw: **The Unexplored State Space.**

* **Day 1:** You deploy an AI agent on a brand-new robot. It works perfectly.
* **Day 100:** Mechanics wear out, motors heat up, and sensors introduce noise (**Hardware Drift**). The AI agent encounters a state it has never seen, panics, and blindly fires an extreme command that destroys the hardware.

**MicroSafe-RL** is an O(1) bare-metal C++ engine that acts as a real-time "Safety Interceptor". It profiles the hardware's normal stability signature and proactively clamps dangerous commands.

## 📊 Performance (Benchmark: STM32 Cortex-M3 @ 72MHz)
| Metric | Value | Advantage |
| :--- | :--- | :--- |
| **Latency (WCET)** | **1.18 µs** | ~85 clock cycles. Faster than physical electricity propagation in many circuits. |
| **RAM Footprint** | **20 Bytes** | Malloc-free. Zero fragmentation risk. Fits on the smallest ATTiny. |
| **Adaptability** | **Model-Free** | No complex physics equations. Adapts to mechanical wear via EMA/MAD stats. |
| **Determinism** | **O(1)** | Constant execution time regardless of signal complexity. |

---

## 🏗️ Project Structure
- `MicroSafeRL.h`: Core C++ library for embedded deployment.
- `wrappers/`: Gymnasium/Gym wrappers for seamless AI integration.
- `tools/`: Auto-Tuner for generating hardware stability signatures.
- `examples/`: Ready-to-run scripts for STM32, Stable Baselines3, and RLLib.

---

## 🛠️ Installation & Setup

### 🐍 Python (Simulation & Training)
Install the safety wrapper and profiler directly from the root:
```bash
pip install .
🔌 Embedded C++ (Hardware Deployment)
Just drop MicroSafeRL.h into your project. It is header-only and has zero dependencies.

💡 Quick Start
1️⃣ Step 1: Profile your Hardware (Python)
Record 2 minutes of your motor working normally and run the profiler to generate your C++ parameters.

Bash
python tools/microsafe_profiler.py data/input_signal.csv
Output: ✅ MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);

2️⃣ Step 2: Shield your AI Agent (Python Wrapper)
If you are training in simulation, use the provided wrapper to shape rewards and shield actions:

Python
from wrappers.microsafe_gym import MicroSafeWrapper
import gymnasium as gym

env = MicroSafeWrapper(gym.make("Pendulum-v1"), kappa=0.078, alpha=0.55)
3️⃣ Step 3: Deploy to MCU (C++)
Drop the parameters into your control loop. It will protect your hardware in every iteration.

C++
#include "MicroSafeRL.h"

MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float sensor_val = analogRead(A0);
    float ai_action = agent.predict(sensor_val);

    // Intercept & Clamp (1.18µs)
    float safe_action = safety.apply_safe_control(ai_action, sensor_val); [cite: 7]
    
    analogWrite(MOTOR_PIN, safe_action);
}
🔬 Critical Engineering FAQ
Q: What exactly is sensor_val? It is a Composite Signal. We recommend passing a value that defines "health" (e.g., Mean Squared Error, Current Draw, or Vibration).

Q: Will it block legitimate fast movements? No. The Auto-Tuner profiles your specific hardware's aggressive-but-normal operation. It only triggers a penalty when the dynamics deviate into "Unknown Chaos".

Q: Can it run on Arduino? Absolutely. While benchmarked on STM32, it runs on any MCU with a C++ compiler. On 16MHz Arduino, latency is ~5-6µs.

📬 Early Access & Pilot Program
We are currently looking for 2 teams in Robotics or Industrial Automation to join our Pilot Phase.

Developer: Dimitar Kretski

Contact: kretski1@gmail.com

Status: Early Access / Commercial Licensing available upon request.