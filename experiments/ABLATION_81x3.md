# Ablation table: the 81 x 3 island quantisation

Interpretation: EXCLUSION explicitly preserves seed-family boundaries. Its
81 x 3 result is a family-preserving control, not evidence for spontaneous
quantization when equal-charge sources from different W families may share
a chief. The corrected measurement conventions are in [ATTRACTOR_CENSUS.md](ATTRACTOR_CENSUS.md).

Consolidated from the runs under `build/lightmatter_decouple/` and
`build/island_census/` (census harness).  All runs: seed 1, canonical
charge words, tube/cube as indicated, ordinary seeds unless stated.
Columns: **final stable census** and **# islands of population exactly 3**
(`size3`), unresolved, occupied centres.

## A. Duo test (two equal-charge families at d=2 / d=4, tube 21x5x5, W=6)

| Mechanism | d | K / D | islands of 3 | unresolved | centres | outcome |
|---|---|---|---|---|---|---|
| colour+matched+decouple (no rule) | 2 | 1 / 5 | 0 | 0 | 5 | **merge** (1K+5D) from frame 2 |
| colour+matched+decouple (no rule) | 4 | 1 / 5 | 0 | 0 | 3-6 | merge by frame 4 (transient 2x3 at f2) |
| + EXCLUSION (identity only) | 2 | 2 / 4 | 2 | 0 | 2 | **2 islands of 3, stable 16 f** |
| + EXCLUSION (identity only) | 4 | 2 / 4 | 2 | 0 | 2 | 2 islands of 3, stable 16 f |
| + EXCLUSION + push | 4 | 2 / 4 | 2 | 0 | 4-5 | 2 islands of 3, stable 16 f |

Runs: `duo2_seed_1`, `duo4_seed_1` (no rule); `xd2_seed_1`, `xd4_seed_1`
(EXCLUSION); `pd4_seed_1` (EXCLUSION+push).

## B. Superposed Platonic cube (L=9, W=243, all copies coincident)

| Mechanism | K / D | islands of 3 | unresolved | centres | outcome |
|---|---|---|---|---|---|
| colour+matched+decouple (no rule) | 8 / 235 | 0 | 0 | ~50-90 | 8 charge-word tribes, no 3-islands |
| + EXCLUSION (identity only) | 81 / 162 | **81** | 0 | 1-9 | 81 x 3 (co-located) |
| + EXCLUSION + push | 81 / 162 | **81** | 0 | 63..167 | 81 x 3, spatially distributed |
| production path (no rule) | 235 / 8 | 0 | 0 | 1 | absorbing plateau |
| production path + EXCLUSION+push | 162 / 81 | 0 | 0 | 15-19 | 81 family clusters, 2K+1D each |

Runs: `basic_seed_1`, `exc_cube_seed_1`, `exc_cube2_seed_1` (colour FSM);
`run64` and `prod_exc` (production, island_census).

## C. Full-scale same-charge contact (spread81, 81 families charge-major)

| Mechanism | frames | K / D | islands of 3 | unresolved | centres | outcome |
|---|---|---|---|---|---|---|
| sparse layout, same charge OUT of reach (no rule) | 2..4 | 81 / 162 | 81 | 0 | 81-204 | geometry-guaranteed islands |
| spread81 (same charge adjacent), no rule | 2..4 | 8 / 235 | 57->9 | 171->27 | 81-235 | **merging** into 8 tribes (27/copy at f4) |
| spread81 + EXCLUSION + push | 2..6 | 81 / 162 | **81** | 0 | 170-201 | 81 x 3 survives contact, distributed |
| spread81 + EXCLUSION + push (12-frame stability run, `sp81xL`) | 2..12 | 81 / 162 | **81** | 0 | 170-215 | 81 x 3 stable across 12 frames |

Runs: `sp81_seed_1` (sparse81), `s81t_seed_1` (spread81 no rule),
`sp81x_seed_1` (spread81 + EXCLUSION + push).

## Reading

- **EXCLUSION** (identity hard core across seed families) is necessary and
  sufficient to stop the equal-charge merge (A, B, C).
- **Push** adds spatial distribution without breaking the quantisation
  (B centres 1-9 -> 63-167; C ~200).
- **Without any rule**, equal-charge contact merges families into the 8
  charge-word tribes; the only rule-free way to 81 x 3 is the sparse layout
  where same-charge families never touch (geometry-guaranteed).
- **Production path**: EXCLUSION+push removes the cross-family tribes and
  keeps 81 family clusters with unresolved = 0, but the production D x D
  promotion gives 2 K + 1 D per family (not 1 K + 2 D) and the push yields
  only ~15-19 centres (see LIGHTMATTER_DECOUPLE.md "production-path port").

## Reproduce

- Colour FSM variants: `build\lightmatter_decouple\lightmatter_decouple{_basic,_exc}.exe <seed> <frames> <tag> <mode>`
  with modes `cube`, `duo2`, `duo4`, `sparse81`, `spread81`.
- Production variants: `build\island_census\island_census{,_exc}.exe <frames> <sieve> <outdir>`.
