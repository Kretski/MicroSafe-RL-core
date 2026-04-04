<p align="center">
  <a href="https://www.producthunt.com/products/github-330?embed=true&amp;utm_source=badge-featured&amp;utm_medium=badge&amp;utm_campaign=badge-github-da53abcb-ebd6-4e2a-9c65-611f6ca15861" target="_blank" rel="noopener noreferrer">
    <img alt="GitHub - Preventing AI Failures in the Physical World in Microseconds | Product Hunt" width="250" height="54" src="https://api.producthunt.com/widgets/embed-image/v1/featured.svg?post_id=1115408&amp;theme=light&amp;t=1775275489205">
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/latency-1.18_µs_WCET-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/RAM-24_bytes-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Logic-Deterministic_O(1)-orange?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-MISRA_C_Compliant-brightgreen?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/DOI-10.5281/zenodo.19019599-blue?style=for-the-badge"/>
</p>

<h1 align="center">🛡️ MicroSafe-RL</h1>
<p align="center">
  <b>Deterministic Sub-Microsecond Safety Layer for Edge AI & Robotics</b><br/>
  <i>High-integrity runtime protection for mission-critical embedded systems.</i>
</p>

---

## 📈 Executive Summary
**MicroSafe-RL** is an ultra-lightweight, bare-metal C++ interceptor that protects physical hardware from Reinforcement Learning (RL) instability and LLM hallucinations (Gemma 4, Llama 3). It acts as a **deterministic safety bridge**, intercepting unsafe commands in under **1.18 µs**.

| Metric | Value |
|---|---|
| **Worst-Case Latency (WCET)** | 1.18 µs (Cortex-M3 @ 72 MHz) |
| **RAM Footprint** | 24 bytes, zero dynamic allocation |
| **Compliance** | MISRA-C:2012 (Zero Critical Violations) |
| **Complexity** | O(1) constant time per step |

---

## 🛡️ Industrial-Grade Safety (MISRA-C:2012)
The core logic has been refactored for **high-integrity environments**. See `MicroSafeRL_misra.h` and the [Compliance Report](MISRA_compliance_report.txt) for details.

* **Rule 15.5 Fixed**: Single exit point architecture for deterministic execution.
* **Rule 12.1 Fixed**: Explicit operator precedence for mathematical reliability.
* **Zero Critical Violations**: Verified via Cppcheck 2.13.0 + MISRA addon.

---

## 🧪 Reliability & Edge Cases
We maintain a rigorous testing suite in `/tests` to ensure safety even when AI models fail.
* **NaN/Inf Protection**: Handled via deterministic clamping.
* **Sensor Surge Recovery**: Capped penalty ensures system stability during noise spikes.
* **Failsafe Initialization**: Guaranteed safe state until sensor telemetry is valid.

---

## 🧠 Core Mechanism
The system tracks **signal entropy** and deviation from a rolling baseline to compute a proactive penalty:

`penalty = κ × (EMA_MAD + α × (1 - coherence) + 0.3 × velocity)`

This penalty dynamically attenuates AI outputs and reshapes RL rewards to prevent hardware wear.

---

## 🗺️ High-Integrity Roadmap (2026)
* **Q2 2026**: 100% MC/DC Test Coverage suite and `static_assert` validation.
* **Q3 2026**: Full Third-Party Audit for **ISO 26262 (ASIL-D)** and **DO-178C** Safety Manuals.
* **Q4 2026**: VHDL/Verilog port for FPGA-based safety (< 100ns latency).

---

## 🔬 Academic Status
**Submitted for Review:** *"A Control Lyapunov Metric for Autonomous Fault Recovery in Embedded and Aerospace Systems"* — **IEEE Transactions on Aerospace and Electronic Systems** (Manuscript ID: TAES-2026-1001).

---

## 📬 Licensing

MicroSafe-RL is released under the **MIT License** for research and prototyping.

For **production deployment** in safety-critical, certified, or regulated environments, commercial licensing and support agreements are available.

**Contact:** [kretski1@gmail.com](mailto:kretski1@gmail.com) (Commercial Licensing & Partnerships)
