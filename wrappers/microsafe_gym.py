"""
MicroSafe Gym Wrapper
=====================
Drop-in safety layer for any Gymnasium environment.

Works with:
  - Stable Baselines3
  - Ray RLLib (via gymnasium API)
  - CleanRL
  - Any framework expecting a Gymnasium-compatible env

Quick start (2 lines):
    from wrappers.microsafe_gym import MicroSafeWrapper
    env = MicroSafeWrapper(gym.make("Pendulum-v1"))

That's it. Your agent now has:
  ✓ Soft action shielding (gravity modulation)
  ✓ Hard action clamping
  ✓ Optional reward shaping
  ✓ Per-step safety diagnostics in info dict
"""

from __future__ import annotations

from typing import Any

import numpy as np

try:
    import gymnasium as gym
    from gymnasium import spaces
except ImportError:
    import gym
    from gym import spaces

from .microsafe_profiler import MicroSafeProfiler


class MicroSafeWrapper(gym.Wrapper):
    """
    Safety wrapper that applies MicroSafe shielding to any Gymnasium env.

    Parameters
    ----------
    env : gym.Env
        The environment to wrap.
    sensor_index : int | None
        Which observation dimension to use as the stability sensor.
        None = use the L2-norm of the full observation vector.
    profiler_kwargs : dict
        Keyword arguments forwarded to MicroSafeProfiler.
        See microsafe_profiler.py for full parameter list.

    Example
    -------
    >>> import gymnasium as gym
    >>> from wrappers.microsafe_gym import MicroSafeWrapper
    >>> env = MicroSafeWrapper(gym.make("Pendulum-v1"), sensor_index=0)
    >>> obs, info = env.reset()
    >>> action = env.action_space.sample()
    >>> obs, reward, terminated, truncated, info = env.step(action)
    >>> print(info["microsafe"])   # full diagnostics
    """

    def __init__(
        self,
        env: gym.Env,
        sensor_index: int | None = None,
        **profiler_kwargs: Any,
    ):
        super().__init__(env)
        self.sensor_index = sensor_index
        self.profiler = MicroSafeProfiler(**profiler_kwargs)
        self._last_obs: np.ndarray | None = None

    # ------------------------------------------------------------------
    # Internal helpers
    # ------------------------------------------------------------------

    def _extract_sensor(self, obs: np.ndarray) -> float:
        """Extract a scalar stability signal from the observation."""
        flat = np.asarray(obs, dtype=float).ravel()
        if self.sensor_index is not None:
            return float(flat[self.sensor_index])
        return float(np.linalg.norm(flat))

    def _shield_action(self, action: Any, sensor_val: float) -> Any:
        """
        Apply soft + hard shielding to the action.

        Handles both continuous (Box) and discrete (Discrete) action spaces:
        - Box:      shield every dimension independently
        - Discrete: shield treated as float, then round back to int
        """
        if isinstance(self.action_space, spaces.Box):
            arr = np.asarray(action, dtype=float).ravel()
            shielded = np.array(
                [self.profiler.apply_safe_control(float(a), sensor_val) for a in arr],
                dtype=action.dtype if hasattr(action, "dtype") else float,
            )
            return shielded.reshape(action.shape if hasattr(action, "shape") else (-1,))

        elif isinstance(self.action_space, spaces.Discrete):
            # Treat discrete action as float for penalty computation only
            shielded_float = self.profiler.apply_safe_control(float(action), sensor_val)
            return int(round(shielded_float)) % self.action_space.n

        # Fallback: return action unmodified
        return action

    # ------------------------------------------------------------------
    # Gymnasium API
    # ------------------------------------------------------------------

    def reset(self, **kwargs: Any):
        self.profiler.reset()
        result = self.env.reset(**kwargs)
        # Gymnasium returns (obs, info); older gym returns just obs
        if isinstance(result, tuple):
            obs, info = result
        else:
            obs, info = result, {}
        self._last_obs = np.asarray(obs)
        return obs, info

    def step(self, action: Any):
        # Derive sensor value from last observation
        sensor_val = (
            self._extract_sensor(self._last_obs)
            if self._last_obs is not None
            else 0.0
        )

        # Apply safety shielding to the action
        safe_action = self._shield_action(action, sensor_val)

        # Step the underlying environment
        result = self.env.step(safe_action)
        if len(result) == 5:
            obs, reward, terminated, truncated, info = result
        else:
            obs, reward, done, info = result
            terminated, truncated = done, False

        self._last_obs = np.asarray(obs)
        new_sensor = self._extract_sensor(self._last_obs)

        # Reward shaping
        shaped_reward = self.profiler.shape_reward(float(reward), new_sensor)

        # Attach diagnostics to info dict
        info["microsafe"] = self.profiler.diagnostics
        info["microsafe"]["original_action"] = action
        info["microsafe"]["safe_action"] = safe_action
        info["microsafe"]["original_reward"] = reward
        info["microsafe"]["shaped_reward"] = shaped_reward

        return obs, shaped_reward, terminated, truncated, info
