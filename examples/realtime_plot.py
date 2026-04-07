import serial
import matplotlib.pyplot as plt

ser = serial.Serial('COM3', 115200)

times, ai, safe = [], [], []

plt.ion()

while True:
    line = ser.readline().decode(errors="ignore").strip()

    if not line or line.startswith("time"):
        continue

    try:
        t, a, s, *_ = map(float, line.split(","))
    except:
        continue

    times.append(t)
    ai.append(a)
    safe.append(s)

    if len(times) > 200:
        times.pop(0)
        ai.pop(0)
        safe.pop(0)

    plt.clf()
    plt.plot(times, ai, label="AI")
    plt.plot(times, safe, label="Safe")
    plt.legend()
    plt.title("Real-time MicroSafeRL")
    plt.pause(0.01)