#ifndef MICRO_SAFE_RL_H
#define MICRO_SAFE_RL_H

class MicroSafeRL {
private:
    float ema_mean;
    float ema_mad;
    float prev_value;
    float current_penalty;

    float kappa, alpha, beta, lambda_, max_penalty;
    float min_limit, max_limit, gravity_factor;
    bool initialized;

    inline float fast_abs(float x) const { return x < 0.0f ? -x : x; }

public:
    MicroSafeRL(float k = 1.15f, float a = 0.55f, float b = 2.2f,
                float lm = 0.12f, float max_p = 1.0f,
                float l_min = -1.5f, float l_max = 1.5f, float g = 0.05f)
        : kappa(k), alpha(a), beta(b), lambda_(lm), max_penalty(max_p),
          min_limit(l_min), max_limit(l_max), gravity_factor(g),
          ema_mean(0.0f), ema_mad(0.0f), prev_value(0.0f),
          current_penalty(0.0f), initialized(false) {}

    void init(float initial_sensor = 0.0f) {
        ema_mean = initial_sensor;
        ema_mad = 0.0f;
        prev_value = initial_sensor;
        current_penalty = 0.0f;
        initialized = true;
    }

    float apply_safe_control(float ai_action, float sensor_val) {
        if (!initialized) {
            init(sensor_val);
            if (ai_action > max_limit) return max_limit;
            if (ai_action < min_limit) return min_limit;
            return ai_action;
        }

        ema_mean = lambda_ * ema_mean + (1.0f - lambda_) * sensor_val;
        float abs_dev = fast_abs(sensor_val - ema_mean);
        ema_mad = lambda_ * ema_mad + (1.0f - lambda_) * abs_dev;

        float velocity = fast_abs(sensor_val - prev_value);
        prev_value = sensor_val;

        float coherence = 1.0f / (1.0f + abs_dev * beta);
        float raw = ema_mad + alpha * (1.0f - coherence) + 0.3f * velocity;

        current_penalty = kappa * raw;
        if (current_penalty > max_penalty) current_penalty = max_penalty;

        float gravity = max(0.0f, 1.0f - current_penalty * gravity_factor);
        float modulated = ai_action * gravity;

        if (modulated > max_limit) return max_limit;
        if (modulated < min_limit) return min_limit;

        return modulated;
    }

    float get_penalty() const { return current_penalty; }
    float get_current_reward() const { return 1.0f - current_penalty; }

    void reset() {
        ema_mean = 0.0f;
        ema_mad = 0.0f;
        prev_value = 0.0f;
        current_penalty = 0.0f;
        initialized = false;
    }
};

#endif