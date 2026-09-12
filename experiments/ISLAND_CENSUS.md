# Island census on the production path (campaign 1A)

Current instrumentation is described in [ATTRACTOR_CENSUS.md](ATTRACTOR_CENSUS.md).
Dangling delegate parents no longer create counted groups, and final-state
population/localization counts exclude disappeared chiefs. The runner also
cross-checks the independent attractor census each frame and writes
`constituents.csv`, `sector_flux.csv`, and `chief_census.csv`. Historical runs
below retain their original measurement conventions.

Harness: `experiments/island_census.cpp`, build `experiments/build_island_census.bat`.
Run from the repository root (outputs isolated per directory):

    build\island_census\island_census.exe [frames] [sieve] [outdir]

Usage example:

    build\island_census\island_census.exe 64 16384 build\island_census\run64

## What it measures

The harness runs the ordinary CPU `simulation()` (reference dynamics; the
`COLOR_ENCOUNTER_FSM` candidate and the reduced `ISLAND_SEED_EXPERIMENT`
branch are both intentionally excluded) on the canonical Platonic superposed
seed L=9, W=243 = (9L)(L/3) = 81 families x 3 copies.  At every completed
light-frame journey (878 ticks at this size) it samples the source centres
and writes:

- `census.csv` — one row per frame: K/D/S/P counts, distinct source centres,
  number of chief groups (`islandChief`), unresolved delegates, mixed-charge
  groups, largest group population/span/family mix, and capture/escape/birth/
  death events relative to the previous frame.
- `groups.csv` — one row per group per frame: population, contributing seed
  families, distinct centres, maximum toroidal Manhattan distance to the
  chief, charge-word count inside the group, whether the named chief is K.
- `summary.txt` — per-chief life-time statistics (first/last frame, population
  and span evolution, longest stable membership run) and a final report card:
  total chiefs, chiefs at the end, groups stable for >= 5 frames, localized
  groups at the end (span <= RMAX, population >= 2), and groups whose final
  population equals L/3.

Invariants verified at every sample: exactly one r2==0 source centre per W
layer at `lcenters[w]`; intrinsic W address preserved on the centre cell;
delegates naming a non-K (or absent) parent are counted as unresolved.

The manuscript's predicted outcome — 9L spatial islands of the form 1K+nD with
n = L/3 - 1, localized and persistent — is a hypothesis this census tests.
Nothing in the harness imposes an island count, a population, or a quota.

## Result of the calibration run (frames=3, sieve=16384)

`build/island_census/run3/` (this run is bit-reproducible; the dynamics are
deterministic and the harness adds no randomness).

| frame | K  | D   | S   | centres | groups | births | notes |
|-------|----|-----|-----|---------|--------|--------|-------|
| 0     | 0  | 0   | 243 | 1       | 0      | 0      | initial superposed seed |
| 1     | 0  | 0   | 243 | 1       | 0      | 0      | first (zero-radius) journey: no interaction |
| 2     | 235| 8   | 0   | 1       | 235    | 235    | D x D promotion cascade in one journey |
| 3     | 235| 8   | 0   | 1       | 235    | 0      | absorbing role configuration |

Group histogram at frames 2 and 3: 227 singleton-K groups (population 1) and
8 K+D groups (population 2).  No group reaches population L/3 = 3.  All 243
source centres remain at the single seed centre (distinct_centers = 1).

### Interpretation (preliminary)

1. With the restored equal-charge D x D promotion (`chiefContact` /
   `promotesDelegate`) live in the production path, one light-frame journey
   turns the superposed seed into a **chief-saturated absorbing state**:
   235 K, 8 D, 0 S, zero unresolved, zero mixed-charge groups.  The reduced
   `ISLAND_SEED_EXPERIMENT` run that previously measured 8 K / 235 D must not
   be used to predict the full-path role census.
2. The superposed geometry is absorbing at the level of roles AND positions:
   by frame 3 no capture, escape, birth or death occurs and the single centre
   never splits.  This reproduces, on the production path, the "rest-state
   coalescence" seen in the reduced color-matching candidate — now with an
   inverted K/D majority.
3. No spatial island can appear before a deterministic first spatial
   asymmetry separates equal-charge groups (the SEED_ASYMMETRY negative:
   the wave equation does not read charge, and polarization cannot
   bootstrap).  The census quantifies the failure at the level of delegation
   structure: 227/243 sources are chiefs with population 1, so the 1K+nD
   island form itself is not produced from this seed by the current rules.

## Repetition and robustness

The engine is deterministic: rerunning the same (frames, sieve) reproduces the
same census bit-for-bit.  Two independent 3-frame runs (calibration and the
head of the 64-frame run) produced identical `census.csv` rows for frames
0-3 (`diff` clean).  Robustness claims should therefore be stated as
invariant checks per frame plus the evolution of the per-chief report across
(long) runs, not as seeded statistics.  Short regression:

    experiments\test_island_census.bat

The harness is the foundation for the next campaigns (sparse prepared seed;
charge-to-amplitude coupling; low-sieve pair channel) so each candidate can
be scored against the same census.

## run64 — final result (64 light-frame journeys, ~53 min wall)

`build/island_census/run64`.  Independent audit
(`python experiments/analyze_island_census.py build/island_census/run64`) plus
`summary.txt`:

- frame 1: first (zero-radius) journey is a no-op (243 S, 1 centre);
- frame 2: D x D promotion cascade -> 235 K / 8 D / 0 S in one journey
  (births = 235);
- frames 3..64: exact absorbing plateau.  Every census row is identical:
  K=235, D=8, S=0, centres=1, groups=235, max_population=2, max_span=0,
  captures=escapes=births=deaths=0.
- All 235 chiefs are present in every frame from 2 to 64 (max stable run = 63
  frames); 227 groups are chief-only (population 1) and 8 groups are K+D
  (population 2); no group ever reaches population L/3 = 3.
- Wall time 3170.9 s total (49.5 s/frame).

Interpretation: over 64 journeys (8 full breathing cycles of 2*RMAX=8 frames)
the production path, from the canonical superposed Platonic seed, never
separates spatially and never forms 1K+nD islands.  It reaches an absorbing
chief-saturated role state (235 K, one centre) after the second journey and
then never changes.  This is the quantitative production-path statement the
reduced ISLAND_SEED and color-matching experiments could not provide: the
9L islands of L/3 constituents do not self-assemble from the superposed seed
under the current production rules, and forming them requires either an
initial spatial asymmetry (prepared seed) or enabling a deterministic
symmetry-breaking mechanism (candidate mechanisms catalogued in the session
log: polarization broadcast dormant behind an impossible precondition, the
homB/gB directional producers never set, the disabled M/Mbar hook, the
color-FSM impulses outside the ordinary build, and the old GPU momentum
semantics).

## Costs and long runs

At L=9/W=243 the production path costs about 50 s per light-frame journey
(878 ticks x 177,147 cells) in the current single-threaded CPU build, i.e.
roughly 53 minutes for the 64-frame run (8 breathing cycles).  The run's
outputs are in `build/island_census/run64/` (census.csv, groups.csv,
summary.txt, analysis.txt).  This cost is the main constraint for higher-L /
longer-era campaigns on the CPU path.

## 20-journey run: the 81-centre signature is reached

`build/island_census_dir/run20` (same harness, `20 16384`; 54.5 s/frame,
1090.5 s total).  Frames 0--6 again reproduce the reference plateau bit-for-bit,
and the harness's own invariant check passes ("census recorded; independent chief
counts agree").

| frame | centres | K | D | groups | unresolved | maxpop | maxspan | captures | escapes |
|-------|---------|---|---|--------|------------|--------|---------|----------|---------|
| 2--6  | 1 | 235 | 8 | 235 | 0 | 2 | 0 | 0 | 0 |
| 7     | 1 | 86 | 157 | 86 | 125 | 3 | 0 | 32 | 157 |
| 8     | 2 | 86 | 157 | 86 | 127 | 3 | 0 | 50 | 52 |
| 9     | 15 | 83 | 160 | 83 | 136 | 3 | 6 | 30 | 39 |
| 10    | 28 | 66 | 177 | 66 | 125 | 4 | 6 | 57 | 46 |
| 11    | 44 | 122 | 121 | 122 | 56 | 3 | 7 | 93 | 24 |
| 12    | 62 | 137 | 106 | 137 | 31 | 3 | 7 | 62 | 37 |
| 13    | 62 | 150 | 93 | 150 | 46 | 5 | 9 | 75 | 90 |
| 14    | 73 | 150 | 93 | 150 | 41 | 3 | 8 | 52 | 47 |
| 15    | 78 | 146 | 97 | 146 | 39 | 4 | 9 | 51 | 49 |
| **16**| **81** | 126 | 117 | 126 | 37 | 4 | 10 | 62 | 60 |
| 17    | 94 | 116 | 127 | 116 | 65 | 6 | 8 | 71 | 99 |
| 18    | 87 | 109 | 134 | 109 | 64 | 6 | 7 | 63 | 62 |
| 19    | 89 | 140 | 103 | 140 | 36 | 4 | 9 | 76 | 48 |
| 20    | 86 | 123 | 120 | 123 | 49 | 4 | 9 | 76 | 89 |

Final report card: `chiefs_at_end=123, localized_at_end=40, pop==L/3_at_end=15`
(the reference run64: `chiefs_at_end=235, localized_at_end=8, pop==L/3_at_end=0`).

Reading: the number of distinct centres grows monotonically -- 1, 1, 1, 2, 15,
28, 44, 62, 62, 73, 78, **81**, 94, 87, 89, 86 -- and passes through the predicted
island count exactly at journey 16 (9L = 81 families).  The maximum toroidal span
grows from 0 to 10 cells, so the constituents of a group are now spatially
extended (the reference kept span = 0 across all 64 journeys), and the number of
groups at the predicted population `L/3` rises from 0 to 15.  The count then
fluctuates in the 86--94 band: the islands are not a stable 81-crystal, and
`max_families` reaches 3--4, so families do mix at later times.
Scored as a candidate mechanism (C): the channel demonstrably supplies the
deterministic spatial asymmetry whose absence the SEED_ASYMMETRY negative and the
64-journey reference plateau had established, and the predicted island count
appears as a transient.  Stabilising it -- and making the directionality
family-selective rather than global -- is the remaining problem.


Harness: the same `island_census.cpp`, rebuilt with the WP8 candidate channel
(`POLAR_BOOTSTRAP_ADDRESS`, `POLAR_BROADCAST_WAVE`, `HOMB_PRODUCER_FSM`,
`HOMB_CONSUMER_TRANSPORT`, `POLAR_MAGNITUDE_FSM`) by
`experiments/build_island_census_dir.bat` into
`build/island_census_dir/island_census_dir.exe`; run as
`build\island_census_dir\island_census_dir.exe 8 16384 build\island_census_dir\run8`
(53.5 s/frame, 428.2 s total).  Candidate macros only: this is not a measurement
of the reference dynamics.

| frame | K | D | centres | groups | unresolved | maxpop | captures | escapes | births | deaths |
|---|---|---|---|---|---|---|---|---|---|---|
| 0--1 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| 2 | 235 | 8 | 1 | 235 | 0 | 2 | 243 | 0 | 235 | 0 |
| **3--6** | **235** | **8** | **1** | **235** | **0** | **2** | **0** | **0** | **0** | **0** |
| 7 | 86 | 157 | 1 | 86 | 125 | 3 | 32 | 157 | 149 | 0 |
| 8 | 86 | 157 | **2** | 86 | 127 | 3 | 50 | 52 | 24 | 24 |

Frames 0--6 are bit-identical to the reference plateau documented above (the
`run64` rows for frames 3 onward are all identical to frame 3); the plateau
breaks at journey 7, i.e. exactly when the broadcast election stamp becomes live
at the source centre (`bstamp = 3514`, `pol = (4,0)`, so `pB` is set under the
magnitude convention).  Final-frame histogram: **population 1: 62 groups,
population 2: 18, population 3: 6**, i.e. six groups reach the target population
`L/3 = 3` (the reference never reaches it in 64 journeys), and all 86 groups are
localised (span <= RMAX = 4).  `max_families` rises from 1 to 2, so families do
not mix appreciably, and `distinct_centers` goes 1 -> 2: the seed centre splits
once, which is the first spatial separation ever produced on this production path.

Interpretation: the candidate channel does what the SEED_ASYMMETRY negative said
was missing -- it supplies a deterministic spatial asymmetry that dissolves the
absorbing chief-saturated state (K 235 -> 86, groups 235 -> 86), produces groups
at the predicted population `L/3`, and splits the centre.  It does **not** reach
the 81-distinct-centres acceptance signature: the bifurcation is global (1 -> 2
centres) rather than per-family, consistent with the transport moving whole W
layers coherently.  Scoring this as a candidate mechanism (C), the remaining
requirement is family-selective directionality (the W-address tie-break branch),
not more time.
