# RESULTS v2 -- reproduced reference measurements (WP2)

Status: WP2 DONE.  Last updated: 11 Sep 2026.
Reference: `MODEL_VERSION` model-ref-v1 (`doc/REFERENCE_CONFIG.md`), CPU, all
candidate macros OFF; model fingerprint `29ef0e...` (post the macro-guarded
WP4.2 addition; reference behaviour re-verified identical).

This file is the single reproducible home for the quantitative numbers the
manuscript cites.  It records the current-HEAD values and supersedes the
"historical" tables where it overlaps them; the older campaign narrative stays
in `RESULTS.md` for provenance.

## Protocol

```bat
experiments\build_probe.bat
experiments\alpha_probe.exe EL SEP FRAMES SIEVE budget
```

Reference sweep: EL in {7, 9, 11, 13}, SEP = 4, SIEVE = 16384, budget = 256,
FRAMES = 200 (100 for EL >= 13).  Reproducible driver:
`experiments/run_wp2_sweep.ps1` (writes per-run logs and `alpha_A_sweep.csv`
under `build\wp2\`).

## Determinism (why there are no "seeds")

The reference path contains no RNG.  Two identical L=7 200-frame runs are
byte-identical in every scientific value (ticks = 23600, active-passes = 6468,
alpha_A = 0.003756878); the ONLY difference is the printed wall-clock line
(`ran 200 light frames in 2.88s` vs `2.87s`).  Error bars therefore cannot come
from seeds; the meaningful dispersion is over the swept sieve modulus S.

## alpha_A L-sweep at the reference sieve (S = 16384)

| L | frames | active passes | s2B passes | pairs | alpha_A | 1/alpha_A |
|---|---|---|---|---|---|---|
| 7 | 200 | 6468 | 0 | 0 | 0.003756878 | 266.18 |
| 9 | 200 | 7700 | 0 | 0 | 0.003739413 | 267.42 |
| 11 | 200 | 12382 | 32 | 32 | 0.009334646 | 107.13 |
| 13 | 100 | 10520 | 80 | 80 | 0.009865940 | 101.36 |

Reproduced at current HEAD (matches `RESULTS.md` "current-HEAD re-validation"
exactly).  alpha_A is strongly L-dependent (1/alpha_A = 266 -> 101): the
candidate **FAILS** by the pre-registered criterion.  No convergence to
1/137.035999177.

## S-scan at current HEAD (L=7, 200 frames) -- the dispersion comes from S

Driver: `run_wp2_sweep.ps1 -SkipL -Fresh -SList ...` (appends to
`build\wp2\S_scan_L7.csv`); analysis: `analyze_wp2_scan.ps1`.  Canonical
32-point comb (S = 128 + 512k, k = 0..31), realized throughput calls/s2B:

| S | calls | s2B | calls/s2B | | S | calls | s2B | calls/s2B |
|---|---|---|---|---|---|---|---|---|
| 128 | 12864 | 1800 | 7.15 | | 8320 | 8940 | 42 | 212.86 |
| 640 | 13998 | 496 | 28.22 | | 8832 | 7308 | 8 | 913.50 |
| 1152 | 12140 | 228 | 53.25 | | 9344 | 6468 | 0 | inf |
| 1664 | 10766 | 170 | 63.33 | | 9856 | 7374 | 24 | 307.25 |
| 2176 | 10686 | 148 | 72.20 | | 10368 | 13660 | 26 | 525.38 |
| 2688 | 8942 | 42 | 212.90 | | 10880 | 6468 | 0 | inf |
| 3200 | 6860 | 24 | 285.83 | | 11392 | 6468 | 0 | inf |
| 3712 | 6468 | 0 | inf | | 11904 | 7308 | 8 | 913.50 |
| 4224 | 12682 | 48 | 264.21 | | 12416 | 7310 | 2 | 3655.0 |
| 4736 | 9346 | 26 | 359.46 | | 12928 | 6468 | 0 | inf |
| 5248 | 15432 | 94 | 164.17 | | 13440 | 6468 | 0 | inf |
| 5760 | 7022 | 2 | 3511.0 | | 13952 | 6468 | 0 | inf |
| 6272 | 9828 | 68 | 144.53 | | 14464 | 6468 | 0 | inf |
| 6784 | 6468 | 0 | inf | | 14976 | 6468 | 0 | inf |
| 7296 | 8588 | 58 | 148.07 | | 15488 | 6856 | 18 | 380.89 |
| 7808 | 6468 | 0 | inf | | 16000 | 6468 | 0 | inf |

Summary (`analyze_wp2_scan.ps1`): 32 points, **20 open / 12 closed** (the gate
shuts in the high-S tail).  Open ratios: min 7.15, **median 238.6**, mean 611.1,
max 3655.

Sampling sensitivity: the coarse 15-point sample of the previous draft gave
median 108.4 / mean 129.6; the full comb gives median 238.6 / mean 611.1.  The
statistic is dominated by the sparse high-S points (S=5760 and S=12416 carry 2
passes each), so even the "dial-free" S-average is not a well-behaved observable
at this run length -- reinforcing the campaign's conclusion.

Reading: the realized coupling is a **non-monotone resonance comb** set by S and
it **closes** above S ~ 7-8k.  1/mean = 1/129.6 sits in the neighbourhood of
1/137 but is not the constant, and it depends on which S values are sampled (the
open/closed split) and on the run length (`RESULTS.md`).  This is the current-HEAD
reproduction of the campaign's dial-free finding: no single S is physical, and
the S-averaged statistic is not stationary -- the dial cannot be tuned away, only
averaged or derived structurally.

Note: these current-HEAD values **differ** from the historical S-scan table in
`RESULTS.md` (e.g. S=128 read 1/1.82 there vs 1/7.15 now); that table predates
the inertia rework and is exactly why WP2 exists.

### L-stability of the comb (same S, L=9 vs L=7)

| S | calls / s2B at L=7 | calls / s2B at L=9 |
|---|---|---|
| 128 | 7.15 | 9.36 |
| 1152 | 53.25 | 92.86 |
| 5248 | 164.17 | 1081.2 |

Same S, different lattice: the ratio changes by up to ~6.6x (S=5248).  The
realized coupling is **lattice-size specific**, matching `RESULTS.md`.  A
dial-free number would need an S-averaged statistic that is stationary across L,
which is not observed.

## alpha_B .. alpha_F at current HEAD

| candidate | definition | current-HEAD value | verdict |
|---|---|---|---|
| alpha_B | realized gate throughput s2B / calls | L=7 : 0 (closed); L=9 : 0; L=11 : 1/387; L=13 : 1/132 | **FAILS** (reduces to the sieve gate, not a force) |
| alpha_C | Wyler-on-lattice (analytic) | 137.03608 (6 digits) | **FAILS** (analytic; no lattice derivation) |
| alpha_D | <sB>/<pB> | 0 (nPol = 0; broadcast dormant) | **FAILS** (unobservable) |
| alpha_E | <pol_v^2>/<pol_u^2> | -> 2 exactly (analytic) | **FAILS** |
| alpha_F | sieve fixed point S* = <u> | L=7 : S* = 41 -> alpha_F ~ 50 | **FAILS** (L-dependent) |

Definitions and the full campaign are in `RESULTS.md`.  The L=13 alpha_B value
(1/132) lands near 1/137 by coincidence -- L=11 gives 1/387 -- which is exactly
the resonance-comb numerology the campaign warns against.

## Pending (WP2 / WP3)

- S-scan at L=9 for the L-stability check (driver: `-ScanL 9 -SList ...`); the
  full 32-point step-512 comb at L=7 is available via the same driver.
- WP3: the single 81 x 3 ablation table, spread81 stability, the production
  1 K + 2 D fix, and the postulate-vs-emergent control.
