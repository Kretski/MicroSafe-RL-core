Apache License 2.0 (default)

Everything in this repository is released under Apache-2.0. See `LICENSE`.

You may use it commercially, modify it, integrate it into a product, and shipthat product, without asking permission and without paying anything. Theconditions are the ordinary Apache-2.0 ones:

* Keep the copyright and license notice
* State significant changes you made to the files
* Include a copy of the license with any redistribution
* Do not use the project name or the author's name to endorse your product
* The patent grant terminates if you initiate patent litigation over thissoftware

If Apache-2.0 works for you, take it and go. No contact required./**
 * MicroSafeRL.h — MISRA-C:2012 Compliant Edition v2
 * ===================================================
 * Verified: Cppcheck 2.13.0 + MISRA addon
 * Remaining advisories: 2.5 (unused macros — devtool only),
 *                       2.7 (ctor param names — C++ limitation)
 * Critical/Required violations: ZERO
 *
 * Author : Kretski, Dimitar
 * DOI    : 10.5281/zenodo.19019599
 */

#ifndef MICRO_SAFE_RL_H
#define MICRO_SAFE_RL_H

#include <stdint.h>

typedef float float32_t;

/* Configuration macros — used by auto-tuner, not in core logic */
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

class MicroSafeRL {

public:
    explicit MicroSafeRL(
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
        ema_mean        = initial_sensor;
        ema_mad         = 0.0f;
        prev_value      = initial_sensor;
        current_penalty = 0.0f;
        initialized     = true;
    }

    float32_t apply_safe_control(
        float32_t ai_action,
        float32_t sensor_val
    )
    {
        float32_t result;

        if (!initialized) {
            init(sensor_val);
        }

        /* EMA update — Rule 12.1: explicit parentheses */
        ema_mean = (lambda_ * ema_mean) +
                   ((1.0f - lambda_) * sensor_val);

        float32_t abs_dev = fast_abs(sensor_val - ema_mean);

        ema_mad = (lambda_ * ema_mad) +
                  ((1.0f - lambda_) * abs_dev);

        float32_t velocity  = fast_abs(sensor_val - prev_value);
        prev_value          = sensor_val;

        float32_t coherence = 1.0f / (1.0f + (abs_dev * beta));
        float32_t raw       = ema_mad
                            + (alpha * (1.0f - coherence))
                            + (MSRL_VELOCITY_GAIN * velocity);

        current_penalty = kappa * raw;
        if (current_penalty > max_penalty) {
            current_penalty = max_penalty;
        }

        float32_t g_raw   = 1.0f - (current_penalty * gravity_factor);
        float32_t gravity = (g_raw > 0.0f) ? g_raw : 0.0f;
        float32_t mod     = ai_action * gravity;

        /* Hard clamp — single exit point (Rule 15.5) */
        if (mod > max_limit) {
            result = max_limit;
        } else if (mod < min_limit) {
            result = min_limit;
        } else {
            result = mod;
        }

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

    static inline float32_t fast_abs(float32_t x)
    {
        return (x < 0.0f) ? (-x) : x;
    }
};

#endif /* MICRO_SAFE_RL_H */
