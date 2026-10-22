// Edge-case tests: both implementations must give identical results.
// Build: g++ -std=c++11 tests/test_safety_core.cpp -o test_safety_core && ./test_safety_core
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "../MicroSafeRL.h"
#include "../MicroSafeRL_misra.h"

static MicroSafeRL make_ref() {
    return MicroSafeRL(1.15f, 0.55f, 2.2f, 0.12f, 1.0f, -1.5f, 1.5f, 0.05f);
}

static MicroSafeRLMisra make_misra() {
    return MicroSafeRLMisra(MSRL_DEFAULT_KAPPA, MSRL_DEFAULT_ALPHA, MSRL_DEFAULT_BETA,
                            MSRL_DEFAULT_LAMBDA, MSRL_DEFAULT_MAX_PENALTY,
                            MSRL_DEFAULT_MIN_LIMIT, MSRL_DEFAULT_MAX_LIMIT,
                            MSRL_DEFAULT_GRAVITY);
}

template <class S>
static void edge_cases(S safety, const char* name) {
    safety.init(0.0f);
    const float nan_v = std::numeric_limits<float>::quiet_NaN();
    const float inf_v = std::numeric_limits<float>::infinity();

    // 1. NaN action -> fail-safe zero
    float r = safety.apply_safe_control(nan_v, 0.0f);
    assert(r == 0.0f);

    // 2. +Inf / -Inf action -> fail-safe zero
    assert(safety.apply_safe_control(inf_v, 0.0f) == 0.0f);
    assert(safety.apply_safe_control(-inf_v, 0.0f) == 0.0f);

    // 3. NaN sensor -> output stays finite and within bounds
    r = safety.apply_safe_control(1.0f, nan_v);
    assert(r == r && r >= -1.5f && r <= 1.5f);

    // 4. Huge sensor value is treated as invalid; penalty stays capped
    r = safety.apply_safe_control(1.0f, 1.0e7f);
    assert(r == r && safety.get_penalty() <= 1.0f);

    // 5. Large finite action is clamped
    assert(safety.apply_safe_control(1000.0f, 0.0f) <= 1.5f);
    assert(safety.apply_safe_control(-1000.0f, 0.0f) >= -1.5f);

    std::cout << "[PASS] Edge cases: " << name << "\n";
}

static void equivalence() {
    MicroSafeRL ref = make_ref();
    MicroSafeRLMisra mis = make_misra();
    ref.init(0.0f);
    mis.init(0.0f);
    std::srand(3);

    float max_diff = 0.0f;
    for (int i = 0; i < 100000; i++) {
        float a = ((std::rand() / (float)RAND_MAX) - 0.5f) * 6.0f;
        float x = ((std::rand() / (float)RAND_MAX) - 0.5f) * 4.0f;
        float d = std::fabs(ref.apply_safe_control(a, x) - mis.apply_safe_control(a, x));
        if (d > max_diff) max_diff = d;
    }
    assert(max_diff < 1e-6f);
    std::cout << "[PASS] Reference and MISRA variant agree (max |diff| = " << max_diff << ")\n";
}

int main() {
    edge_cases(make_ref(), "MicroSafeRL.h");
    edge_cases(make_misra(), "MicroSafeRL_misra.h");
    equivalence();
    std::cout << "\nAll edge-case tests passed.\n";
    return 0;
}
