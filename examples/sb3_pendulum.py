"""
Example: MicroSafe + Stable Baselines3
=======================================
Install: pip install stable-baselines3 gymnasium

Two-line integration — everything else stays the same.
"""

import gymnasium as gym
from stable_baselines3 import PPO
from stable_baselines3.common.env_checker import check_env

# ── The two lines that matter ──────────────────────────────────────────
from wrappers.microsafe_gym import MicroSafeWrapper
env = MicroSafeWrapper(gym.make("Pendulum-v1"), sensor_index=0)
# ───────────────────────────────────────────────────────────────────────

# Optional: verify the wrapper is Gymnasium-compliant
check_env(env, warn=True)

# Train exactly as you normally would — nothing else changes
model = PPO("MlpPolicy", env, verbose=1, n_steps=512, batch_size=64)
model.learn(total_timesteps=20_000)

# Evaluate and print safety diagnostics
obs, info = env.reset()
total_reward = 0.0
for _ in range(200):
    action, _ = model.predict(obs, deterministic=True)
    obs, reward, terminated, truncated, info = env.step(action)
    total_reward += reward

    ms = info["microsafe"]
    print(
        f"penalty={ms['penalty']:.3f}  "
        f"gravity={ms['gravity']:.3f}  "
        f"shaped_reward={ms['shaped_reward']:.3f}"
    )
    if terminated or truncated:
        obs, info = env.reset()

print(f"\nTotal reward: {total_reward:.2f}")
env.close()
