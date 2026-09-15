# A rule with a predicted `N*`: capture by shell overlap vs escape by surface (item 4)

**Question.** Section 6 of `DYNAMIC_QUANTIZATION_DERIVATION.md` states what a genuinely
dynamical quantum would need: a rule that makes a group of size `N*` the *stable*
configuration, i.e. capture winning up to `N*` and escape winning above it, with the crossing
set by *local* geometry.  Item (4) of the menu asks for such a rule **with a predicted `N*`**
-- a prediction registered before the run, not a value read off afterwards.

**Artifacts.** Rule: `SURFACE_ESCAPE_FSM` in `src/model/interaction.cpp` (macro-guarded, OFF
by default).  Harness: `experiments/p7_concentration.cpp` (new modes `chain`, `gap2`, `mid`,
`edge`, plus the existing `co`/`spread`; it prints the prediction itself).  Build:
`experiments/build_p7_concentration.bat` (now three binaries: `_ref`, `_fix`, `_escape`).
Logs: `build/p7_ref_*.txt`, `build/p7_esc_*.txt`.

## 1. The rule, and why each part is local

* **Capture by shell overlap** -- unchanged from the reference: a singleton joins a group when
  the two wavefronts are simultaneously present at a shared voxel (`encounter()`, gate
  `r > 0`, `interaction.cpp`).  This *is* the shell-overlap test, and the reference already
  fixes its range at `2*RMAX` (section 3).
* **Escape by surface** -- new: a delegate that shares **no** same-charge contact for one full
  partner rotation is released back to a singleton (`applySurfaceEscape()`, called once per
  light frame from `commitSourceTick()`).  The threshold is `W_USED` light frames because
  `rotatePartners()` (`utils.cpp`) advances the partner lattice by one slice per frame, so
  every ordered pair of layers is offered a meeting within that period: the threshold is
  *derived*, not fitted.

Both parts read only `w`, `ch`, `kind` and the per-cell wavefront state: no `ISLAND_SIZE`, no
`L`, no family index.  The candidate is strictly weaker in information than `EXCLUSION_FSM`,
which read `w / 3` -- the criterion the earlier notes flagged as a stand-in.

## 2. The prediction, as registered before the run

The harness prints it, so it is part of every log file rather than a later rationalisation:

```
PREDICTION escape-rule=ON: gap <= 2*RMAX -> contacts keep the group, T2 caps max_pop at 2
PREDICTION escape-rule=ON: gap > 2*RMAX -> no shell overlap, every delegate released
```

where `gap` is the separation of the three planted constituents along the tube axis and
`2*RMAX = 4` cells in this geometry.  Concretely: **`N* = 2` for `gap <= 4`, `N* = 1` for
`gap > 4`** (the single-member value, once the delegates have evaporated).  The gap sequence
`0, 1, 2, 3, 4, 5` brackets the boundary from both sides.

## 3. Measurement (10 frames per run, tube `15x5x5x3`, `RMAX = 2`)

Reference build (rule OFF) -- also the control; it reproduces the published P7 rows
(`gap = 0` splits via T2, `gap = 5` keeps `1K+2D`):

| gap | promotes | final | max_pop |
|---|---|---|---|
| 0 | 1 | 2K+1D | 2 |
| 1 | 1 | 2K+1D | 2 |
| 2 | 1 | 2K+1D | 2 |
| 3 | 1 | 2K+1D | 2 |
| 4 | 1 | 2K+1D | 2 |
| 5 | 0 | 1K+2D | 3 |

New datum for the *contact range*: the shells still overlap at `gap = 4 = 2*RMAX` (a promotion
fires) and not at `gap = 5`.  The range `2*RMAX` is therefore confirmed from both sides, not
only from above.

Candidate build (`SURFACE_ESCAPE_FSM` ON):

| gap | escapes | final | max_pop | predicted `N*` | verdict |
|---|---|---|---|---|---|
| 0 | 0 | 2K+1D | 2 | 2 | confirmed |
| 1 | 0 | 2K+1D | 2 | 2 | confirmed |
| 2 | 0 | 2K+1D | 2 | 2 | confirmed |
| 3 | **2** | 2K+0D+1S | **1** | 2 | **falsified** |
| 4 | **2** | 2K+0D+1S | **1** | 2 | **falsified** |
| 5 | 2 | 1K+0D+2S | 1 | 1 | confirmed |

**The prediction is falsified at `gap = 3` and `gap = 4`.**  The measured boundary of the
escape rule lies between `gap = 2` and `gap = 3`, i.e. *below* the shell-overlap range
`2*RMAX = 4`.

## 4. Why, and what it means

**What "duty cycle" means here.**  The shell of a bubble is not a static ball: the active
wavefront is scheduled by comparing each cell's radius with the breathing phase,
`r <= effective_t(t)`, and `t` cycles modulo `2*RMAX` (`update_pulsating_wavefront()`,
`simulation.cpp`; the tick rule is `t = (t+1) % (2*RMAX)`).  At `RMAX = 2` the breathing
period is therefore **4 light frames**, and at the contraction phase the gate `r > 0` fails
everywhere, so *no* pair contacts in that frame -- not even co-located ones.  The **duty
cycle** of a pair is the fraction of that 4-frame breathing period in which the two shells
are simultaneously open at a shared voxel.  The measured per-frame contact pattern in the
reference logs (values are the cell-level encounter counter; the zeros are the contraction
phases, and the pattern repeats with period 4 = `2*RMAX`):

| gap | contacts per frame (frames 2..7) | duty | longest contact-free run |
|---|---|---|---|
| 0 | 156, 396, 156, **0**, 156, 396 | 3/4 | 1 frame |
| 1 | 82, 396, 156, **0**, 156, 396 | 3/4 | 1 frame |
| 2 | 36, 106, 82, **0**, 156, 396 | 3/4 | 1 frame |
| 3 | 32, **0, 0, 0**, 32 | 1/4 | **3 frames** |
| 4 | 52, **0, 0, 0**, 52 | 1/4 | **3 frames** |
| 5 | 0, 0, 0, 0, 0, 0 | 0 | unbounded |

**The threshold converts duty into a boundary.**  The rule releases a delegate when it goes
`W_USED` (= 3) consecutive frames without contact, so it fires exactly when the *longest
contact-free run* reaches 3 frames -- i.e. when the duty cycle is `1/4` or less.  So the
escape boundary sits between `gap = 2` (duty `3/4`, longest run 1) and `gap = 3` (duty
`1/4`, longest run 3), and not at the geometric range `2*RMAX = 4` where overlap still
*exists*.

**Why the threshold is marginal, not fundamental.**  A pair of specific layers is only
*offered* a meeting once per partner rotation (`rotatePartners()` advances one slice per
frame, period `W_USED = 3`), while the breathing period is `2*RMAX = 4`; the two cadences are
incommensurate (joint period 12).  With the timer set to exactly `W_USED`, the rule asks
"does this pair overlap at *every* scheduled meeting?", turning an *existential* geometric
criterion (overlap at some phase, which holds up to `gap = 4`) into a *universal* temporal
one (overlap at all phases, which holds only up to `gap = 2`).  The candidate therefore
measures persistence of contact rather than existence of overlap, and the surviving group is
the one that touches in every scheduled frame -- which is why the natural geometric threshold
`2*RMAX` is not the threshold the dynamics uses.

Consequences for the quantisation question:

* The designed rule **does** produce a genuine interior balance (capture inside, escape
  outside, both rates nonzero) -- what section 6 asked for.  But the value selected is
  `N* = 2` at best and `N* = 1` beyond ~2 cells: the crossing is set by the pulsation duty
  cycle, and the only upper bound inherited is the T2 promotion cap of 2.
* `N* = L/3 = 3` appears **nowhere**: not in the surviving regime (2), not in the dissolved one
  (1).  Adding the escape rule to the reference makes populations *smaller*, not quantised.
* The rule has a visible side effect worth recording: it dissolves spread islands -- exactly
  the configurations that survive transport in the inertia fixtures.  So the candidate states
  a sharp requirement on transport: persistence needs *sustained* contact, not occasional
  overlap.
* Both branches of the crossing are local and parameter-free, so this is a candidate rule of
  the kind the design criterion allows.  It simply does not deliver `L/3`, and we now know
  *why* the natural geometric threshold (`2*RMAX`) is not the threshold the dynamics uses.

## 5. Falsifiable follow-ups

1. **Phase-matched seed.**  If the three copies are emitted in phase (or `RMAX` is raised so
   the overlap window covers the whole cycle), the candidate's boundary should move out to
   `gap = 2*RMAX = 4`.  Testable by adding a phase field to `placeLayer()`.
2. **Longer timer.**  With the threshold at `2*W_USED` frames the boundary should move to
   `gap = 3`-`4`; if it does not, intermittency is not the only effect.
3. **Vertical (`w`) separation.**  The same experiment with the copies separated in `w`
   rather than in `x` isolates the rotation schedule from the spatial distance.

## Limits

* Six geometries, 10 frames, one tube size: the boundary is bracketed to one cell, not located
  exactly (a `gap` scan at half-cell resolution is not possible on this lattice).
* `surface_escapes_total` counts release *events*, not distinct members: a member released and
  re-captured counts twice (visible at `gap = 3`, `4`: two events, one member in the final
  state).
* The reference build is untouched by the patch (every change is inside `#ifdef
  SURFACE_ESCAPE_FSM`), which the control rows confirm.

## Reproduce

```bat
experiments\build_p7_concentration.bat
build\p7_concentration\p7_concentration_ref.exe    10 mid
build\p7_concentration\p7_concentration_escape.exe 10 mid     rem gap=3: falsified
build\p7_concentration\p7_concentration_escape.exe 10 spread  rem gap=5: confirmed
```
