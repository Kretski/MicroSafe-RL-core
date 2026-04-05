# 🛡️ MicroSafe-RL

**Deterministic Sub-Microsecond Safety Layer for Edge AI & Robotics**

MicroSafe-RL is an ultra-lightweight, bare-metal C++ interceptor designed to protect physical hardware from Reinforcement Learning (RL) instability and Edge LLMs "hallucinations".

## 📈 Executive Summary

RL policies trained in simulation often produce out-of-range commands when deployed on real hardware. This library "clamps" these commands **deterministically** in **O(1)** time before they reach the execution engine.

| Metric                        | Value                                          |
|-------------------------------|------------------------------------------------|
| **Worst-Case Latency (WCET)** | 1.18 µs (Cortex-M3 @ 72 MHz)                   |
| **RAM Footprint**             | 24 bytes, zero dynamic allocation              |
| **Compliance**                | MISRA-C:2012 (Zero Critical Violations)        |
| **Complexity**                | O(1) constant time per step                    |

## 🎬 Demonstration Video

See MicroSafe-RL in action, preventing critical failure in real time:

[▶ Watch Demo](media/demo.mp4)

## 🛡️ How It Works

The interceptor monitors the signal behavior using an **Exponential Moving Average (EMA)**. When the input command deviates significantly from the established baseline, a "gravity factor" is applied to smoothly return the command to the safe operating zone.

$$
penalty = \kappa \times (EMA\_MAD + \alpha \times (1 - coherence) + 0.3 \times velocity)
$$

$$
gravity = \max(0, 1 - penalty \times g)
$$

$$
safe\_out = clip(ai\_action \times gravity, min\_limit, max\_limit)
$$

$$
reward = 1 - penalty
$$

**Hard Shield**: The hard clip (`min_limit` / `max_limit`) is always active from the very first step, ensuring safety even before the EMA has collected sufficient history.
## 🔗 Integration with Local AI Bots / Edge LLMs

MicroSafe-RL now includes a **Python-C++ Bridge** that enables seamless and safe connection with local Large Language Models.

### Supported Use Cases
- Using **Ollama** (Gemma 4, Llama 3.2, Phi-4, Mistral, etc.) as a robotic controller
- Running local vision-language models for perception + reasoning
- Edge LLM agents that output high-level actions (velocity, torque, joint targets)

The bridge forwards the LLM-generated action to MicroSafe-RL, which applies deterministic safety clamping before the command reaches the hardware. This combination gives you the flexibility and intelligence of a local LLM while maintaining sub-microsecond physical safety guarantees.

### Example Bridge Usage (Python)

```python
import ollama
from microsafe_bridge import MicroSafeBridge

safety_bridge = MicroSafeBridge(
    model_name="gemma:2b",          # or any local model via Ollama
    safety_params={"kappa": 0.078, "alpha": 0.55, ...}
)

while True:
    prompt = get_current_observation()   # sensor data, camera, etc.
    response = ollama.chat(model="gemma:2b", messages=[{"role": "user", "content": prompt}])
    
    try:
        ai_action = parse_action(response['message']['content'])   # extract float/vector
        safe_action = safety_bridge.apply(ai_action)               # MicroSafe-RL protection
        actuator_command(safe_action)
    except:
        # fallback to safe default
        actuator_command(0.0)
## 📊 Benchmark Results

Tested against a Kalman-filter detector and a PLC threshold system across 120 scenarios (runaway, adversarial, and safe conditions).

| System              | Mean Detection Margin (steps before failure) | Std Dev |
|---------------------|----------------------------------------------|---------|
| **MicroSafe-RL**    | 19.2                                         | ±1.4    |
| Kalman-filter       | 11.0                                         | ±1.6    |
| PLC threshold       | 8.0                                          | ±0.7    |

**Hardware Validation** (Arduino Uno + MPU-6050):  
Average end-to-end intercept latency — 3.2 ms, with **1.18 µs** pure computational latency of the protection algorithm.

## 🧪 Integration Example (Embedded C++)

```cpp
#include "MicroSafeRL_misra.h"

// Parameters: kappa, alpha, decay, beta, g, min, max, vel_weight
MicroSafeRL safety(0.078f, 0.55f, 2.2f, 0.12f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float safe_val = safety.apply_safe_control(ai_action, sensor_val);
    float reward   = safety.get_current_reward();

    actuator.set(safe_val);
    agent.update(reward);
}
⚠️ Limitations

Warm-up Period: EMA requires approximately 20 calibration steps. During this time, only the hard clip is active.
Hardware Protection: This is a software safety layer and does not replace physical protections such as fuses and current limiters.
Tuning: Parameters (κ, α, g, etc.) significantly affect behavior and should be profiled for the specific hardware.

🔬 Academic Status
Submitted for Review:
"A Control Lyapunov Metric for Autonomous Fault Recovery in Embedded and Aerospace Systems" — IEEE Transactions on Aerospace and Electronic Systems.
Preprint:
Kretski, D. (2026). ORAC-NT v5.x: Optimal and Stable FDIR Architecture. Zenodo.
DOI: 10.5281/zenodo.19019599
📬 Licensing

MIT License for academic and non-commercial use.
For production deployment in safety-critical systems, please contact: kretski1@gmail.com


Ready to copy and paste.
Let me know if you want any changes (shorter version, badges, different tone, etc.)!