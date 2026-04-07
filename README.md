# 🛡️ MicroSafe-RL / ORAC-NT v7.4

![Latency](https://img.shields.io/badge/latency-535ns-blue)
![Memory](https://img.shields.io/badge/RAM-24B-lightgrey)
![Complexity](https://img.shields.io/badge/complexity-O(1)-orange)
![MISRA](https://img.shields.io/badge/MISRA--C-2012-success)
![License](https://img.shields.io/badge/license-Proprietary-red)
![DOI](https://img.shields.io/badge/DOI-10.5281%2Fzenodo.19019599-blue)
![Ollama](https://img.shields.io/badge/Ollama-Compatible-green)
![Gemma](https://img.shields.io/badge/Gemma-2B%2F7B-blue)

**Deterministic Safety Filter for Real-Time Control Systems & Local LLMs**

MicroSafe-RL is a constant-time safety layer placed between AI/control logic and physical actuators.  
It enforces bounded, predictable behavior on every control cycle.

---

## 🚀 Quick Start

```cpp
#include "MicroSafeRL_misra.h"

// kappa, alpha, decay, beta, gravity, min, max, velocity weight
MicroSafeRL safety(0.078f, 0.55f, 2.2f, 0.12f, 1.0f, -1.5f, 1.5f, 0.05f);

void loop() {
    float ai_action = get_ai_command();
    float sensor_val = read_sensor();

    float safe_val = safety.apply_safe_control(ai_action, sensor_val);
    actuator.set(safe_val);
}
No dynamic memory. No async. Runs every control cycle.

🤖 Local LLM Integration (Gemma, Llama 3, Ollama)
MicroSafe-RL acts as a safety filter for local chat bots, preventing hallucinations and unsafe commands before they reach hardware.

Architecture
text
[Local Chat Bot] → [MicroSafe-RL] → [Safe Output]
    (Gemma/Llama)     (535ns filter)
Quick Example with Gemma
python
from gemma_safety_demo import SafeGemmaBridge

bridge = SafeGemmaBridge(model_path="gemma-2b-it")
response = bridge.generate_with_safety("Move arm to position 10")
# Unsafe commands are automatically clamped
Files
File	Purpose
gemma_safety_demo.py	Complete safety wrapper for Gemma
examples/advanced/real_gemma_integration.py	Production-grade integration
Supported Local Bots
Google Gemma (2B/7B)

Meta Llama 3 (via Ollama)

Mistral (via LM Studio)

Any OpenAI-compatible local API

Run the Demo
bash
# Install dependencies
pip install -e .

# Run Gemma safety demo
python gemma_safety_demo.py

# Or with Ollama
curl http://localhost:11434/api/generate -d '{"model": "llama3", "prompt": "safe command"}'
📍 Pipeline Position
text
[ AI / RL / LLM ] → [ MicroSafe-RL ] → [ Actuator ]
📌 Properties
O(1) execution

535 ns latency (STM32 @ 84 MHz, DWT verified)

24 bytes RAM

no dynamic allocation

fixed-point arithmetic

MISRA-C:2012 compatible

🧠 Behavior
At each control step, the system evaluates:

signal instability

deviation from baseline

rate of change

If unsafe behavior is detected, a deterministic constraint is applied.

🛡️ Core Mechanism
text
penalty = κ × (EMA_MAD + α × (1 - coherence) + v_weight × velocity)
gravity = max(0, 1 - penalty × g)
safe_out = clip(ai_action × gravity, min_limit, max_limit)
Hard clipping is always active.

📊 Benchmarks
Metric	Value
Latency	535 ns
Memory	24 bytes
Complexity	O(1)
False Positive Rate	0.05
📊 Comparison
Method	Deterministic	Latency	Adaptivity	Safe by Default
Threshold	✅	✅	❌	⚠️
Kalman Filter	❌	❌	✅	❌
PID Controller	✅	✅	⚠️	❌
MicroSafe-RL	✅	✅	✅	✅
📈 Experimental Results
Figure A — Detection Margin
MicroSafe maintains a significantly higher time-to-failure margin with low variance.

https://media/Figure_A_margin.png

Figure B — ROC Space
MicroSafe dominates the ROC space, achieving higher true positive rate.

https://media/Figure_B_ROC.png

Figure C — Latency Distribution
MicroSafe shows a tight, deterministic latency distribution near 535 ns.

https://media/Figure_C_hist.png

🔬 Failure Modes Covered
signal spikes

drift

oscillations

noisy inputs

invalid AI outputs

LLM hallucinations

⚙️ Design Constraints
no malloc / free

no recursion

constant-time execution

fixed memory footprint

predictable branching

❌ Not Designed For
offline ML

cloud inference

non-physical systems

🧪 Validation
Measured via DWT cycle counter (STM32)

DOI: 10.5281/zenodo.19019599

📁 Project Structure
text
MicroSafe/
├── MicroSafeRL.h              # Core header
├── MicroSafeRL_misra.h        # MISRA-C compliant version
├── gemma_safety_demo.py       # Local LLM integration demo
├── setup.py                   # Python bindings
├── media/                     # Figures and demos
├── examples/                  # Usage examples
├── tests/                     # Unit tests
├── wrappers/                  # Gym/RL wrappers
└── docs/                      # Documentation
🧭 Positioning
Think of it as:
PID controller — but for enforcing safety constraints

📬 Licensing & Contact
All rights reserved.

This software is proprietary.
Use, modification, or distribution without explicit permission is prohibited.

For commercial licensing:
📧 kretski1@gmail.com