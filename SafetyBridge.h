/*
 * Copyright (c) 2026 Dimitar Kretski
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0. See the LICENSE file
 * in the repository root for the full text.
 */
#ifndef SAFETY_BRIDGE_H
#define SAFETY_BRIDGE_H

#include "MicroSafeRL.h"

// SafetyBridge — thin wrapper around MicroSafeRL with structured result output.
// is_safe = penalty < 0.9 * max_penalty: flags near-saturation before the
// hard clamp engages, so upstream logic has a cycle or two to react.

struct SafetyResult {
    float safe_action;   // attenuated + clamped command
    float penalty;       // instability score in [0, max_penalty]
    bool  was_modified;  // |safe_action - raw| > 0.001 (absolute)
    bool  is_safe;       // penalty < 0.9 * max_penalty
};

class SafetyBridge {
public:
    SafetyBridge(float k = 1.15f, float a = 0.55f, float b = 2.2f,
                 float lm = 0.12f, float max_p = 1.0f,
                 float l_min = -1.5f, float l_max = 1.5f, float g = 0.05f)
        : safety(k, a, b, lm, max_p, l_min, l_max, g),
          safe_threshold(0.9f * max_p) {}

    void init(float initial_sensor = 0.0f) {
        safety.init(initial_sensor);
    }

    SafetyResult process(float raw_ai_command, float current_sensor) {
        SafetyResult result;

        float safe_cmd = safety.apply_safe_control(raw_ai_command, current_sensor);
        float penalty  = safety.get_penalty();
        float diff     = safe_cmd - raw_ai_command;
        if (diff < 0.0f) diff = -diff;

        result.safe_action  = safe_cmd;
        result.penalty      = penalty;
        // A NaN input makes diff NaN; report that as modified.
        result.was_modified = (diff > 0.001f) || (diff != diff);
        result.is_safe      = (penalty < safe_threshold);

        return result;
    }

    float get_penalty() const {
        return safety.get_penalty();
    }

    void reset() {
        safety.reset();
    }

private:
    MicroSafeRL safety;
    float safe_threshold;
};

#endif // SAFETY_BRIDGE_H
