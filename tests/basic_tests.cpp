#include <iostream>
#include <cmath>
#include <cassert>
#include "../MicroSafeRL.h"

void test_normal_behavior() {
    MicroSafeRL safety;

    float output = safety.apply_safe_control(1.0f, 1.0f);

    assert(fabs(output - 1.0f) < 0.01f);
    std::cout << "[PASS] Normal behavior\n";
}

void test_clamp_upper_bound() {
    MicroSafeRL safety;

    float output = safety.apply_safe_control(100.0f, 1.0f);

    assert(output <= safety.MAX_OUTPUT);
    std::cout << "[PASS] Clamp upper bound\n";
}

void test_clamp_lower_bound() {
    MicroSafeRL safety;

    float output = safety.apply_safe_control(-100.0f, 1.0f);

    assert(output >= safety.MIN_OUTPUT);
    std::cout << "[PASS] Clamp lower bound\n";
}

void test_spike_input() {
    MicroSafeRL safety;

    float stable = safety.apply_safe_control(1.0f, 1.0f);
    float spike  = safety.apply_safe_control(10.0f, 1.0f);

    assert(spike < 10.0f); // attenuation expected
    std::cout << "[PASS] Spike attenuation\n";
}

void test_noise_resilience() {
    MicroSafeRL safety;

    float base = 1.0f;
    float noisy_sum = 0.0f;

    for (int i = 0; i < 50; i++) {
        float noise = ((rand() % 100) / 1000.0f) - 0.05f;
        noisy_sum += safety.apply_safe_control(base + noise, base);
    }

    float avg = noisy_sum / 50.0f;

    assert(fabs(avg - base) < 0.2f);
    std::cout << "[PASS] Noise resilience\n";
}

int main() {
    test_normal_behavior();
    test_clamp_upper_bound();
    test_clamp_lower_bound();
    test_spike_input();
    test_noise_resilience();

    std::cout << "\nAll tests passed.\n";
    return 0;
}