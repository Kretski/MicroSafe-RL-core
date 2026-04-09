# MicroSafe-RL — Runtime Safety Layer for LLM Control

MicroSafe-RL is a lightweight, real-time safety layer designed for **LLM-driven control systems** (robotics, embedded AI, edge devices).

It ensures that every action proposed by an AI model is **validated, corrected, and quantified** before execution.

---

## 🚀 Positioning

Not a "plugin".

A **runtime safety layer for LLM-driven control systems with built-in validation and metrics.**
safety_score = 1 - penalty

→ Provides **quantifiable safety per cycle**

Patented safety architecture ("Synergy") enabling deterministic, model-independent control at runtime.
---

## ⚙️ Core Concept

The system sits between:
LLM / RL Agent → MicroSafe → Hardware / System

Each control cycle:

1. AI proposes action (`raw_action`)
2. Safety layer evaluates constraints
3. Unsafe actions are clipped or corrected (`safe_action`)
4. Penalty is computed
5. Safety score is generated
6. Final safe command is executed


## 🔍 Built-in Validation & Verification (V&V)

MicroSafe-RL includes continuous runtime validation and verification:

- **Input Validation**  
  Detects NaN, Inf, and malformed AI outputs.

- **Output Verification**  
  Enforces strict physical bounds via deterministic clipping.

- **Runtime Monitoring**  
  Tracks penalty, risk, and intervention events per control cycle.

- **Behavioral Verification**  
  Explicitly flags unsafe AI actions (`INTERCEPTED` state).

Unlike traditional V&V systems, MicroSafe-RL performs verification **in real time, inside the control loop**.


## 📊 Runtime Telemetry (VV System)

### 🔹 VVRecord (per-cycle logging)

Each loop records:

- `raw_action`
- `safe_action`
- `penalty`
- `gravity`
- `clipped` (bool)
- `safety_score`
- `latency`
- `status`

This enables **full traceability of AI behavior under constraints**.

---

### 🔹 VVReport (session aggregation)

At the end of a session, the system computes:

- `clip_rate` → how often AI violated constraints
- `penalty_mean`
- `penalty_max`
- `safety_score_mean`
- `pass/fail` vs threshold (default: `0.5`)
- `hardware_latency` (~535 ns observed)

---

### 🔹 Automatic Report Generation
generate_report()

Triggered by:

- Command: `report`
- OR automatic on `Ctrl+C`

Outputs:
vv_report_VV-YYYYMMDD-HHMMSS.json

---

## 🧠 Why This Matters

Typical AI control systems:

❌ No guarantees  
❌ No runtime validation  
❌ No safety metrics  

MicroSafe-RL provides:

✅ Deterministic safety layer  
✅ Real-time constraint enforcement  
✅ Measurable safety (not heuristic)  
✅ Hardware-level latency  

---

## 📁 Project Structure

.
├── data/
│ └── demo_log.csv
│
├── examples/
│ └── basic_demo/
│ └── basic_demo.ino
│
├── include/
│ ├── MicroSafeController.h
│ ├── MicroSafeRL.h
│ └── MicroSafeRL_CBF.h
│
├── MicroSafeRL_CBF_Demo/
│ ├── MicroSafeRL_CBF_Demo.ino
│ └── Readme.md
│
├── python/
│ └── microsafe_llm_plugin.py
│
├── tests/
│ └── test_basic.cpp


---

## 🔧 Example Output (Serial)

AI,SAFE,SCORE
0.82,0.65,0.91
-1.20,-0.80,0.60
0.30,0.30,1.00


---

## 🧪 Example Use Cases

- LLM-controlled robotics
- Autonomous systems
- Embedded AI (MCU / Edge)
- Safety-critical control loops
- Human-in-the-loop AI systems

---

## 🧩 Key Features

- Constraint-based safety (CBF-style)
- Real-time clipping & correction
- Penalty-driven evaluation
- Deterministic execution
- Ultra-low latency (~ns scale)
- Full session analytics (VVReport)

---

## 📈 Safety Metric
safety_score = 1 - penalty


| Penalty | Safety Score | Meaning        |
|--------|-------------|----------------|
| 0.0    | 1.0         | Fully safe     |
| 0.5    | 0.5         | Borderline     |
| 1.0    | 0.0         | Unsafe         |

---

## ▶️ Getting Started

1. Open `basic_demo.ino` in Arduino IDE
2. Upload to board
3. Open Serial Monitor (115200 baud)
4. Observe:

- AI vs Safe actions
- Safety score per cycle

---

## 📌 Future Extensions

- Adaptive constraints (learned boundaries)
- Multi-dimensional safety fields
- Integration with ROS / robotics stacks
- Hardware-in-the-loop validation
- Distributed safety monitoring

---


## 📄 License

Proprietary License – Patent Pending

This software contains technology subject to a pending patent application.

- Allowed: research and evaluation use only
- Not allowed: commercial use, redistribution, or modification without permission

For licensing inquiries: kretski1@gmail.com

---

## 🤝 Contribution

Pull requests welcome.

Focus areas:

- Safety models
- Latency optimization
- Real-world validation scenarios

