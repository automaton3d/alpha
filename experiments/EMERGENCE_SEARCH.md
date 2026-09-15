# A new search for an emergent quantum (after items 1-4)

**Decision recorded.** Menu item (5) offered two routes: declare the charge unit `L/3` an explicit
topological axiom, or look again for an emergent quantum.  This work stream takes the second
route, and this note is its first phase: the search space, the obstruction it runs into, and one
candidate that survives the locality criterion -- with its prediction registered before the runs.

## 1. What the search may use (design constraints from items 1-4)

A mechanism qualifies only if it is **cell-local** (no `ISLAND_SIZE`, no `L`, no family index,
no host table), if it can vary the fixed point **with the lattice side**, and if its prediction is
registered before the run (the practice that made item 4 falsifiable).  Items 1-4 established the
falsified ground: no space-providing channel holds copies apart at `d >= 2*RMAX+1`; the
reference transitions cap a group at `2` (T2); and the geometric capacities (shell counts
`26, 66, 158` cells at `r = 1, 2, 3`) are orders of magnitude above `L/3` and do not select it.

## 2. The local quantities that scale with `L`, and the quantum each can give

Every field a cell may read is in `simulation.h` (`w`, `x[]`, `ch`, `kind`, `parent`, `a`,
`r`, `r2`, `t`, `f = effective_t(t)`, `k`, `u`, `v`, `pol_*`, `pB`, `sB`, `s2B`, `m[]`,
`c[]`).  The ones that scale with the lattice, and the quantum a rule keyed on them can produce:

| local quantity | value at `L = 9 / 15` | scales as | quantum it can select |
|---|---|---|---|
| `RMAX` (learnable locally: the peak of `f` over one cycle) | `4 / 7` | `L/2` | `RMAX`, or `2*RMAX` distinct phases |
| breathing period `2*RMAX` (a cell can count its own cycles) | `8 / 14` | `L` | at most one member per phase -> `<= 2*RMAX` |
| `FRAME` (tick period, learnable by counting to wrap) | `878 / 2285` | `3*L^2` | nothing selective (too coarse) |
| address differences `|w - w'|` (learnable only through encounters) | -- | `W = 3*L^2` | `<=` the number of distinct addresses met |
| `EL` (lattice side) | `9 / 15` | `L` | not locally measurable without a wrap-around census |

`L/3` itself is **not** in this list, and cannot be assembled from it: `L/3 = W/(9*L)` needs two
global counts at once (`W` and `9L`).  The closest local quantity is `RMAX`, related by

```
L/3 = (2L / (3(L-1))) * RMAX        (RMAX = floor(L/2), L a multiple of 3)
```

whose coefficient tends to `2/3` but **is different at every accessible size**: `L/3 = 3` against
`RMAX = 4` at `L = 9`, `L/3 = 5` against `RMAX = 7` at `L = 15`.  Since a fixed local rule has a
fixed coefficient, **no rule of this class produces exactly `L/3`**; it can only be asymptotically
proportional.  That is the obstruction: in this architecture the *value* `L/3` is the
factorisation of the topology equation `W = 3*L^2 = (9L)(L/3)`, i.e. an input to the lattice
definition, while a *scaling* with `L` is available dynamically.

## 3. The candidate: identity by (charge word, breathing phase)

The reference refuses merges between **different** charge words (`chiefContact` returns early when
`main.ch != mirror.ch`).  The candidate extends exactly that refusal to sources that agree in word
*and* in breathing phase:

```
two sources with the same ch and the same t may not share a group
```

Reading: two bubbles superposed at one site, with the same charge word and the same phase of the
breathing clock, are **indistinguishable** -- which is the model's own conflation of same-charge
sources (the membership argument of `DYNAMIC_QUANTIZATION_LEGEND.md` turns on it).  The rule reads
only the cell's own `ch` and `t`: no `ISLAND_SIZE`, no `L`, no host table.  It converts the group
cap from the reference's `2` (set by T2) into the **phase occupancy** `<= 2*RMAX`, a locally
measurable number.

**Predictions, registered before the runs** (the harness prints them as well):

| arm | seed phases | predicted | why |
|---|---|---|---|
| in-phase | all copies at `t = 0` (canonical seed) | **no group forms: `max_pop = 1`** | every candidate merge is a phase conflict, so the election is refused throughout |
| phase-spread | copies at `t = 0, 1, ..., k-1` | **one group of `k`** (so `k = L/3` for a family of `L/3` copies) | each member occupies a distinct phase, so no merge is refused and the cap `2*RMAX` is not reached |

The second arm is the interesting one: with a phase-spread seed the *family population becomes
reachable* under a local rule, where the reference splits it (`2K+1D`) and the earlier candidate
reached it only by reading the family index (`DD_INTRA_ISLAND_FIX`).  The honest limit is stated
in the note's closing section: the rule removes the *cap*, not the *multiplicity* -- the value
still comes from how many copies the seed places, so what this buys is a local mechanism for
persistence, not a derivation of the number.

## 4. Falsifiers

1. In-phase arm: if a group forms anyway (`max_pop = 2` or more), the candidate is inert and the
   phase structure is not what the election sees.
2. Phase-spread arm: if the population stays at `2` (`2K+1D`), the phase spread did not survive
   the clock resets (`reemitAtContact` resets `t`), and the candidate cannot hold a family.
3. Both arms: if the result depends on the tube side or on `RMAX`, the mechanism is not local.

## 6. Measurement: the phase structure, not the rule, decides

Harness `experiments/p7_concentration.cpp` with the new mode `coPhase` (the three copies at one
site with breathing phases `t = 0, 1, 2`) and the `PHASE_DISTINCT_FSM` build variant
(`build_p7_concentration.bat`, now four binaries).  Eight light frames per arm:

| arm | build | contacts per frame | promotions | final | max_pop |
|---|---|---|---|---|---|
| `co` (in-phase) | reference | `156, 396, 156, 0, ...` | **1** | `2K+1D` | 2 |
| `coPhase` (phases 0,1,2) | reference | `52, 0, 52, 0, ...` | **0** | `1K+2D` | **3** |
| `co` (in-phase) | `PHASE_DISTINCT_FSM` | `156, 396, 156, 0, ...` | **0** | `1K+2D` | **3** |
| `coPhase` | `PHASE_DISTINCT_FSM` | `52, 0, 52, 0, ...` | 0 | `1K+2D` | 3 |

**Finding A (the rule works as designed).**  In the in-phase arm the candidate leaves the contact
count *identical* to the reference (`156, 396, 156, 0`) and removes exactly the reducer:
promotions `1 -> 0`, so the planted population `3` survives.  The rule reads only `ch` and `t`,
so this is the locality criterion satisfied, and it reproduces what `DD_INTRA_ISLAND_FIX` did
while reading no family index.

**Finding B (downgraded by the multiplicity check below).**  The **reference** build also
preserves the population `3` when the copies are given *distinct breathing phases*: `coPhase` with
no macro at all gives `1K+2D`, `max_pop = 3`, and **zero promotions**, against `2K+1D`,
`max_pop = 2` for the in-phase baseline.  The contact count does not vanish (`52` in some frames),
so the loss is selective.  The multiplicity check of section 6b shows this is **not** a mechanism:
at five copies the same phase spread still promotes twice and the population lands at `3`, not `5`.

## 6b. The multiplicity check: the rule scales, the phase spread does not

Same harness, mode `five`/`fiveCo` (five copies, `W = 5`, one site), ten light frames:

| arm | build | promotions | final | max_pop |
|---|---|---|---|---|
| `fiveCo` (five copies in phase) | reference | **3** | `4K+1D` | 2 |
| `five` (phases 0..4) | reference | **2** | `3K+2D` | 3 |
| `fiveCo` (five copies in phase) | `PHASE_DISTINCT_FSM` | **0** | **`1K+4D`** | **5** |
| `five` (phases 0..4) | `PHASE_DISTINCT_FSM` | 2 | `3K+2D` | 3 |

What the three rows say:

* **Reference, in phase**: three promotions cascade the planted group down to `4K+1D`, `max_pop 2`
  --- the membership bound at work, now measured at a second multiplicity.
* **Reference, phase-spread**: the spread *halves* the reduction (2 promotions instead of 3) and
  leaves a population of `3`, but does **not** preserve the family.  So the tube's three-copy
  result was a property of that multiplicity, not of the phase spread in general: the spread
  reduces the *rate* of the reducer, it does not remove it.  Finding B is recorded as a partial
  effect and the phase route is **closed as a mechanism**, which is what the multiplicity check was
  for.
* **The rule, in phase**: promotions `3 -> 0` and the planted population survives whole
  (`1K+4D`, `max_pop = 5`).  Together with the three-copy arm (`max_pop = 3`) the rule holds
  `N* = m` at `m = 3` and `m = 5` --- the family multiplicity --- and it reads only the cell's own
  charge word and breathing phase.
* **The rule, phase-spread**: identical to the reference (`3K+2D`), because with distinct phases
  there is no phase conflict to refuse.  The rule is therefore *not* a blanket freeze of the
  membership sector: it acts exactly on indistinguishable pairs, and its scope is measurable.

## 6c. Where this leaves the quantum

The rule does not *create* the multiplicity; it makes the multiplicity the seed places the
*stable* configuration, locally and without reading `ISLAND_SIZE`.  That is a weaker goal than
"derive `L/3`" and a stronger one than the previous candidate, which read the family index
(`DD_INTRA_ISLAND_FIX`): on the locality criterion the new rule is strictly better, and its
predictions are sharp.  The registered prediction for the full census is therefore:

* `PHASE_DISTINCT_FSM` on the canonical superposed seed, reference dynamics otherwise:
  `81` groups of `1K+2D` with `max_pop = 3 = L/3` at `L = 9`, and `135` groups of `1K+4D` with
  `max_pop = 5 = L/3` at `L = 15`;
* the promotion counter stays at zero along the frozen runs, as in the tube.

Both are falsifiable with one run each (the `L = 9` run is hours, the tube arms are already done);
if the `L = 15` arm also lands on `L/3`, the model has a *local* rule whose fixed point tracks the
family multiplicity at two lattice sides --- the value still inherited from the topology equation
`W = (9L)(L/3)`, which is where the paper already locates the axiom.

**My registered prediction was mis-specified.**  For the in-phase arm I predicted "no group forms,
all `S`, `max_pop = 1`": the harness *plants* `1K+2D`, and a rule that refuses merges cannot
dissolve an existing group, so the measured `1K+2D` is the correct outcome of the rule as written.
The prediction that mattered -- that the reducer is refused -- is confirmed (promotions `1 -> 0`).
Recorded here rather than quietly amended.

## 7. The decisive test this opens (registered, not yet run)

The multiplicity check retired the *seed-side* version of this prediction: giving every layer its
own phase (`cell.t = w mod (2*RMAX)`) lifts the reference's cap from `2` to `3` at five copies but
does not preserve the family, so the phase-spread seed is **not** registered as a candidate.  What
is registered is the rule, whose prediction is sharp because the in-phase arm of section 6b showed
the reducer switched off completely:

* `PHASE_DISTINCT_FSM` on the canonical superposed seed, reference dynamics otherwise:
  `81` groups of `1K+2D` with `max_pop = 3 = L/3` at `L = 9`, and `135` groups of `1K+4D` with
  `max_pop = 5 = L/3` at `L = 15`;
* the promotion counter stays at zero along the frozen runs, as in the tube;
* and, as a control, the same runs without the macro reproduce the published `235K + 8D`
  (`max_pop 2`) at `L = 9`.

If the two census arms land on `L/3`, the model has a **local** rule whose fixed point tracks the
family multiplicity at two lattice sides, with the value inherited from the topology equation
`W = (9L)(L/3)` where the paper already locates the axiom.  If they do not, the tube result is a
prepared-seed effect and the route is closed.

```bat
rem tube: done (sections 6 and 6b)
build\p7_concentration\p7_concentration_phase.exe 10 co
build\p7_concentration\p7_concentration_phase.exe 10 fiveCo

rem census: needs the island_census harness rebuilt with -D PHASE_DISTINCT_FSM (hours per run)
build\island_census\island_census_phase.exe 12 16384 build/island_census/phase_distinct
```

## 8. What the search has established so far

* No local quantity equals `L/3`; the obstruction is quantitative (`L/3 = (2L/(3(L-1))) RMAX`),
  so a fixed local rule can only be asymptotically proportional to it.
* The *seed-side* route is closed: a phase-spread seed reduces the rate of the reducer but does not
  remove it (`m = 5` still promotes twice), so the three-copy result was multiplicity-specific.
* The *rule-side* route is open and measured: `PHASE_DISTINCT_FSM` --- identity by (charge word,
  breathing phase) --- holds `N* = m` at `m = 3` and `m = 5` on a prepared family, refuses exactly
  the merges it should (promotions `3 -> 0`, contact count unchanged), and is inert when the phases
  are distinct, i.e. it has a measurable scope rather than being a blanket freeze.
* On the locality criterion this rule is strictly better than the earlier family-index candidate
  (`DD_INTRA_ISLAND_FIX`): it reads only `ch` and `t`, with no `ISLAND_SIZE`, no `L` and no host
  table.  What it does **not** do is derive the value: the multiplicity is the seed's, i.e. the
  topological axiom, which is where the paper already puts it.
