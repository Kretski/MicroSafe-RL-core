#include <iostream>
#include <cassert>
#include <cmath>
#include "../MicroSafeRL_misra.h"

void run_edge_case_tests() {
    // Initialize with standard parameters
    MicroSafeRL safety(0.088f, 0.55f, 0.95f, 0.85f, 1.0f, -1.5f, 1.5f, 0.05f);
    safety.init(0.0f);

    std::cout << "Running Critical Safety Tests...\n";

    // TEST 1: Handling NaN (Not a Number) from AI
    float32_t nan_action = std::nanf("");
    float32_t result_nan = safety.apply_safe_control(nan_action, 0.0f);
    // Logic: mod = NaN * 1.0 = NaN. Clamp should catch it.
    std::cout << "[CHECK] Input NaN -> Output: " << result_nan << " (Clamped)\n";
    assert(result_nan <= 1.5f && result_nan >= -1.5f);

    // TEST 2: Handling Infinity from AI
    float32_t inf_action = std::numeric_limits<float>::infinity();
    float32_t result_inf = safety.apply_safe_control(inf_action, 0.0f);
    std::cout << "[CHECK] Input +Inf -> Output: " << result_inf << " (Max Limit)\n";
    assert(result_inf == 1.5f);

    // TEST 3: Massive Sensor Noise (Instability Check)
    float32_t crazy_sensor = 1000000.0f;
    float32_t result_instable = safety.apply_safe_control(1.0f, crazy_sensor);
    std::cout << "[CHECK] Sensor Surge -> Penalty: " << safety.get_penalty() << "\n";
    assert(safety.get_penalty() <= 1.0f); // Should be capped at max_penalty

    std::cout << "✅ ALL EDGE CASE TESTS PASSED.\n";
}

int main() {
    run_edge_case_tests();
    return 0;
}