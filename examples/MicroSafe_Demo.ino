#include "MicroSafeRL.h"

// 1. ИНИЦИАЛИЗАЦИЯ: Използваме старата v1 сигнатура (със 7 параметъра)
// (kappa, alpha, decay, beta, max_penalty, min_limit, max_limit)
MicroSafeRL safety(1.15f, 0.55f, 0.95f, 1.5f, 1.0f, -1.5f, 1.5f);

void setup() {
    Serial.begin(115200);
    // Изчакваме Serial монитора да зареди
    while (!Serial) { delay(10); } 
    
    Serial.println("=== MicroSafeRL Ultimate (v3 EMA + Hard Shield) ===");
    Serial.println("Start...");
    delay(1000);
}

void loop() {
    // --- СИМУЛИРАНА СРЕДА ---
    
    // 1. Четем сензора (симулираме нормален дрифт + лек шум)
    float time_sec = millis() / 1000.0f;
    float sensor_val = sin(time_sec) + (random(-10, 10) / 100.0f);

    // 2. AI Агентът подава команда
    // Нарочно подаваме ОПАСНА стойност (1.68), която е над max_limit (1.50)
    float ai_action = 1.68f; 

    // --- MICROSAFE-RL БЛОК ---
    
    // Засичаме колко микросекунди отнема изчислението
    unsigned long start_time = micros();

    // 3. Прилагаме щита (Тук се случва магията: EMA -> Soft Penalty -> Hard Clamp)
    float safe_action = safety.apply_safe_control(ai_action, sensor_val);

    unsigned long end_time = micros();
    unsigned long latency = end_time - start_time;

    // 4. Взимаме наградата за AI агента
    float reward = safety.get_current_reward();

    // --- ИЗХОД КЪМ SERIAL MONITOR ---
    
    Serial.print("AI: "); 
    Serial.print(ai_action, 2);
    
    Serial.print("  Safe: "); 
    // Ако щитът работи, тук ТРЯБВА да видим точно 1.50 (заради Hard Shielding-а)
    Serial.print(safe_action, 2); 
    
    Serial.print("  | Reward: "); 
    Serial.print(reward, 2);
    
    Serial.print("  | Latency: "); 
    Serial.print(latency); 
    Serial.println(" us");

    // Работим на 10Hz за демото
    delay(100); 
}