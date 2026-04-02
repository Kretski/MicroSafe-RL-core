🛡️ MicroSafe-RL
  Sub‑microsecond adaptive safety layer for Edge AI
  ────────────────────────────────────────────────────────────────────────────
-->

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
  <img src="docs/demo.gif" width="80%" alt="MicroSafe-RL in action – showing penalty spike on sudden drift"/>
  <br/>
  <sub>🎥 <em>Replace with your own GIF: STM32 + potentiometer + LED</em></sub>
</p>

---

## 🚨 The Hardware Drift Nightmare

> **Day 1** – Perfect AI, brand‑new robot, everything works.  
> **Day 100** – Mechanics wear, sensors drift, old limits fail.  
> **AI starts guessing → hardware breaks.**

**MicroSafe-RL stops the nightmare.**  
It learns *your* hardware’s normal behaviour and **adapts in real time** – no model, no retraining, no manual tweaking.

---

## ⚡ Why MicroSafe-RL?

| Feature | What it means for you |
|---------|----------------------|
| **⚡ Sub‑µs latency** | Reacts faster than any software watchdog (1.18 µs @72 MHz) |
| **🧠 Model‑free** | No physics equations – just sensor data |
| **🔄 Self‑adapting** | Tracks hardware drift, wear, and changing dynamics |
| **🪶 20 bytes RAM** | Runs on the cheapest MCU |
| **🔒 Zero malloc** | Deterministic, no heap fragmentation |
| **🔧 Drop‑in C++ header** | Integrate in 10 minutes |

---

## 🧠 How It Works (Simplified)

1. **Learn the “normal”** – EMA + MAD + velocity create a *stability signature*.
2. **Detect anomalies** – deviation from normal → penalty score.
3. **Apply two‑layer safety**  
   - 🟢 *Soft shielding*: scale down action (gravity factor)  
   - 🔴 *Hard shielding*: clamp to absolute safe limits  
4. **AI learns** – penalty is subtracted from the reward stream. The agent *avoids* risky zones before reaching them.

<p align="center">
  <img src="docs/block_diagram.png" width="70%" alt="MicroSafe-RL block diagram"/>
  <br/>
  <sub>📐 Block diagram – coming soon</sub>
</p>

---

## 🚀 Quick Start (2 minutes)

### 1️⃣ Record normal operation

Run the auto‑tuner while your system works correctly (no AI, just telemetry):

```bash
python microsafe_profiler.py normal_data.csv
It outputs ready‑to‑use parameters:

cpp
MicroSafeRL safety(0.078f, 0.55f, 0.95f, 0.84f, 1.0f, -1.5f, 1.5f, 0.05f);
2️⃣ Integrate the header
cpp
#include "MicroSafeRL.h"

MicroSafeRL safety(kappa, alpha, decay, beta, max_penalty,
                   min_action, max_action, gravity_g);

void loop() {
    float safe_action = safety.apply_safe_control(ai_action, sensor_val);
    float reward = safety.get_current_reward(sensor_val);
    // use safe_action and reward as you like
}
3️⃣ Build and run – safety works out of the box
📊 Performance (STM32F103 @72 MHz)
Metric	Value
Latency (WCET)	1.18 µs (85 cycles)
RAM	20 bytes
Flash	<300 bytes
Allocation	0 (no malloc)
Deterministic	Yes (O(1))
Faster than any software‑based hard limit you have ever used.

🔬 What It Prevents
❌ Runaway actuator commands

❌ Destructive oscillations

❌ Current spikes

❌ Out‑of‑distribution control (when the AI “guesses”)

🧩 Use Cases
🤖 Robotics – servo/motor protection under wear & tear

🚁 Drones – prevent over‑current during aggressive manoeuvres

🏭 Industrial automation – adaptive safety for ageing machinery

⚛️ Quantum control – keep cryogenic stability without retraining

🧪 Embedded AI research – safe RL on real hardware

📬 Early Access / Pilot Program
We are looking for 2 teams working on:

robotics

drones

industrial systems

👉 If your AI has ever damaged hardware – we want to talk.

✉️ Contact: [your‑email@example.com]

👨‍💻 Author
Dimitar Kretski
Early Access / Pilot Phase

⚖️ Intellectual Property & License
MicroSafe-RL is proprietary and patent‑pending.
Public release does not waive any intellectual property rights.
Commercial licenses are available – contact the author.

⭐ Show your support
If this project helps you or you believe in safe Edge AI, give it a star ⭐ – it shows us there is real demand.