"""
MicroSafe Profiler - Python counterpart of MicroSafeRL.h
Exact mirror of the C++ implementation for sim-to-real consistency.
Updated: 2026-04-04 (Tuned for Hardware Profile MAD 0.58 / V 0.46)
"""

import math
from typing import Optional, Dict


class MicroSafeProfiler:
    """
    Python implementation of MicroSafeRL v3.1
    Matches the C++ version exactly for consistent behavior between simulation and hardware.
    """

    def __init__(
        self,
        kappa: float = 0.088,          # Tuned: High sensitivity for 0.58 MAD
        alpha: float = 0.55,           # Optimized coherence factor
        beta: float = 0.85,            # Tuned: Stability decay for 0.46 Velocity
        lambda_: float = 0.95,         # EMA smoothing factor (Decay)
        max_penalty: float = 1.0,      # Normalized max penalty
        min_limit: float = -1.5,       # Hard limit (Min)
        max_limit: float = 1.5,        # Hard limit (Max)
        gravity_factor: float = 1.0,   # Soft shielding strength (matches g_factor)
        velocity_gain: float = 0.05    # Weight for physical velocity in penalty
    ):
        self.kappa = kappa
        self.alpha = alpha
        self.beta = beta
        self.lambda_ = lambda_          
        self.max_penalty = max_penalty
        self.min_limit = min_limit
        self.max_limit = max_limit
        self.gravity_factor = gravity_factor
        self.velocity_gain = velocity_gain

        # Internal state
        self._ema_mean = 0.0
        self._ema_mad = 0.0
        self._prev_value = 0.0
        self._current_penalty = 0.0
        self._initialized = False

        # Diagnostics
        self.last_penalty = 0.0
        self.last_gravity = 1.0
        self.last_coherence = 1.0
        self.last_velocity = 0.0

    def init(self, initial_sensor: float = 0.0) -> None:
        """Initialize internal state (mirrors C++ init)"""
        self._ema_mean = initial_sensor
        self._ema_mad = 0.0
        self._prev_value = initial_sensor
        self._current_penalty = 0.0
        self._initialized = True

    def apply_safe_control(self, ai_action: float, sensor_val: float) -> float:
        """
        Main safety function - applies soft + hard shielding.
        Exact match to C++ apply_safe_control().
        """
        if not self._initialized:
            self.init(sensor_val)
            # First call - only hard shielding
            return max(self.min_limit, min(self.max_limit, ai_action))

        # 1. EMA + MAD update (Deterministic history tracking)
        self._ema_mean = self.lambda_ * self._ema_mean + (1.0 - self.lambda_) * sensor_val
        abs_dev = abs(sensor_val - self._ema_mean)
        self._ema_mad = self.lambda_ * self._ema_mad + (1.0 - self.lambda_) * abs_dev

        # 2. Velocity calculation
        velocity = abs(sensor_val - self._prev_value)
        self._prev_value = sensor_val

        # 3. Penalty calculation (The core stability metric)
        coherence = 1.0 / (1.0 + abs_dev * self.beta)
        raw = self._ema_mad + self.alpha * (1.0 - coherence) + self.velocity_gain * velocity
        self._current_penalty = self.kappa * raw
        
        if self._current_penalty > self.max_penalty:
            self._current_penalty = self.max_penalty

        # 4. Soft shielding (Dynamic Gravity)
        gravity = 1.0 - (self._current_penalty * self.gravity_factor)
        gravity = max(0.0, gravity)
        modulated = ai_action * gravity

        # 5. Hard shielding (Safety Clamp)
        safe_action = max(self.min_limit, min(self.max_limit, modulated))

        # Update diagnostics
        self.last_penalty = self._current_penalty
        self.last_gravity = gravity
        self.last_coherence = coherence
        self.last_velocity = velocity

        return safe_action

    def get_penalty(self) -> float:
        """Return current penalty (mirrors C++ get_penalty)"""
        return self._current_penalty

    def get_current_reward(self) -> float:
        """Return shaped reward = 1.0 - penalty"""
        return 1.0 - self._current_penalty

    def shape_reward(self, env_reward: float, sensor: float) -> float:
        """
        Blend environment reward with safety penalty (default weight 0.3).
        Accepts 'sensor' argument to match microsafe_gym wrapper signature.
        """
        # Бележка: Добавен е параметърът `sensor`, за да не гърми кодът при 
        # подаване на 2 аргумента от Gym wrapper-а. В случай че логиката 
        # изисква преизчисляване на penalty на база на този сензор ПРЕДИ
        # връщането на наградата, може да се добави тук. Засега ползваме текущия стейт.
        safety_r = self.get_current_reward()
        return (0.7 * env_reward) + (0.3 * safety_r)

    def reset(self) -> None:
        """Reset state for new episode"""
        self._ema_mean = 0.0
        self._ema_mad = 0.0
        self._prev_value = 0.0
        self._current_penalty = 0.0
        self._initialized = False

    @property
    def diagnostics(self) -> Dict[str, float]:
        """Return last step diagnostics"""
        return {
            "penalty": self.last_penalty,
            "gravity": self.last_gravity,
            "coherence": self.last_coherence,
            "velocity": self.last_velocity,
            "ema_mad": self._ema_mad
        }


# ========================== Sim-to-Real Validation ==========================
if __name__ == "__main__":
    # Initialize with tuned parameters
    safety = MicroSafeProfiler()
    
    print(f"🛡️ Validation started with parameters: kappa={safety.kappa}, lambda={safety.lambda_}")
    
    # Simulate a high-drift scenario
    for i in range(5):
        sensor = 0.5 + (i * 0.2)           # Rapid drift
        action = 1.4                       # High AI command
        
        safe_action = safety.apply_safe_control(action, sensor)
        print(f"Step {i} | Sensor: {sensor:.2f} | AI: {action:.2f} -> Safe: {safe_action:.3f} | Penalty: {safety.get_penalty():.3f}")