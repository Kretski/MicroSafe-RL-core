import matplotlib.pyplot as plt
import numpy as np
import re
import os

# --- ФУНКЦИЯ ЗА АВТОМАТИЧНО ПОЧИСТВАНЕ НА ДАННИТЕ ---
def load_clean_telemetry(file_path, value_index):
    """
    Извлича чисти числа от лог файловете.
    value_index=0 за Signal, value_index=1 за Penalty
    """
    data = []
    if not os.path.exists(file_path):
        print(f"ГРЕШКА: Файлът {file_path} не е намерен!")
        return np.array([])

    with open(file_path, 'r') as f:
        for line in f:
            # Регулярен израз за намиране на всички числа (цели и с десетична запетая)
            numbers = re.findall(r"[-+]?\d*\.\d+|\d+", line)
            if len(numbers) > value_index:
                try:
                    data.append(float(numbers[value_index]))
                except ValueError:
                    continue
    return np.array(data)

# --- 1. ЗАРЕЖДАНЕ НА ДАННИТЕ ---
print("Зареждане на телеметрията от /data...")

# В input_signal.csv търсим първото число на реда (Signal)
signal = load_clean_telemetry("data/input_signal.csv", 0)
# В output_signature.csv търсим второто число (Penalty)
# (Ако си копирал целия лог и в двата файла, скриптът ще се оправи)
penalty = load_clean_telemetry("data/output_signature.csv", 1)

# Проверка дали данните са с еднаква дължина
min_len = min(len(signal), len(penalty))
signal = signal[:min_len]
penalty = penalty[:min_len]

if min_len == 0:
    print("ГРЕШКА: CSV файловете са празни или невалидни!")
    exit()

# --- 2. СТИЛИЗИРАНЕ И ЧЕРТАНЕ ---
plt.style.use('seaborn-v0_8-muted') # Професионален стил
fig, ax1 = plt.subplots(figsize=(12, 6))

# Първа ос: Сензорен сигнал (Синьо)
color_signal = 'tab:blue'
ax1.set_xlabel('Time Step (Telemetry Sync)')
ax1.set_ylabel('Sensor Raw Signal', color=color_signal, fontweight='bold')
ax1.plot(signal, color=color_signal, label='Sensor Signal', linewidth=1.5, alpha=0.8)
ax1.tick_params(axis='y', labelcolor=color_signal)
ax1.grid(True, which='both', linestyle='--', alpha=0.5)

# Втора ос: Наказание (Оранжево/Червено)
ax2 = ax1.twinx() 
color_penalty = 'tab:orange'
ax2.set_ylabel('MicroSafe-RL Penalty (U_t)', color=color_penalty, fontweight='bold')
ax2.plot(penalty, color=color_penalty, label='Stability Signature', linewidth=2)
ax2.tick_params(axis='y', labelcolor=color_penalty)

# Оцветяване на зоната на "Хаос" (Автоматично засичане)
chaos_threshold = 0.25 # Праг за визуално маркиране
ax2.fill_between(range(len(penalty)), 0, penalty, 
                 where=(penalty > chaos_threshold), 
                 color='red', alpha=0.15, label='High Entropy Zone')

# Заглавие и финални щрихи
plt.title('MicroSafe-RL: Edge AI Stability Benchmarks', fontsize=14, fontweight='bold', pad=20)
fig.tight_layout()

# Легенда
lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper left')

# --- 3. ЗАПАЗВАНЕ И ПОКАЗВАНЕ ---
output_image = "Figure_Benchmark.png"
plt.savefig(output_image, dpi=300)
print(f"УСПЕХ: Графиката е запазена като {output_image}")
plt.show()