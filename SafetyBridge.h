#ifndef SAFETY_BRIDGE_H
#define SAFETY_BRIDGE_H

#include "MicroSafeRL.h"

struct SafetyResult {
    float safe_action;
    float penalty;
    bool  was_modified;
    bool  is_safe;
};

class SafetyBridge {
private:
    MicroSafeRL safety;

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

        result.safe_action   = safe_cmd;
        result.penalty       = penalty;
        result.was_modified  = (fabs(safe_cmd - raw_ai_command) > 0.001f);
        result.is_safe       = (penalty < 0.9f);

        return result;
    }

    float get_penalty() const {
        return safety.get_penalty();
    }
};

#endif