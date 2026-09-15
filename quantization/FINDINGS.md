# Findings: the flux harness wired to real data (14 Sep 2026)

The harness in this directory tests the manuscript's attractor hypothesis
`dN = Gamma_cap(N) - Gamma_esc(N)` from **per-event fluxes** instead of population
levels.  This note records the first time it was run against data produced by the
simulator, so the next reader does not have to redo the wiring.

## Two blockers, both fixed here

1. **numpy/pandas are not installed in this environment**, so `quantize.py` cannot
   run as-is.  `quantize_stdlib.py` is a stdlib port of its five sections using the
   same estimators (OLS drift, percentile bootstrap over islands, AR(1) residual
   check, within-island permutation surrogates).  `quantize.py` remains the
   reference implementation; the two must agree where numpy/pandas exist.
2. **The flux data already exists but was not wired to the harness.**  Every census
   runner writes `constituents.csv` with
   `frame,chief_w,constituents,captures,escapes` -- one row per island per frame
   with the EXACT captures/escapes measured from consecutive membership snapshots.
   `flux_from_census.py` converts it to the harness format
   (`run,island,frame,N,captures,escapes`) with `N = constituents + 1`.  The
   "last resort" `--derive-fluxes` path is therefore never needed on real data.

Validated with stdlib twins of the author's synthetic controls
(`make_synth_stdlib.py`): the mean-reverting scenario is recovered as
**N\* = 30.38, 95% CI [29.87, 30.91], surrogate p = 0.005 -> PASS**, and the
fixed-partition scenario correctly returns **INCONCLUSIVE (zero escapes)**.

## Result: six real datasets, no attractor anywhere

    python flux_from_census.py ref_L9=build/census_corrected > analysis/ref_L9.csv
    python flux_from_census.py ref_L15=build/island_census/L15 > analysis/ref_L15.csv
    python flux_from_census.py cand_frozen=build/island_census/prod_exc_fix > analysis/cand_frozen.csv
    python flux_from_census.py candADDR_L9=build/island_census_addr/run20 > analysis/candADDR_L9.csv
    python flux_from_census.py candDIR_L9_20=build/island_census_dir/run20 > analysis/cand_turnover.csv
    python flux_from_census.py candDIR_L9_64=build/island_census_dir/run64 > analysis/candDIR_L9_64.csv
    python quantize_stdlib.py analysis/*.csv --by-run --out analysis/report_all.txt

| run (build) | L | frames | sum cap | sum esc | b | N\* | verdict |
|---|---|---|---|---|---|---|---|
| `ref_L9` (reference) | 9 | 4 | 243 | **0** | +0.33 | none | INCONCLUSIVE (zero escapes) |
| `ref_L15` (reference) | 15 | 4 | 675 | **0** | +0.33 | none | INCONCLUSIVE (zero escapes) |
| `cand_frozen` (EXCLUSION+`DD_INTRA_ISLAND_FIX`) | 9 | 12 | 243 | **0** | +0.09 | none | INCONCLUSIVE (zero escapes) |
| `candADDR_L9` (`ADDRESS_TARGET_FSM`) | 9 | 20 | 243 | **0** | +0.05 | none | INCONCLUSIVE (zero escapes) |
| `candDIR_L9_20` (directional channel) | 9 | 20 | 1093 | 899 | **+0.494** | none | **FAIL** (anti-restorative) |
| `candDIR_L9_64` (directional channel) | 9 | 64 | 3903 | 3727 | **+0.495** | none | **FAIL** (anti-restorative) |

Two conclusions.

- **Four builds are frozen in the harness's own sense** (zero escapes, every
  island): the reference at both lattice sides, the `EXCLUSION` candidate and the
  address-walk candidate.  The harness reports exactly the failure mode it was
  built to detect, and this is the membership theorem's corner reached by an
  independent statistical instrument (`DYNAMIC_QUANTIZATION_DERIVATION.md`).
- **Where turnover exists, the drift is anti-restorative.**  For the directional
  channel the fitted slope is `b = +0.494` (20 frames) and `b = +0.495` (64
  frames) -- stable across run length -- and the binned curve rises monotonically
  with population (`N=1: -0.42, N=2: +0.15, N=3: +0.64, N=4: +1.00, N=5: +1.10,
  N=6: +1.46`): larger islands grow faster.  There is no `N*` to report, so the
  population attractor is not merely unmeasured -- it is **falsified on the only
  data where it could have been measured**, and the candidate's aggregation is a
  runaway rather than an equilibrating process (consistent with its census drift,
  chiefs 86 -> 150 and `max_population` 3 -> 6 with no stationarity).

## What the harness cannot do, and what would make it decision-grade

- It cannot create turnover where the rules produce none: with zero escapes,
  `Gamma_esc == 0` by construction and no estimator can extract an `N*` (the
  verdict says so rather than reporting a spurious one).
- Its per-run mode (`--by-run`) exists precisely to read `N*` against the lattice
  side, which is what a quantisation claim needs; but with no `N*` at either
  `L = 9` or `L = 15`, the scaling question is moot.
- To become decision-grade the harness needs a configuration with **turnover and a
  stationary regime**: the prepared-island configurations (inertia tubes, placed
  seeds) rather than the superposed seed, plus an `L`-sweep at fixed rule set.
  And per `DYNAMIC_QUANTIZATION_DERIVATION.md` the claim to test must be that
  `N*` tracks `L/3` **with no rule containing `L/3`** -- which the reference
  membership transitions rule out in advance, so a candidate build that reads
  `L/3` can only confirm its own axiom.
