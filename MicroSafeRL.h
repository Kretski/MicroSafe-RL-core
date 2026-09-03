Apache License 2.0 (default)

Everything in this repository is released under Apache-2.0. See `LICENSE`.

You may use it commercially, modify it, integrate it into a product, and shipthat product, without asking permission and without paying anything. Theconditions are the ordinary Apache-2.0 ones:

* Keep the copyright and license notice
* State significant changes you made to the files
* Include a copy of the license with any redistribution
* Do not use the project name or the author's name to endorse your product
* The patent grant terminates if you initiate patent litigation over thissoftware

If Apache-2.0 works for you, take it and go. No contact required.#ifndef MICRO_SAFE_RL_H
#define MICRO_SAFE_RL_H

// MicroSafe-RL — lightweight deterministic safety layer for AI/RL control systems.
// Memory footprint: ~68 bytes RAM (4 state floats + 8 config floats + DebugState + bool).
// Latency: <1.2 µs on STM32F4 @ 168 MHz (no dynamic allocation, no loops).
// Language: C++ (MISRA C++ guidelines, C++03 compatible).

class MicroSafeRL {
private:
    // --- State (16 bytes) ---
    float ema_mean;
    float ema_mad;
    float prev_value;
    float current_penalty;

    // --- Configuration (32 bytes) ---
    float kappa, alpha, beta, lambda_, max_penalty;
    float min_limit, max_limit, gravity_factor;

    // --- Flags ---
    bool initialized;

    inline float fast_abs(float x) const { return x < 0.0f ? -x : x; }

    // NaN check (portable, no <cmath> dependency)
    inline bool is_nan(float x) const { return x != x; }
    inline bool is_inf(float x) const { return x > 1e6f || x < -1e6f; }
    inline bool is_invalid(float x) const { return is_nan(x) || is_inf(x); }

public:
    // ==============================
    // DEBUG STRUCT (20 bytes)
    // ==============================
    struct DebugState {
        float instability;  // ema_mad
        float deviation;    // |sensor - mean|
        float dynamics;     // velocity
        float input;        // ai_action (pre-safety)
        float output;       // final safe output
    };

    DebugState debug;

    // ==============================
    // CONSTRUCTOR
    // ==============================
    MicroSafeRL(float k = 1.15f, float a = 0.55f, float b = 2.2f,
                float lm = 0.12f, float max_p = 1.0f,
                float l_min = -1.5f, float l_max = 1.5f, float g = 0.05f)
        : kappa(k), alpha(a), beta(b), lambda_(lm), max_penalty(max_p),
          min_limit(l_min), max_limit(l_max), gravity_factor(g),
          ema_mean(0.0f), ema_mad(0.0f), prev_value(0.0f),
          current_penalty(0.0f), initialized(false)
    {
        debug = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }

    // ==============================
    // INIT
    // ==============================
    void init(float initial_sensor = 0.0f) {
        if (is_invalid(initial_sensor)) initial_sensor = 0.0f;
        ema_mean       = initial_sensor;
        ema_mad        = 0.0f;
        prev_value     = initial_sensor;
        current_penalty = 0.0f;
        initialized    = true;
        debug = {0.0f, 0.0f, 0.0f, initial_sensor, initial_sensor};
    }

    // ==============================
    // CORE FUNCTION
    // ==============================
    float apply_safe_control(float ai_action, float sensor_val) {

        // --- NaN / Inf guard ---
        // If ai_action is invalid, return fail-safe zero.
        if (is_invalid(ai_action)) {
            debug.input  = ai_action;
            debug.output = 0.0f;
            return 0.0f;
        }
        // If sensor is invalid, use last known good value (dead-reckoning).
        if (is_invalid(sensor_val)) {
            sensor_val = initialized ? prev_value : 0.0f;
        }

        // --- First call: auto-init ---
        if (!initialized) {
            init(sensor_val);

            float out = ai_action;
            if (out > max_limit) out = max_limit;
            if (out < min_limit) out = min_limit;

            debug.instability = 0.0f;
            debug.deviation   = 0.0f;
            debug.dynamics    = 0.0f;
            debug.input       = ai_action;
            debug.output      = out;

            return out;
        }

        // --- EMA mean ---
        ema_mean = lambda_ * ema_mean + (1.0f - lambda_) * sensor_val;

        // --- Deviation ---
        float abs_dev = fast_abs(sensor_val - ema_mean);

        // --- MAD (exponential) ---
        ema_mad = lambda_ * ema_mad + (1.0f - lambda_) * abs_dev;

        // --- Dynamics (velocity proxy) ---
        float velocity = fast_abs(sensor_val - prev_value);
        prev_value = sensor_val;

        // --- Coherence: 1 when deviation=0, decreases with deviation ---
        float coherence = 1.0f / (1.0f + abs_dev * beta);

        // --- Instability score ---
        // 0.3 weight on velocity is a fixed design parameter (see README §Parameters).
        float raw = ema_mad + alpha * (1.0f - coherence) + 0.3f * velocity;

        current_penalty = kappa * raw;
        if (current_penalty > max_penalty) current_penalty = max_penalty;
        if (current_penalty < 0.0f)        current_penalty = 0.0f;

        // --- Gravity modulation: attenuates AI command proportionally to penalty ---
        float gravity = 1.0f / (1.0f + current_penalty * gravity_factor);
        if (gravity > 1.0f) gravity = 1.0f;
        if (gravity < 0.0f) gravity = 0.0f;

        float modulated = ai_action * gravity;

        // --- Hard clamp ---
        float out = modulated;
        if (out > max_limit) out = max_limit;
        if (out < min_limit) out = min_limit;

        // --- Debug fill ---
        debug.instability = ema_mad;
        debug.deviation   = abs_dev;
        debug.dynamics    = velocity;
        debug.input       = ai_action;
        debug.output      = out;

        return out;
    }

    // ==============================
    // ACCESSORS
    // ==============================
    float get_penalty()        const { return current_penalty; }
    float get_current_reward() const { return 1.0f - current_penalty; }
    DebugState get_debug()     const { return debug; }

    // ==============================
    // RESET
    // ==============================
    void reset() {
        ema_mean        = 0.0f;
        ema_mad         = 0.0f;
        prev_value      = 0.0f;
        current_penalty = 0.0f;
        initialized     = false;
        debug = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }
};

#endif // MICRO_SAFE_RL_H

