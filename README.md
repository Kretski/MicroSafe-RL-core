# MicroSafe-RL Core

**A deterministic runtime output filter for AI/RL-assisted control on constrained hardware.**

MicroSafe-RL Core sits between a controller (classical, RL, or learned) and aphysical actuator. It does not replace the controller and does not model theplant. It monitors sensor behaviour, attenuates commands when short-terminstability rises, and enforces hard actuator bounds before execution.

    AI / RL policy  →  MicroSafe-RL  →  actuator

* * *

## What problem this addresses

A controller can emit a numerically valid command that is bad for the hardware:beyond actuator limits, spiking during noisy sensor phases, or overshootingduring recovery from a disturbance. On microcontroller-class targets thestandard runtime-assurance approaches — control barrier functions with a QPsolver, Simplex architectures with a verified backup controller — need a plantmodel and compute budget that a Cortex-M control loop often does not have.

MicroSafe-RL Core is a deliberately minimal alternative for that gap: model-free,fixed-cost, and small enough to sit inside an existing loop withoutrestructuring it.

* * *

## Scope and honest positioning

This is important enough to state before the feature list.

**What the hard bounds guarantee.** For finite inputs, the output is alwayswithin `[a_min, a_max]`. This is an unconditional guarantee and it is thecomponent doing the load-bearing safety work.

**What the adaptive layer currently contributes.** Attenuation is`a_safe = clip(a / (1 + g·P))`. At the default `gravity_factor = 0.05` and`max_penalty = 1.0`, the maximum possible attenuation is:

    1 / (1 + 0.05 · 1.0) = 0.952  →  4.76% reduction

That is small. The penalty signal is presently more useful as **observability**— an exported instability score that upstream logic can react to — than as anactuator-level intervention. Users who want the attenuation to have visibleauthority must raise `gravity_factor` substantially and re-tune for their plant.

**What this is not.** It is not a safety controller, not a certified component,and not a substitute for a hardware interlock or a verified backup controller.It has no formal guarantee of closed-loop stability (see Limitations).

* * *

## Memory and latency

| Metric | Value | Measurement |
| --- | --- | --- |
| Runtime state | 16 bytes | 4 × `float`: `ema_mean`, `ema_mad`, `prev_value`, `current_penalty` |
| Full object | ~68 bytes | State + 8 config floats + `DebugState` + flag; verify with `sizeof()` on your target |
| Worst-case latency | < 1.2 µs | STM32F4 @ 168 MHz, DWT cycle counter |
| Dynamic allocation | None | Header-only, no heap, no recursion, no data-dependent loops |

Latency was measured on hardware, not estimated. The measurement harness isbeing packaged for release so the figure is independently reproducible; untilthen, treat it as indicative of the right order of magnitude rather than as acertified worst case.

* * *

## Algorithm

Per control cycle, given sensor reading `x_t` and proposed action `a_t`:

    NaN/Inf guard      invalid a_t → 0.0 ; invalid x_t → prev_value
    EMA                mu_t  = λ·mu_{t-1} + (1-λ)·x_t
    deviation          d_t   = |x_t - mu_t|
    EWMA-MAD           M_t   = λ·M_{t-1} + (1-λ)·d_t
    sensor velocity    v_t   = |x_t - x_{t-1}|
    coherence          C_t   = 1 / (1 + β·d_t)
    instability        R_t   = M_t + α·(1 - C_t) + 0.3·v_t
    penalty            P_t   = min(κ·R_t, P_max)
    attenuation        G_t   = 1 / (1 + g·P_t)
    output             a_safe = clip(a_t · G_t, a_min, a_max)

Note that `M_t` is an exponentially weighted mean absolute deviation updatedevery cycle, not a batch MAD over a window. The velocity term tracks the*sensor*, not the commanded action — the filter responds to how the physicalstate is moving, not only to what the controller proposes.

### Default parameters

    kappa          = 1.15
    alpha          = 0.55
    beta           = 2.2
    lambda         = 0.12
    max_penalty    = 1.0
    gravity_factor = 0.05
    bounds         = [-1.5, +1.5]

These are starting values from bench tuning on a small rotor, notgeneral-purpose defaults. See Limitations regarding units.

* * *

## Usage

    #include "SafetyBridge.h"
    
    SafetyBridge bridge;
    
    void setup() {
        bridge.init(0.0f);
    }
    
    void loop() {
        float raw_command = get_controller_output();
        float sensor_val  = read_sensor();
    
        SafetyResult result = bridge.process(raw_command, sensor_val);
        actuator_write(result.safe_action);
    }

    struct SafetyResult {
        float safe_action;   // attenuated and clamped command
        float penalty;       // instability score in [0, max_penalty]
        bool  was_modified;  // |safe_action - raw| > 0.001 (absolute)
        bool  is_safe;       // penalty < 0.9 * max_penalty
    };

**Caveat on `was_modified`:** the threshold is absolute. With default`gravity_factor`, attenuation of a command with |a| < 0.021 falls below 0.001,so the flag reads false even though the filter acted. Use `penalty` rather than`was_modified` if you need to detect low-amplitude intervention. A relativethreshold is planned.

**Rationale for `is_safe` at 0.9 × max_penalty:** flags near-saturation beforethe hard clamp engages, so upstream logic has a cycle or two to react.

* * *

## Invalid-input handling

    a_t  is NaN or |a_t|  > 1e6   →  return 0.0f            (fail-safe)
    x_t  is NaN or |x_t|  > 1e6   →  use prev_value          (dead-reckoning)

Note that fail-safe zero is only genuinely safe on plants where zero command isa safe state. On a gravity-loaded joint or a lifting rotor it is not. This is aplant-dependent configuration decision, not a property of the library — seeLimitations.

* * *

## Limitations and known issues

Stated openly because they determine where this is and is not appropriate.

1. **Zero is not universally safe.** Both the attenuation path and theNaN fail-safe drive the command toward zero. On systems where zero commandmeans "fall", "stall", or "release", this is the wrong fallback. Aconfigurable fallback action (e.g. attenuate toward a gravity-compensationterm) is the highest-priority planned change.
  
2. **No closed-loop stability analysis.** Inserting a state-dependent gainchanges the dynamics of the closed loop, and control authority is reducedprecisely when sensor velocity is high. Limit cycles or slow divergence areplausible failure modes and have not been ruled out analytically orempirically.
  
3. **`R_t` is dimensionally inconsistent.** It sums a quantity in sensor units(`M_t`), a dimensionless quantity (`1 - C_t`), and a velocity inunits-per-sample (`v_t`). Rescaling the sensor changes `R_t` and invalidates`kappa`, `beta`, and the hard-coded `0.3`. Parameters are therefore notportable between applications without retuning. Normalisation is planned.
  
4. **No baseline comparison yet.** Verification to date establishesself-consistency (the code implements the equations) and invariants (boundshold, determinism holds). It does **not** establish that the filter improvessafety relative to plain clipping. See Validation status.
  
5. **Attenuation authority is low at defaults.** See Scope above.
  

* * *

## Validation status

**Established:**

| Property | Method |
| --- | --- |
| Output within bounds for finite inputs | 100,000 randomised cases, 0 violations |
| Determinism (same state + params → same output) | Reference test suite |
| Penalty saturation at `max_penalty` | Reference test suite |
| Penalty rise under disturbance, decay after | Reference test suite |
| Response to sensor velocity | Reference test suite |
| Math matches the C++ header | Python reference reimplementation |
| Runs in a real control loop | STM32F4 + rotor bench, serial trace |

**Not established:**

* Improvement over hard clipping under matched disturbance scenarios
* Closed-loop stability on any plant
* Behaviour on a plant where zero command is unsafe
* Parameter transferability across systems
* Independently reproducible latency figure

The next milestone is a benchmark comparing **raw controller / hard clipping /MicroSafe-RL / CBF-QP** on a common plant (reaction-wheel pendulum insimulation, then hardware) under identical disturbances, reporting constraintviolations, action jerk, task performance loss, intervention rate, and computecost. Until that exists, claims about this layer's benefit should be read ashypotheses.

* * *

## Related work

MicroSafe-RL Core belongs to the runtime-assurance family, alongside controlbarrier functions and ASIF, Simplex architectures, reference governors, andshielded RL. Those methods offer stronger guarantees and require a plant model,a solver, or a verified backup controller. This library trades the guarantee fora fixed sub-microsecond cost and a 16-byte state on hardware where those methodsdo not fit. That trade is the contribution, and it is only worth anything if thebenchmark above shows a measurable gain over plain clipping.

* * *

## Repository structure

    MicroSafeRL.h                Core C++ safety layer
    MicroSafeRL_misra.h          MISRA C++-oriented variant
    SafetyBridge.h               Runtime bridge API
    MISRA_compliance_report.txt  Static-analysis notes
    microsafe_profiler.py        Simulation and tuning helper
    paper_mode.py                Benchmark script
    gemma_safety_demo.py         Example integration demo

Header-only, C++03 compatible, MISRA C++ guidelines.

* * *

## Roadmap

* Configurable fallback action (not hard-coded zero)
* Dimensional normalisation of `R_t`
* Relative threshold for `was_modified`
* Published latency benchmark harness
* Comparative benchmark vs clipping and CBF
* Per-cycle telemetry and JSON session records
* Pure C variant

* * *

## Safety notice

Experimental software. Not certified for safety-critical deployment. Do not useas the only safety mechanism in any system where failure may cause injury,property damage, or regulatory non-compliance. Independent validation andhardware testing are required before any deployment.

* * *

## License

Licensed under the **Apache License, Version 2.0**. See `LICENSE`.

You may use, modify, and ship this in a commercial product without permissionand without payment, under the ordinary Apache-2.0 conditions.

A separate commercial agreement is available for organisations that needwarranty and liability terms, indemnification, certification support, orvalidation on their own plant — things the open license explicitly disclaims.It is never required to use the software. See `COMMERCIAL.md`.

Copyright (c) 2026 Dimitar Kretski.

* * *

## Contact

Dimitar Kretski — kretski1@gmail.comCenter for Hydro- and Aerodynamics, Varna, Bulgaria
