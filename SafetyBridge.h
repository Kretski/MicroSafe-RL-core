#ifndef SAFETY_BRIDGE_H
#define SAFETY_BRIDGE_H

#include <cmath>           // for fabs()
#include "MicroSafeRL.h"

// SafetyBridge — thin wrapper around MicroSafeRL with structured result output.
// Adds is_safe flag (penalty < SAFE_THRESHOLD) for easy integration in control loops.

struct SafetyResult {
    float safe_action;   // attenuated + clamped command
    float penalty;       // instability score in [0, max_penalty]
    bool  was_modified;  // true if output differs from raw input by >0.001
    bool  is_safe;       // true if penalty < SAFE_THRESHOLD (default 0.9)
};

class SafetyBridge {
private:
    MicroSafeRL safety;

    // SAFE_THRESHOLD: penalty below which the system is considered stable.
    // 0.9 = 90% of max_penalty. Configurable via constructor if needed.
    // At default max_penalty=1.0, this means instability score < 0.9.
    static const float SAFE_THRESHOLD;  // defined below

public:
    SafetyBridge(float k = 1.15f, float a = 0.55f, float b = 2.2f,
                 float lm = 0.12f, float max_p = 1.0f,
                 float l_min = -1.5f, float l_max = 1.5f, float g = 0.05f)
        : safety(k, a, b, lm, max_p, l_min, l_max, g) {}

    void init(float initial_sensor = 0.0f) {
        safety.init(initial_sensor);
    }

    SafetyResult process(float raw_ai_command, float current_sensor) {
        SafetyResult result;

        float safe_cmd = safety.apply_safe_control(raw_ai_command, current_sensor);
        float penalty  = safety.get_penalty();

        result.safe_action  = safe_cmd;
        result.penalty      = penalty;
        result.was_modified = (fabs((double)(safe_cmd - raw_ai_command)) > 0.001);
        result.is_safe      = (penalty < SAFE_THRESHOLD);

        return result;
    }

    float get_penalty() const {
        return safety.get_penalty();
    }

    void reset() {
        safety.reset();
    }
};

// SAFE_THRESHOLD = 0.9 (90% of max_penalty=1.0).
// Rationale: allows moderate instability compensation while flagging
// near-saturation conditions before hard clamp engages.
const float SafetyBridge::SAFE_THRESHOLD = 0.9f;

#endif // SAFETY_BRIDGE_H
