import numpy as np
import time
from wrappers.microsafe_gym import MicroSafeWrapper
import gymnasium as gym

# Симулираме олекотен интерфейс към Gemma 4 / Edge LLM
class Gemma4EdgeAgent:
    def __init__(self):
        print("🧠 Gemma 4 Edge Model Loaded (Optimized for Robotics)")

    def get_action(self, observation):
        """
        Gemma анализира данните от сензорите и решава какво да прави.
        Понякога 'халюцинира' и дава опасни команди (напр. над 1.5).
        """
        # Симулираме нормална команда, която внезапно става нестабилна
        base_action = np.sin(time.time()) 
        if np.random.rand() > 0.85:
            return 2.5  # ОПАСНА КОМАНДА: Gemma иска прекомерна мощност/ъгъл
        return base_action

def run_safe_ai_demo():
    print("🛡️ Starting MicroSafe-RL + Gemma 4 Integration Demo")
    
    # 1. Създаваме средата (напр. роботизирана ръка или балансиращ робот)
    env = gym.make("CartPole-v1", render_mode="human")
    
    # 2. THE BRIDGE: Обгръщаме средата с твоя предпазен слой
    # Параметрите идват от твоя профилировчик (microsafe_profiler.py)
    env = MicroSafeWrapper(env, alpha=0.078, beta=0.55, kappa=0.95)
    
    agent = Gemma4EdgeAgent()
    observation, info = env.reset()

    print("\n--- Real-Time Safety Monitoring Active ---")
    
    for _ in range(500):
        # 3. Gemma 4 генерира команда на високо ниво
        raw_action = agent.get_action(observation)
        
        # 4. MICROSAFE-RL ИНТЕРВЕНЦИЯ
        # Тук твоят код взима решение за 1.18 µs дали да пусне командата
        observation, reward, terminated, truncated, info = env.step(raw_action)
        
        # 5. Демонстрираме разликата
        safe_action = info.get('safe_action', raw_action)
        
        if abs(raw_action - safe_action) > 0.01:
            print(f"⚠️ [INTERCEPTED] Gemma requested: {raw_action:.2f} | MicroSafe enforced: {safe_action:.2f}")
        else:
            print(f"✅ [SAFE] Executing Gemma command: {raw_action:.2f}")

        if terminated or truncated:
            observation, info = env.reset()
        
        time.sleep(0.05) # Симулираме реален контролен цикъл

    env.close()

if __name__ == "__main__":
    run_safe_ai_demo()