import numpy as np
import pandas as pd
import sys

def profile_motor(csv_file, column_name):
    print(f"🔍 Analyzing data from: {csv_file}...")
    
    try:
        df = pd.read_csv(csv_file)
        data = df[column_name].values
    except Exception as e:
        print(f"❌ Error reading file: {e}")
        return

    # 1. Basic statistical analysis of the signal
    signal_mean = np.mean(data)
    velocities = np.abs(np.diff(data))
    max_velocity = np.max(velocities)
    
    # Empirical Mean Absolute Deviation (MAD) - how noisy the motor is normally
    mad = np.mean(np.abs(data - signal_mean))

    # 2. CALCULATING PARAMETERS
    
    # DECAY: 0.95 is an industry standard for Edge AI (covers about 20 steps of history).
    decay = 0.95
    
    # ALPHA: Base penalty for lack of coherence. We keep it at 0.55 as a strong baseline.
    alpha = 0.55
    
    # BETA: Sensitivity to "unknown" spikes. 
    # We want the normal noise (MAD) to yield high coherence.
    # Formula: beta = 0.5 / mad -> ensures that normal deviations do not trigger the shield.
    beta = 0.5 / (mad if mad > 0 else 0.001)
    beta = round(beta, 2)

    # KAPPA: Global multiplier.
    # The most important step: We simulate the C++ core logic on normal data,
    # to see what the maximum "raw penalty" is when everything is operating normally.
    ema_mean = data[0]
    ema_mad = 0.0
    max_raw_penalty = 0.0
    
    for i in range(1, len(data)):
        x = data[i]
        v = velocities[i-1]
        
        # Simulation of MicroSafe-RL v3 EMA
        ema_mean = decay * ema_mean + (1.0 - decay) * x
        abs_dev = abs(x - ema_mean)
        ema_mad = decay * ema_mad + (1.0 - decay) * abs_dev
        
        coherence = 1.0 / (1.0 + abs_dev * beta)
        raw_penalty = ema_mad + alpha * (1.0 - coherence) + 0.3 * v
        
        if raw_penalty > max_raw_penalty:
            max_raw_penalty = raw_penalty

    # We want the maximum normal penalty to be no more than 0.05 (i.e., the AI has 95% freedom).
    target_max_normal_penalty = 0.05
    kappa = target_max_normal_penalty / (max_raw_penalty if max_raw_penalty > 0 else 1.0)
    kappa = round(kappa, 3)

    # 3. Generating the result for the user
    print("\n📊 MicroSafe-RL Auto-Tuner Report")
    print("==================================")
    print(f"Signal Noise (MAD):   {mad:.4f}")
    print(f"Max Velocity (V):     {max_velocity:.4f}")
    print("-" * 34)
    print("✅ Optimal parameters found:")
    print(f"  kappa = {kappa}")
    print(f"  alpha = {alpha}")
    print(f"  decay = {decay}")
    print(f"  beta  = {beta}")
    print("-" * 34)
    print("🚀 Copy this line directly into your C++ code (MicroSafeRL.h):")
    print(f"\nMicroSafeRL safety({kappa}f, {alpha}f, {decay}f, {beta}f, 1.0f, -1.5f, 1.5f, 0.05f);\n")


if __name__ == "__main__":
    # For testing, if the script is run without arguments, create a simulated file
    if len(sys.argv) < 2:
        print("⚠️ No CSV file provided. Generating simulated 'normal' data for testing...")
        
        # Simulate normal motor operation with slight noise (sine wave + noise)
        t = np.linspace(0, 10, 500)
        sim_data = np.sin(t) + np.random.normal(0, 0.1, 500)
        pd.DataFrame({"sensor_signal": sim_data}).to_csv("test_motor_data.csv", index=False)
        
        profile_motor("test_motor_data.csv", "sensor_signal")
    else:
        profile_motor(sys.argv[1], "sensor_signal") # Change "sensor_signal" to your column name