#ifndef MICRO_SAFE_RL_H
#define MICRO_SAFE_RL_H

class MicroSafeRL {
private:
    float ema_mean;
    float ema_mad;
    float prev_value;
    float kappa, alpha, decay, beta, max_penalty;
    
    // Лимитите за Hard Shielding (старите имена от v1)
    float min_limit, max_limit;
    float gravity_factor; 
    float current_penalty; 
    bool initialized;
    
    // Бърза абсолютна стойност (вместо бавния math.h abs)
    inline float fast_abs(float x) { return x < 0 ? -x : x; }
    
public:
    // Универсален конструктор със старите параметри:
    // (kappa, alpha, decay, beta, max_penalty, l_min, l_max, gravity_factor)
    MicroSafeRL(float k = 1.15f, float a = 0.55f, float d = 0.95f, float b = 1.5f, 
                float max_p = 1.0f, float l_min = -1.5f, float l_max = 1.5f, float g_fact = 0.05f)
        : kappa(k), alpha(a), decay(d), beta(b), max_penalty(max_p), 
          min_limit(l_min), max_limit(l_max), gravity_factor(g_fact) {
        reset();
    }
    
    // Старото име на основната функция: apply_safe_control
    float apply_safe_control(float ai_action, float sensor_val) {
        if (!initialized) {
            ema_mean = sensor_val;
            ema_mad = 0.0f;
            prev_value = sensor_val;
            current_penalty = 0.0f;
            initialized = true;
            
            // Hard Shielding за стартовия момент
            if (ai_action > max_limit) return max_limit;
            if (ai_action < min_limit) return min_limit;
            return ai_action; 
        }
        
        // --- v3 EMA СКОРОСТНА МАТЕМАТИКА ---
        ema_mean = decay * ema_mean + (1.0f - decay) * sensor_val;
        float abs_dev = fast_abs(sensor_val - ema_mean);
        ema_mad = decay * ema_mad + (1.0f - decay) * abs_dev;
        
        float velocity = fast_abs(sensor_val - prev_value);
        prev_value = sensor_val;
        
        float coherence = 1.0f / (1.0f + abs_dev * beta);
        float raw = ema_mad + alpha * (1.0f - coherence) + 0.3f * velocity;
        
        current_penalty = kappa * raw;
        if (current_penalty > max_penalty) current_penalty = max_penalty;

        // --- SOFT SHIELDING ---
        float gravity = 1.0f - (current_penalty * gravity_factor);
        if (gravity < 0.0f) gravity = 0.0f;
        float modulated_action = ai_action * gravity;

        // --- HARD SHIELDING (Твърдата стена от v1) ---
        if (modulated_action > max_limit) return max_limit;
        if (modulated_action < min_limit) return min_limit;

        return modulated_action;
    }

    // Старото име за връщане на наградата към AI агента
    float get_current_reward() {
        return 1.0f - current_penalty;
    }
    
    void reset() {
        ema_mean = 0.0f;
        ema_mad = 0.0f;
        prev_value = 0.0f;
        current_penalty = 0.0f;
        initialized = false;
    }
};

#endif