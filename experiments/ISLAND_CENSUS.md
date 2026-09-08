# Island census on the production path (campaign 1A)

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
hB/gB directional producers never set, the disabled M/Mbar hook, the
color-FSM impulses outside the ordinary build, and the old GPU momentum
semantics).

## Costs and long runs

At L=9/W=243 the production path costs about 50 s per light-frame journey
(878 ticks x 177,147 cells) in the current single-threaded CPU build, i.e.
roughly 53 minutes for the 64-frame run (8 breathing cycles).  The run's
outputs are in `build/island_census/run64/` (census.csv, groups.csv,
summary.txt, analysis.txt).  This cost is the main constraint for
higher-L / longer-era campaigns on the CPU path.

