"""
MicroSafe Profiler — Python port of MicroSafeRL.h
Implements EMA + MAD statistical safety shielding for RL agents.

Mirrors the C++ embedded implementation exactly so that
simulation results transfer to hardware without re-tuning.
"""

import math


class MicroSafeProfiler:
    """
    Statistical safety shield using Exponential Moving Average
    and Mean Absolute Deviation (EMA + MAD).

    Matches the MicroSafeRL.h embedded implementation.

    Parameters
    ----------
    kappa : float
        Penalty scaling factor (default 1.15, mirrors C++ default)
    alpha : float
        Incoherence weight in penalty formula
    decay : float
        EMA decay factor — higher = slower adaptation
    beta : float
        Coherence sharpness factor
    max_penalty : float
        Hard cap on penalty value
    min_limit : float
        Hard lower bound on safe action
    max_limit : float
        Hard upper bound on safe action
    gravity_factor : float
        Soft shielding slope (default 0.05, mirrors C++)
    reward_shaping : bool
        If True, modifies reward using safety signal
    reward_weight : float
        Weight of safety penalty in shaped reward (0.0–1.0)
    """

    def __init__(
        self,
        kappa: float = 1.15,
        alpha: float = 0.55,
        decay: float = 0.95,
        beta: float = 1.5,
        max_penalty: float = 1.0,
        min_limit: float = -1.5,
        max_limit: float = 1.5,
        gravity_factor: float = 0.05,
        reward_shaping: bool = True,
        reward_weight: float = 0.3,
    ):
        self.kappa = kappa
        self.alpha = alpha
        self.decay = decay
        self.beta = beta
        self.max_penalty = max_penalty
        self.min_limit = min_limit
        self.max_limit = max_limit
        self.gravity_factor = gravity_factor
        self.reward_shaping = reward_shaping
        self.reward_weight = reward_weight

        self._ema_mean = 0.0
        self._ema_mad = 0.0
        self._prev_value = 0.0
        self._initialized = False

        # Diagnostics accessible after each step
        self.last_penalty = 0.0
        self.last_gravity = 1.0
        self.last_coherence = 1.0
        self.last_velocity = 0.0

    def apply_safe_control(self, ai_action: float, sensor_val: float) -> float:
        """
        Apply soft + hard shielding to an AI action.

        On the first call, initialises internal state and returns
        ai_action unmodified (mirrors C++ behaviour).

        Parameters
        ----------
        ai_action : float
            Raw action proposed by the RL agent.
        sensor_val : float
            Current scalar observation used for stability analysis.

        Returns
        -------
        float
            Safety-modulated action, clamped to [min_limit, max_limit].
        """
        if not self._initialized:
            self._ema_mean = sensor_val
            self._ema_mad = 0.0
            self._prev_value = sensor_val
            self._initialized = True
            return ai_action

        # 1. EMA + MAD update
        self._ema_mean = self.decay * self._ema_mean + (1.0 - self.decay) * sensor_val
        abs_dev = abs(sensor_val - self._ema_mean)
        self._ema_mad = self.decay * self._ema_mad + (1.0 - self.decay) * abs_dev

        # 2. Rate of change
        velocity = abs(sensor_val - self._prev_value)
        self._prev_value = sensor_val

        # 3. Penalty calculation
        coherence = 1.0 / (1.0 + abs_dev * self.beta)
        raw = self._ema_mad + self.alpha * (1.0 - coherence) + 0.3 * velocity
        penalty = min(self.kappa * raw, self.max_penalty)

        # 4. Soft shielding (gravity modulation)
        gravity = max(0.0, 1.0 - penalty * self.gravity_factor)
        modulated = ai_action * gravity

        # Store diagnostics
        self.last_penalty = penalty
        self.last_gravity = gravity
        self.last_coherence = coherence
        self.last_velocity = velocity

        # 5. Hard shielding
        return float(max(self.min_limit, min(self.max_limit, modulated)))

    def get_current_reward(self, sensor_val: float) -> float:
        """
        Return a safety-shaped reward signal in [0, 1].

        Includes velocity term (unlike the C++ version) for
        consistency with apply_safe_control.
        """
        abs_dev = abs(sensor_val - self._ema_mean)
        coherence = 1.0 / (1.0 + abs_dev * self.beta)
        raw = self._ema_mad + self.alpha * (1.0 - coherence) + 0.3 * self.last_velocity
        penalty = min(self.kappa * raw, self.max_penalty)
        return max(0.0, 1.0 - penalty)

    def shape_reward(self, env_reward: float, sensor_val: float) -> float:
        """
        Blend environment reward with safety signal.

        shaped = (1 - w) * env_reward + w * safety_reward
        """
        if not self.reward_shaping:
            return env_reward
        safety_r = self.get_current_reward(sensor_val)
        return (1.0 - self.reward_weight) * env_reward + self.reward_weight * safety_r

    def reset(self) -> None:
        """Reset internal state at episode boundary."""
        self._ema_mean = 0.0
        self._ema_mad = 0.0
        self._prev_value = 0.0
        self._initialized = False
        self.last_penalty = 0.0
        self.last_gravity = 1.0
        self.last_coherence = 1.0
        self.last_velocity = 0.0

    @property
    def diagnostics(self) -> dict:
        """Return last-step diagnostics as a dict."""
        return {
            "penalty":   self.last_penalty,
            "gravity":   self.last_gravity,
            "coherence": self.last_coherence,
            "velocity":  self.last_velocity,
            "ema_mean":  self._ema_mean,
            "ema_mad":   self._ema_mad,
        }
