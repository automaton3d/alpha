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

## WP3 reproduction status (11 Sep 2026)

WP3 re-checked the ablation against the current tree.  The **duo** rows were
re-run fresh this session (seed 1, 16 frames, tube 21x5x5, W=6) and reproduce
the documented runs exactly:

| case | build | final census | matches |
|---|---|---|---|
| duo2 (d=2, no rule) | basic | K=1 D=5 (merge) | `duo2_seed_1` |
| duo2 (d=2, +EXCLUSION) | exc | K=2 D=4, size3=2 | `xd2_seed_1` |
| duo4 (d=4, no rule) | basic | K=1 D=5 (merge) | `duo4_seed_1` |
| duo4 (d=4, +EXCLUSION) | exc | K=2 D=4, size3=2 | `pd4_seed_1` |

The **cube** (L=9, W=243) and **spread81** (LX=85) rows are NOT re-run here: the
colour-FSM cube costs ~25 s/frame (the 177 147-cell allocation plus the
per-layer init dominates), beyond the interactive budget.  Their evidence is the
kept traces under `build\lightmatter_decouple\`; final rows
(`frame,K,D,S,centers,chief_centers,groups_of_three,unresolved,decouple_pairs`):

| run | frame | K | D | centres | chief centres | size3 | unresolved |
|---|---|---|---|---|---|---|---|
| `exc_cube_seed_1` | 12 | 81 | 162 | 3 | 2 | **81** | **0** |
| `exc_cube2_seed_1` | 12 | 81 | 162 | 167 | 61 | **81** | **0** |
| `sp81x_seed_1` | 6 | 81 | 162 | 198 | 75 | **81** | **0** |
| `sp81xL_seed_1` | 12 | 81 | 162 | 209 | 78 | **81** | **0** |
| `s81t_seed_1` (no rule) | 4 | 8 | 235 | 210 | 8 | 9 | 27 |
| `sp81_seed_1` (sparse, no rule) | 4 | 81 | 162 | 201 | 81 | **81** | **0** |

Reading: with EXCLUSION the 81 x 3 survives at full scale under same-charge
contact (sp81x/sp81xL, 6-12 frames, unresolved 0); without it the same-charge
families merge (s81t: K 81 -> 8, 27 unresolved).  The cube gives 81 x 3 either
co-located (`exc_cube`, 3 centres) or spatially distributed (`exc_cube2`, 167).

Reproduce the heavy rows in long batches (~25 s+/frame):

    build\lightmatter_decouple\lightmatter_decouple_exc.exe   1 12 exc_cube cube
    build\lightmatter_decouple\lightmatter_decouple_exc.exe   1 12 sp81x    spread81
    build\lightmatter_decouple\lightmatter_decouple_basic.exe 1 12 s81t     spread81

Note: the harness binaries date from the colour-FSM path, which REPLACES
`update_lattice_cpu` (`COLOR_ENCOUNTER_FSM`, simulation.cpp), so the ordinary
`encounter()` is not on this path; `color_fsm.inc` is unchanged since the build,
so the binaries are behaviourally current.

### Production-path rows (documented traces)

`build\island_census\{run64,prod_exc}\census.csv`
(`frame,tick,K,D,S,P,distinct_centers,groups,unresolved,...`):

| run | frame | K | D | distinct centres | groups | max population |
|---|---|---|---|---|---|---|
| `run64` (production, no rule) | 64 | 235 | 8 | 1 | 235 | 2 |
| `prod_exc` (production + EXCLUSION+push) | 12 | 162 | 81 | 17 | 162 | 2 |

Reading: from the superposed seed the production path reaches an absorbing
plateau (K=235, D=8, one centre, 235 groups, no births/deaths after frame 2).
EXCLUSION+push removes the cross-family tribes and keeps 81 family clusters
(unresolved 0), but the production D x D promotion still yields **2 K + 1 D** per
family, not 1 K + 2 D -- that fix is WP3.3 and remains open.

## WP3.3 -- production 1 K + 2 D fix (11 Sep 2026)

**Root cause.** The production election (`chiefContact`, `chief_transition.h`) is
a pairwise minimum-address tournament: for one family (3 copies at w, w+1, w+2)
the contact between the two delegates (w+1, w+2) elects `w+1` as its own chief
while `w` is already chief -> **2 K + 1 D** per family.  Measured: `prod_exc`
= 162 K / 81 D (= 2 K + 1 D per family).

**Candidate fix `DD_INTRA_ISLAND_FIX`** (`chief_transition.h`): a same-family
S x S contact elects the **family-minimum** address (`w / ISLAND_SIZE *
ISLAND_SIZE`) as chief, so a 3-copy family elects exactly one chief.  Cross-family
contacts keep the minimum-address rule.  Macro-guarded; OFF in the reference
build (which stays 6468/0/0).

**Minimal production-path test** (`experiments/promotion_three.cpp`, build
`experiments/build_promotion_three.bat`): three equal-charge sources (one family
of 3) start coincident in a 21x5x5 tube; `ISLAND_SIZE` is set to 3 to match the
L=9 structure.

| variant | final | verdict |
|---|---|---|
| reference (macro off) | K=2 D=1 S=0 | 2 K + 1 D (over-promoted) |
| `DD_INTRA_ISLAND_FIX` | K=1 D=2 S=0 | **1 K + 2 D (island)** |

The fix is validated at the rule level.  Still open: re-running the full
production census (`island_census` at L=9/W=243, ~50 s/frame) with
`-D DD_INTRA_ISLAND_FIX` together with EXCLUSION to confirm 81 K + 162 D at full
scale (heavy).

## WP3.2 -- spread81 membership and extent distribution (11 Sep 2026)

Computed from the kept `sp81xL_seed_1/groups.csv` -- **no new run**; columns
`frame,chief,population,families,centers,max_distance_to_chief`:

| frame | groups | population == 3 | `max_distance_to_chief` |
|---|---|---|---|
| 2 | 81 | **81 (100%)** | all 6 (uniform) |
| 6 | 81 | **81 (100%)** | 0 .. 12 |
| 12 | 81 | **81 (100%)** | 0 .. 22 |

Membership is a **delta at population 3** for every one of the 81 islands at
every sampled frame (frames 2-12): the family-preserving quantisation is exact.
Every island keeps a single contributing family (`families == 1`) and unresolved
delegates are zero.  The spatial extent (toroidal Manhattan distance to the
chief) starts uniform (all 6) and broadens to 0..22 by frame 12: the push spreads
the islands **without changing membership** -- the quantisation and the spatial
distribution are decoupled controls.
