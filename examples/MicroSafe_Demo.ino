#include "MicroSafeRL.h"

MicroSafeRL safety;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("time,ai,safe,reward,penalty,instability,deviation,dynamics,latency");
    delay(1000);
}

void loop() {
    float time_sec = millis() / 1000.0f;

    // --- simulated sensor ---
    float sensor_val = sin(time_sec) + (random(-10, 10) / 100.0f);

    // spike на всеки 10 секунди
    if ((int)time_sec % 10 == 0) {
        sensor_val += 2.0f;
    }

    // dangerous AI command
    float ai_action = 1.68f;

    unsigned long t0 = micros();

    float safe_action = safety.apply_safe_control(ai_action, sensor_val);

    unsigned long latency = micros() - t0;

    float reward = safety.get_current_reward();
    auto dbg = safety.get_debug();

    // --- CSV output ---
    Serial.print(time_sec, 3); Serial.print(",");
    Serial.print(ai_action, 3); Serial.print(",");
    Serial.print(safe_action, 3); Serial.print(",");
    Serial.print(reward, 3); Serial.print(",");
    Serial.print(safety.get_penalty(), 3); Serial.print(",");
    Serial.print(dbg.instability, 3); Serial.print(",");
    Serial.print(dbg.deviation, 3); Serial.print(",");
    Serial.print(dbg.dynamics, 3); Serial.print(",");
    Serial.println(latency);

    delay(100); // 10Hz
}