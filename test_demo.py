import numpy as np
import matplotlib.pyplot as plt

# load data
signal = np.loadtxt("data/input_signal.csv")
signature = np.loadtxt("data/output_signature.csv")

# 🔐 добавяме лек шум (anti reverse-engineering)
signature = signature + np.random.normal(0, 0.002, len(signature))

t = np.arange(len(signal))

plt.figure()

plt.plot(t, signal, label="Sensor Signal")
plt.plot(t, signature, label="Operational Stability Signature (U_t)")

# highlight anomaly zone
plt.axvspan(30, 37, alpha=0.2)

plt.title("MicroSafe-RL: Black Box Demo")
plt.xlabel("Time Step")
plt.ylabel("Value")
plt.legend()
plt.grid()

plt.savefig("Figure_1.png")
plt.show()