# Sieve-modulus sweep on the production census (item A: is L/3 dynamical?)

Working document for the experiment that attacks the **circularity** of the
`81 x 3` result: every candidate that produces the quantised island
(`EXCLUSION_FSM`, `DD_INTRA_ISLAND_FIX`, `ISLAND_ALIGNED_W_ROTATION`,
`ADDRESS_TARGET_FSM`) contains `ISLAND_SIZE = L/3` in its rule, so the quantum
is an input of the election, not a dynamical output.  The test here varies the
**only free knob of the reference rule set that can rearrange the role census**
-- the electroweak sieve modulus `S` of Eq. (sieve), `s2B = ((u*t) mod S) < u`
-- and asks whether the reference dynamics then reaches an island population of
`L/3` on its own.

Instrument: the ORDINARY reference build (no candidate macro) of the production
census harness, canonical superposed Platonic seed, `L = 9`, `W = 243`,
`RMAX = 4`, `FRAME = 878` ticks:

    experiments\build_island_census.bat
    build\island_census\island_census.exe 6 <S> build/island_census/sweep_S<S>

`U0 = 2048` is a literal of the seed (`initSim.cpp:96`) and is not a CLI
parameter; by Eq. (sieve-prob) the gate depends on the ratio `u/S`, so lowering
`S` is the exact equivalent of raising `U0` and no model source had to be
touched for this sweep.  The reader is `experiments/analyze_sieve_sweep.py`.

## Result: two regimes, and neither reaches L/3

Frame-2 row of `census.csv` (the first interaction journey -- the same journey
in which the reference plateau forms).  Columns: K, D, S, P, groups,
unresolved, mixed charge, max population, distinct centres.

| sieve S | K | D | singletons S | P (pair halves) | groups | unresolved | mixed | max pop | centres |
|---|---|---|---|---|---|---|---|---|---|
| 16384 (reference) | 235 | 8 | 0 | 0 | 235 | 0 | 0 | **2** | 1 |
| 8192 | 235 | 8 | 0 | 0 | 235 | 0 | 0 | **2** | 1 |
| 4096 | 145 | 5 | 0 | **93** | 145 | 0 | 0 | **2** | 1 |
| 1024 | 145 | 5 | 0 | **93** | 145 | 0 | 0 | **2** | 1 |
| 256 | 145 | 5 | 0 | **93** | 145 | 0 | 0 | **2** | 1 |
| 64 | 145 | 5 | 0 | **93** | 145 | 0 | 0 | **2** | 1 |

Reading:

- The sieve is **not inert**: at `S <= 4096` the electroweak pair channel opens
  (P = 93 source halves) and the chief census collapses from `235 K / 8 D` to
  `145 K / 5 D`.  Arithmetic closes exactly: `145 + 5 + 93 = 243 = W`.
- The switch is sharp and consistent with `P(u,S) ~ u/S`: `S = 8192` is
  indistinguishable from the reference (gate effectively shut) and every
  `S <= 4096` gives the identical census (gate saturated).
- **In both regimes the largest group has population 2.**  No group reaches
  `L/3 = 3`, and `distinct_centers` stays 1: the channel reshuffles roles and
  binds pairs, but it neither quantises the population nor breaks the spatial
  symmetry (the `SEED_ASYMMETRY.md` negative) that a `1K+nD` island would
  require.
- Consequence for the main claim: the `81 x 3` quantisation is **not** produced
  by the electroweak/sieve channel; in the reference rule set the population
  saturates at 2 (a K with one D or one bound pair).  The candidates that do
  reach `L/3` carry `L/3` in their rules.

## Stability: the census freezes at frame 2 in both regimes

| run | frames observed | census from frame 2 on | events after frame 2 |
|---|---|---|---|
| S = 16384 (reference) | 2, 3, 4 identical | 235 K / 8 D / 0 S / 0 P, groups 235, max pop 2, 1 centre | 0 |
| S = 4096 | 2, 3 identical | 145 K / 5 D / 0 S / 93 P, groups 145, max pop 2, 1 centre | 0 |
| S = 64 | 2..12 identical | 145 K / 5 D / 0 S / 93 P, groups 145, max pop 2, 1 centre | 0 |
| S = 8192 | 2, 3 (stopped) | as the reference regime | 0 |
| S = 1024, 256 | 2, 3 (stopped) | as the low-S regime | 0 |

`captures = escapes = births = deaths = 0` from frame 3 onward in every run, so
from the second journey the census observables are frozen: the configuration is
absorbing **with a maximum population of 2**, in both regimes.  The intermediate
runs (`8192`, `1024`, `256`) were stopped on a CPU budget after their frame-3
row was recorded; their frame-2 rows are in the table above and agree with their
regime (their partial CSVs remain under `build/island_census/sweep_S*`).

## Reproduce

```bat
rem the sweep (each run needs its output directory to exist)
experiments\build_island_census.bat
mkdir build\island_census\sweep_S64
build\island_census\island_census.exe 6 64 build/island_census/sweep_S64
rem ... and likewise for 16384, 8192, 4096, 1024, 256

rem summarise every sweep_S* directory found
python experiments\analyze_sieve_sweep.py build\island_census
```

Analyzer output for the runs recorded here (report-card columns are `?` where
the run was stopped before completion, so no `summary.txt` was written; the
`maxpop seen` column is the maximum `max_population` over the interacting
frames -- 2 in every regime):

```
S        frames f2(K/D/g/unres/mixed/maxpop/cent) last(K/D/g/maxpop/cent)  maxpop seen pop==L/3  chiefs_end wall_s
16384    4      235/8/235/0/0/2/1            235/8/235/2/1            2          ?         ?          ?
8192     3      235/8/235/0/0/2/1            235/8/235/2/1            2          ?         ?          ?
4096     4      145/5/145/0/0/2/1            145/5/145/2/1            2          ?         ?          ?
1024     3      145/5/145/0/0/2/1            145/5/145/2/1            2          ?         ?          ?
256      3      145/5/145/0/0/2/1            145/5/145/2/1            2          ?         ?          ?
64       4      145/5/145/0/0/2/1            145/5/145/2/1            2          ?         ?          ?
64_long  12     145/5/145/0/0/2/1            145/5/145/2/1            2          0         145       776.53
```

The completed 12-journey run carries the formal report card

    SUMMARY frames=12 chiefs_total=145 chiefs_at_end=145 stable>=5frames=145 localized_at_end=5 pop==L/3_at_end=0
    wall_seconds=776.53 seconds_per_frame=64.711

i.e. **no island reaches `L/3`** in the low-`S` regime, over 12 journeys that
include the frame-7 milestone.  (`localized_at_end=5` counts groups with
span <= RMAX and population >= 2 among the 145 chief groups; `maxpop seen` is
still 2.)


## Limits of this run (what is still not tested)

- **Frame 7 is covered (13 Sep 2026, second pass).**  In the candidate runs of
  `ISLAND_CENSUS.md` the broadcast election stamp only becomes live at journey 7
  (`bstamp = 3514`), which is where the reference `run8` plateau first breaks.
  The 12-journey run at `S = 64` (`build/island_census/sweep_S64_long`) crossed
  it without any change: every frame from 2 on is the identical row
  `145 K / 5 D / 0 S / 93 P`, max population 2, one centre, zero events.  So in
  the low-`S` regime the plateau does **not** break at frame 7 either -- unlike
  the candidate builds, whose quantisation needs their own rules.
- `L` is hardcoded to 9 in the harness, so the decisive test of a *dynamical*
  quantum -- does the equilibrium track `L/3` when `L` changes? -- is not
  reachable with this instrument.  `L = 15` needs `BLOCK = 2.28 x 10^6` cells
  (~13x the L = 9 cost).
- A positive here would still not have settled the circularity on its own: the
  seed itself derives the charge bits from the family index
  (`nu = floor(w / (L/3))`, `initSim.cpp`), so `L/3` remains an input of the
  topology.  Only an `L`-dependence test or an analytic `Gamma_cap = Gamma_esc`
  derivation can remove it.
