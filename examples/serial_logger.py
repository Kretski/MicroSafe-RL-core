import serial
import csv
import time

ser = serial.Serial('COM3', 115200, timeout=1)  # смени при нужда

filename = f"log_{int(time.time())}.csv"

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)

    print("Logging to", filename)

    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue

        print(line)

        try:
            writer.writerow(line.split(","))
        except:
            pass