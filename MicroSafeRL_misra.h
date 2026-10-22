/*
 * Copyright (c) 2026 Dimitar Kretski
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0. See the LICENSE file
 * in the repository root for the full text.
 */
/*
 * MicroSafe-RL — MISRA C++-oriented variant.
 * Implements the same algorithm as MicroSafeRL.h (reference implementation):
 *   - invalid action (NaN or |a| > 1e6)  -> 0.0f
 *   - invalid sensor (NaN or |x| > 1e6)  -> last valid sensor value
 *   - attenuation G = 1 / (1 + g * P), then hard clamp
 * Class name is MicroSafeRLMisra so both headers can be used in one build.
 */
#ifndef MICRO_SAFE_RL_MISRA_H
#define MICRO_SAFE_RL_MISRA_H

#include <stdint.h>

typedef float float32_t;

/* Configuration macros — defaults, not used by core logic */
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_KAPPA       (1.15f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_ALPHA       (0.55f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_BETA        (2.2f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_LAMBDA      (0.12f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_MAX_PENALTY (1.0f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_MIN_LIMIT   (-1.5f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_MAX_LIMIT   (1.5f)
/* cppcheck-suppress misra-c2012-2.5 */
#define MSRL_DEFAULT_GRAVITY     (0.05f)

#define MSRL_VELOCITY_GAIN       (0.3f)
#define MSRL_INVALID_BOUND       (1.0e6f)

class MicroSafeRLMisra {

public:
    explicit MicroSafeRLMisra(
        float32_t k_kappa,
        float32_t a_alpha,
        float32_t b_beta,
        float32_t lm_lambda,
        float32_t max_p,
        float32_t l_min,
        float32_t l_max,
        float32_t g_gravity
    )
        : kappa(k_kappa), alpha(a_alpha), beta(b_beta),
          lambda_(lm_lambda), max_penalty(max_p),
          min_limit(l_min), max_limit(l_max),
          gravity_factor(g_gravity),
          ema_mean(0.0f), ema_mad(0.0f), prev_value(0.0f),
          current_penalty(0.0f), initialized(false)
    {}

    void init(float32_t initial_sensor)
    {
        float32_t s = is_invalid(initial_sensor) ? 0.0f : initial_sensor;
        ema_mean        = s;
        ema_mad         = 0.0f;
        prev_value      = s;
        current_penalty = 0.0f;
        initialized     = true;
    }

    float32_t apply_safe_control(
        float32_t ai_action,
        float32_t sensor_val
    )
    {
        float32_t result;

        if (is_invalid(ai_action)) {
            /* Fail-safe: invalid action -> zero command */
            result = 0.0f;
        } else {
            float32_t s = sensor_val;
            if (is_invalid(s)) {
                /* Dead-reckoning: reuse last valid sensor value */
                s = initialized ? prev_value : 0.0f;
            }

            if (!initialized) {
                /* First call: initialise, clamp only */
                init(s);
                result = clamp(ai_action);
            } else {
                /* EMA update — Rule 12.1: explicit parentheses */
                ema_mean = (lambda_ * ema_mean) +
                           ((1.0f - lambda_) * s);

                float32_t abs_dev = fast_abs(s - ema_mean);

                ema_mad = (lambda_ * ema_mad) +
                          ((1.0f - lambda_) * abs_dev);

                float32_t velocity  = fast_abs(s - prev_value);
                prev_value          = s;

                float32_t coherence = 1.0f / (1.0f + (abs_dev * beta));
                float32_t raw       = ema_mad
                                    + (alpha * (1.0f - coherence))
                                    + (MSRL_VELOCITY_GAIN * velocity);

                current_penalty = kappa * raw;
                if (current_penalty > max_penalty) {
                    current_penalty = max_penalty;
                }
                if (current_penalty < 0.0f) {
                    current_penalty = 0.0f;
                }

                float32_t gain = 1.0f / (1.0f + (current_penalty * gravity_factor));
                if (gain > 1.0f) {
                    gain = 1.0f;
                }
                if (gain < 0.0f) {
                    gain = 0.0f;
                }

                result = clamp(ai_action * gain);
            }
        }

        /* Single exit point (Rule 15.5) */
        return result;
    }

    float32_t get_penalty(void)        const { return current_penalty; }
    float32_t get_current_reward(void) const { return 1.0f - current_penalty; }

    void reset(void)
    {
        ema_mean        = 0.0f;
        ema_mad         = 0.0f;
        prev_value      = 0.0f;
        current_penalty = 0.0f;
        initialized     = false;
    }

private:
    float32_t kappa;
    float32_t alpha;
    float32_t beta;
    float32_t lambda_;
    float32_t max_penalty;
    float32_t min_limit;
    float32_t max_limit;
    float32_t gravity_factor;
    float32_t ema_mean;
    float32_t ema_mad;
    float32_t prev_value;
    float32_t current_penalty;
    bool      initialized;

    static float32_t fast_abs(float32_t x)
    {
        return (x < 0.0f) ? (-x) : x;
    }

    static bool is_invalid(float32_t x)
    {
        /* NaN is the only value not equal to itself; also rejects |x| > bound and Inf */
        return (x != x) || (x > MSRL_INVALID_BOUND) || (x < (-MSRL_INVALID_BOUND));
    }

    float32_t clamp(float32_t x) const
    {
        float32_t r;
        if (x > max_limit) {
            r = max_limit;
        } else if (x < min_limit) {
            r = min_limit;
        } else {
            r = x;
        }
        return r;
    }
};

#endif /* MICRO_SAFE_RL_MISRA_H */
