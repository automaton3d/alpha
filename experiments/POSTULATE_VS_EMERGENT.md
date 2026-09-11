# Postulate vs emergent -- WP3.4 control / editor response

This table answers the editor point "speed/polarisation prescribed": for each
model ingredient it states whether it is a **postulate** that *defines* the rule
(P), an **emergent** consequence *measured* in the reference runs (E), or a
**candidate** that the current implementation does not establish (C).  It is the
table requested for the Limitations section of the manuscript.  Code anchors are
given so a referee can check each entry.

## The table

| Ingredient | Status | Definition (where) | Note |
|---|---|---|---|
| Cell size X | **P** (free unit) | manuscript, App. "Calculation of X" | fixes the metric normalisation; not derived |
| Clock: front advance = 1 cell/tick | **P** (definition of the information clock) | `simulation.h` `effective_t` (372); `simulation.cpp` `phase_step` (264-265) | the active shell is the set `{r == effective_t(t)}`; `r` is maintained by `update_pulsating_wavefront()` (simulation.cpp:109) via additions. The 1/tick speed is the *definition* of the clock, not a measured emergent constant |
| Breathing phase `f = effective_t(t)` (triangle, period 2·RMAX) | **P** | `simulation.h` (368-380) | |
| `v_max = X/N_t` | **P** (identity) | manuscript "max information speed" | `N_t` = ticks per light frame |
| `W = 3L^2`, `9L` islands of `L/3` | **P** (topology) | census allocates `W = 3L^2` (`island_census.cpp:285`) | the GUI cfg may set any `W` (e.g. 10); only the census uses the canonical value |
| Six charge bits + masks | **P** | `simulation.h` (31-39) | |
| Wave update (Laplacian, damping, shell forcing) | **P** (rule, parameterised) | `simulation.cpp` `phase_step` (296-345) | the dispersion relation is a function of the chosen shifts, not fitted to data |
| Charge conservation | **E** (measured) | `charges.cpp` census each tick | `Dtot == dPair + freeD: OK` every tick |
| Speed of light `c` | **E** (observer reading) | manuscript, special relativity | `c` is the value an observer assigns to `v_max`; not in the rule |
| Polarization pair `(pol_u,pol_v)`, `pB/sB` | **C** (postulated broadcast, NOT emergent) | `polarization.cpp`; `SEED_ASYMMETRY.md` | does not bootstrap from the zero seed; `alpha_D` unobservable |
| Aggregation (`1 K + n D` islands) | **C** (not emergent from the seed) | `ISLAND_CENSUS.md` | production path reaches an absorbing plateau; needs a candidate (EXCLUSION / `DD_INTRA_ISLAND_FIX`) |
| Island inertia / transport | **C** (prepared, controlled) | `INERTIA_REVALIDATION.md` | K-only transport passes; multi-constituent island identity fragile |

## Reading

- The **speed** (`v_max`, the breathing phase) and the **polarization broadcast**
  are the two ingredients the editor flagged as "prescribed": this table states,
  with code anchors, that they are a **postulated clock** and a **postulated
  broadcast** respectively -- not emergent results. The manuscript's P/M/C
  grading already labels them; this table makes the split explicit and
  checkable.
- Only **charge conservation** and the **observer reading of `c`** are emergent in
  the strong sense; the aggregation/inertia claims are **candidates** (C), not
  established by the reference rule.

## Reproduce / check

```bat
rem charge conservation per tick (census line printed by any harness)
build\pbsb_two\pbsb_two.exe        rem or alpha_probe / island_census
rem the active shell radius is effective_t(t): see the source anchors above
```

No new run is required: every entry is either a code anchor (a referee can read
it) or an already-reproduced measurement (`RESULTS_v2.md`, `ISLAND_CENSUS.md`).
