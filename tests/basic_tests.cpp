// Basic behaviour tests for MicroSafeRL.h (reference implementation).
// Build: g++ -std=c++11 tests/basic_tests.cpp -o basic_tests && ./basic_tests
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include "../MicroSafeRL.h"

static void test_normal_behavior() {
    MicroSafeRL safety;
    safety.init(1.0f);

    // Steady sensor: penalty stays ~0, command passes almost unchanged
    float output = safety.apply_safe_control(1.0f, 1.0f);
    assert(std::fabs(output - 1.0f) < 0.01f);
    std::cout << "[PASS] Normal behaviour\n";
}

static void test_clamp_upper_bound() {
    MicroSafeRL safety;
    safety.init(1.0f);

    float output = safety.apply_safe_control(100.0f, 1.0f);
    assert(output <= safety.get_max_limit());
    std::cout << "[PASS] Clamp upper bound\n";
}

static void test_clamp_lower_bound() {
    MicroSafeRL safety;
    safety.init(1.0f);

    float output = safety.apply_safe_control(-100.0f, 1.0f);
    assert(output >= safety.get_min_limit());
    std::cout << "[PASS] Clamp lower bound\n";
}

static void test_penalty_rises_on_disturbance() {
    MicroSafeRL safety;
    safety.init(0.0f);

    for (int i = 0; i < 20; i++) safety.apply_safe_control(1.0f, 0.0f);
    float calm = safety.get_penalty();

    safety.apply_safe_control(1.0f, 2.0f);   // sudden sensor jump
    float disturbed = safety.get_penalty();

    assert(disturbed > calm);
    assert(disturbed <= safety.get_max_penalty());
    std::cout << "[PASS] Penalty rises on disturbance (" << calm << " -> " << disturbed << ")\n";
}

static void test_noise_resilience() {
    MicroSafeRL safety;
    safety.init(1.0f);
    std::srand(1);

    float sum = 0.0f;
    for (int i = 0; i < 50; i++) {
        float noise = ((std::rand() % 100) / 1000.0f) - 0.05f;   // +/- 0.05
        sum += safety.apply_safe_control(1.0f, 1.0f + noise);
    }
    float avg = sum / 50.0f;

    assert(std::fabs(avg - 1.0f) < 0.2f);
    std::cout << "[PASS] Noise resilience (avg " << avg << ")\n";
}

static void test_randomised_bounds() {
    MicroSafeRL safety;
    safety.init(0.0f);
    std::srand(7);

    for (int i = 0; i < 100000; i++) {
        float a = ((std::rand() / (float)RAND_MAX) - 0.5f) * 2000.0f;
        float x = ((std::rand() / (float)RAND_MAX) - 0.5f) * 200.0f;
        float out = safety.apply_safe_control(a, x);
        assert(out >= safety.get_min_limit() && out <= safety.get_max_limit());
    }
    std::cout << "[PASS] Bounds hold on 100,000 random cases\n";
}

int main() {
    test_normal_behavior();
    test_clamp_upper_bound();
    test_clamp_lower_bound();
    test_penalty_rises_on_disturbance();
    test_noise_resilience();
    test_randomised_bounds();

    std::cout << "\nAll basic tests passed.\n";
    return 0;
}
