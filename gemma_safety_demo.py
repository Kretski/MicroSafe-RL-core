"""
gemma_safety_demo.py
Gemma 4 + MicroSafe-RL Safety Bridge - Clean Demo
"""

import sys
from pathlib import Path
import random

# Fix import
sys.path.append(str(Path(__file__).parent))
from microsafe_profiler import MicroSafeProfiler


class SafetyBridge:
    def __init__(self):
        self.safety = MicroSafeProfiler(
            kappa=1.15, alpha=0.55, beta=2.2, lambda_=0.12,
            max_penalty=1.0, min_limit=-1.5, max_limit=1.5, gravity_factor=0.05
        )
        self.safety.init(0.5)

    def process(self, raw_command: float, sensor_value: float):
        safe_action = self.safety.apply_safe_control(raw_command, sensor_value)
        penalty = self.safety.get_penalty()

        return {
            "raw": round(raw_command, 3),
            "safe": round(safe_action, 3),
            "penalty": round(penalty, 4),
            "modified": abs(safe_action - raw_command) > 0.001
        }


def main():
    print("🚀 Gemma 4 + MicroSafe-RL Safety Bridge Demo\n")
    bridge = SafetyBridge()

    print(f"{'Step':<4} {'Sensor':<8} {'Raw Cmd':<10} {'Safe Cmd':<10} {'Penalty':<8} {'Shield'}")
    print("-" * 68)

    for step in range(20):
        sensor = round(0.45 + random.uniform(-0.25, 0.35), 3)
        raw_cmd = round(random.gauss(0.7, 0.6), 3)   # симулира по-реалистични команди от Gemma

        # Симулиране на опасни команди от време на време
        if random.random() < 0.25:
            raw_cmd = random.choice([1.85, -1.3, 2.1, -0.95])

        result = bridge.process(raw_cmd, sensor)

        shield = "🛡️ YES" if result["modified"] else "✅ NO"

        print(f"{step:<4} {sensor:<8} {result['raw']:<10} {result['safe']:<10} "
              f"{result['penalty']:<8} {shield}")

    print("\n✅ Демо завърши успешно! Safety Bridge работи.")


if __name__ == "__main__":
    main()