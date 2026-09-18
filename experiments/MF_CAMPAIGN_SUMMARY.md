# Multifrequency Mechanics Campaign Summary

**Date**: 2026-09-18
**Status**: All experiments passed

---

## Overview

The multifrequency mechanics implementation introduces two candidate rules for handling
multi-frequency populations in the cellular automaton:

1. **`PAIR_STACK_ABSORB_FSM`** - Allows free S+S formations to absorb identical co-located
   stacks, increasing `pair_count` dynamically
2. **`MULTIFREQ_RAY_FSM`** - Implements ray detection for frequency-bearing sources, gating
   the pB/sB channels based on detection bit status

---

## Implementation Details

### PAIR_STACK_ABSORB_FSM (interaction.cpp)

- **Location**: `absorbCoLocatedStacks()` function (lines 253-280)
- **Trigger**: When a fresh S+S pair forms (`enc_pair >= 1`)
- **Behavior**: Counts identical co-located stacks at the formation site and absorbs them
  into the new pair's `pair_count` (newCount = 1 + absorbed)
- **Idempotence Guard**: Prevents double-counting from bidirectional encounter evaluation

### MULTIFREQ_RAY_FSM (interaction.cpp)

- **Location**: `mfFreqBearing()`, `mfComputeRayDetect()`, `mfGatedFlag()` functions
- **Ray Detection**: Uses pure-climb DDA along the polarization walker's orbital budget
- **Effective Tick**: `t_eff = 2 * pair_count` (clamped to RMAX)
- **Detection**: Checks for active cell with `s2B` flag or non-zero `u` amplitude
- **Gating**: Frequency-bearing sides whose detection bit didn't fire contribute no pB/sB

---

## Experimental Results

### Test Configuration 1: Default (EL=7, Frames=6, Sieve=1)

| Probe Variant                          | enc_pair | enc_absorb | mf_reads | mf_hits | Result |
|----------------------------------------|----------|------------|----------|---------|--------|
| PAIR_STACK_ABSORB_FSM ON               | 27       | 1          | 0        | 0       | ✅ ALL OK |
| Reference control (no macros)          | 18       | 0          | 0        | 0       | ✅ ALL OK |
| MULTIFREQ_RAY_FSM ON                   | 18       | 0          | 1616     | 1564    | ✅ ALL OK |
| Combined (both macros ON)              | 27       | 1          | 3868     | 1260    | ✅ ALL OK |

### Test Configuration 2: Extended (EL=9, Frames=8, Sieve=2)

| Probe Variant                          | enc_pair | enc_absorb | mf_reads | mf_hits | Result |
|----------------------------------------|----------|------------|----------|---------|--------|
| PAIR_STACK_ABSORB_FSM ON               | 23       | 1          | 0        | 0       | ✅ ALL OK |
| Combined (both macros ON)              | 23       | 1          | 6356     | 2404    | ✅ ALL OK |

---

## Key Observations

1. **Stack absorption works correctly**: When `PAIR_STACK_ABSORB_FSM` is enabled, the formed
   pair absorbs the seeded stack, yielding `pair_count = 5` (1 + 4) as expected.

2. **Ray detection is active**: The `MULTIFREQ_RAY_FSM` probe shows significant ray reads
   and hits, confirming the detection mechanics are functioning.

3. **Combined mode is stable**: Both mechanisms work together without conflicts.

4. **Reference control is unaffected**: Without macros, the reference behavior is preserved.

5. **Decay mechanics**: The absorbed population (5) correctly decays through the model's
   own turnaround consumption, demonstrating the multi-frequency lifetime semantics.

---

## Files Created/Modified

- `experiments/build_mf_probe.bat` - Build script for multifrequency probes
- `experiments/run_mf_campaign.bat` - Campaign runner script
- `experiments/MF_CAMPAIGN_SUMMARY.md` - This summary
- `experiments/mf_probe_ray.exe` - Ray detection probe binary
- `experiments/mf_probe_combined.exe` - Combined probe binary
- `experiments/stack_absorb_probe.exe` - Stack absorption probe binary
- `experiments/stack_absorb_probe_ref.exe` - Reference control binary

---

## Next Steps

1. Extend the probe to test more complex multi-stack scenarios
2. Add tests with different charge word combinations
3. Validate ray detection with non-zero orbital budgets (m ≠ 0)
4. Test with larger lattice sizes for performance scaling analysis