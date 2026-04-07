import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("log.csv")  # преименувай файла

# --- AI vs Safe ---
plt.figure()
plt.plot(df["time"], df["ai"], label="AI action")
plt.plot(df["time"], df["safe"], label="Safe output")
plt.legend()
plt.title("AI vs Safe Control")
plt.xlabel("Time")
plt.ylabel("Signal")

# --- Safety metrics ---
plt.figure()
plt.plot(df["time"], df["penalty"], label="Penalty")
plt.plot(df["time"], df["instability"], label="Instability")
plt.legend()
plt.title("Safety Metrics")

# --- latency ---
plt.figure()
plt.plot(df["time"], df["latency"])
plt.title("Latency (µs)")

plt.show()