"""
Example: MicroSafe + Ray RLLib
================================
Install: pip install "ray[rllib]" gymnasium torch

Two-line integration via RLLib's env wrapping.
"""

import gymnasium as gym
import ray
from ray.rllib.algorithms.ppo import PPOConfig

from wrappers.microsafe_gym import MicroSafeWrapper


def make_safe_env(config):
    """Factory function for RLLib — wraps any env with MicroSafe."""
    base_env = gym.make("Pendulum-v1")
    # ── The integration ────────────────────────────────────────────────
    return MicroSafeWrapper(base_env, sensor_index=0, reward_weight=0.2)
    # ───────────────────────────────────────────────────────────────────


ray.init(ignore_reinit_error=True)

algo = (
    PPOConfig()
    .environment(env=make_safe_env)
    .rollouts(num_rollout_workers=2)
    .training(lr=3e-4, gamma=0.99)
    .build()
)

for i in range(5):
    result = algo.train()
    print(
        f"Iter {i+1}: "
        f"reward_mean={result['episode_reward_mean']:.2f}  "
        f"episodes={result['episodes_this_iter']}"
    )

ray.shutdown()
