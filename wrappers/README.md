# MicroSafe-RL — Gym Wrappers

Drop-in safety layer for any Gymnasium-compatible environment.
Implements the same **EMA + MAD** shielding algorithm as the embedded
`MicroSafeRL.h` C++ header — so simulation results transfer to hardware
without re-tuning parameters.

---

## Installation

```bash
pip install gymnasium numpy
# for examples:
pip install stable-baselines3 torch
```

No additional dependencies. The wrapper has zero non-standard imports.

---

## 2-Line Integration

```python
from wrappers.microsafe_gym import MicroSafeWrapper
env = MicroSafeWrapper(gym.make("Pendulum-v1"))
```

Pass this `env` to **Stable Baselines3**, **Ray RLLib**, **CleanRL**,
or any framework that accepts a Gymnasium environment. Nothing else changes.

---

## How It Works

Every `env.step(action)` runs three safety stages:

```
AI action
    │
    ▼
[1] Soft Shielding        gravity = 1 − penalty × factor
    │                     modulated = action × gravity
    ▼
[2] Hard Shielding        clamp to [min_limit, max_limit]
    │
    ▼
safe_action → environment
    │
    ▼
[3] Reward Shaping        shaped = (1−w)×env_reward + w×safety_reward
```

The penalty is derived from the observation's statistical stability:

```
penalty = κ × (EMA_MAD + α × (1 − coherence) + 0.3 × velocity)
```

This matches the `MicroSafeRL.h` formula exactly.

---

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `kappa` | 1.15 | Penalty scale (matches C++ default) |
| `alpha` | 0.55 | Incoherence weight |
| `decay` | 0.95 | EMA decay — higher = slower adaptation |
| `beta` | 1.5 | Coherence sharpness |
| `max_penalty` | 1.0 | Hard cap on penalty |
| `min_limit` | -1.5 | Hard lower bound on action |
| `max_limit` | 1.5 | Hard upper bound on action |
| `gravity_factor` | 0.05 | Soft shielding slope |
| `reward_shaping` | True | Enable reward modification |
| `reward_weight` | 0.3 | Safety weight in shaped reward |
| `sensor_index` | None | Obs dimension for stability signal (None = L2 norm) |

---

## Per-Step Diagnostics

Every `info` dict contains a `"microsafe"` key:

```python
obs, reward, terminated, truncated, info = env.step(action)
ms = info["microsafe"]

ms["penalty"]          # current safety penalty [0, max_penalty]
ms["gravity"]          # soft shielding multiplier [0, 1]
ms["coherence"]        # statistical coherence [0, 1]
ms["velocity"]         # rate-of-change of sensor signal
ms["ema_mean"]         # exponential moving average
ms["ema_mad"]          # EMA mean absolute deviation
ms["original_action"]  # raw AI action before shielding
ms["safe_action"]      # shielded action sent to env
ms["original_reward"]  # env reward before shaping
ms["shaped_reward"]    # final reward received by agent
```

---

## Examples

| File | Framework |
|------|-----------|
| `examples/sb3_pendulum.py` | Stable Baselines3 + PPO |
| `examples/rllib_pendulum.py` | Ray RLLib + PPO |

---

## Relationship to Hardware

The Python `MicroSafeProfiler` class is a direct port of `MicroSafeRL.h`.
All default parameters are identical. This means:

1. Train in simulation with the wrapper
2. Flash `MicroSafeRL.h` to STM32 / Arduino with the same parameters
3. Behaviour matches — no hardware re-tuning required

---

## Citation

If you use MicroSafe-RL in research, please cite the ORAC-NT preprint:

```
Kretski, D. (2025). ORAC-NT: Deterministic Vitality Control for Embedded
Thermal Management. Zenodo. https://doi.org/10.5281/zenodo.19019599
```
