# Gravity probe — two-aggregate free fall in a rectangular tube

Status: **design / pre-registration** (nothing implemented yet).

## 1. Motivation

The manuscripts propose gravity as a *conjectured* residual effect of the
automaton's contact dynamics (``gravitons'' = R1 fully-complementary charge
pairs, cross-sector; ``None of these behaviours has been observed in the
implementation'').  Following the discipline of the alpha campaign, this probe
turns that conjecture into falsifiable measurements instead of narrative.

## 2. Questions and pre-registered targets

| Id | Question | Operational target | Acceptance |
|----|----------|--------------------|-----------|
| G0 | Does the pipeline measure anything stable? | Two aggregates at rest, distance $d(t)$ per light frame | $d$ reproducible across seeds/parities |
| G1 | Is there a distance law? | Relative acceleration $a(t)$ vs separation $r$ | Stable exponent $\beta$ ($a\propto r^{-\beta}$) over a window of $\ge$3 octaves |
| G2 | Is it composition-independent? | Repeat G1 with different internal structure (K+D vs P-stack) | $\beta$ and prefactor match within tolerance |
| G3 | Is there a two-body regime? | Bounded orbit over $N\ge 8$ revolutions without retuning | No drift/exponential blow-up |

**Null outcome (expected, given the current state):** if G1/G2 fail at
accessible $L$, the gravity conjecture is recorded as *not reproduced* at
accessible sizes — a clean negative result, consistent with the alpha battery.

## 3. Geometry: rectangular tube (anisotropic sublattice)

Two source aggregates are placed on the **long axis** $x$; the two short axes
only need to be wider than the interaction window.

```
LX = long axis (reach)
LY = LZ = short side   (>= 2 * RMAX_short)
W  = number of W-layers (>= 2 for the pair; larger W adds "vacuum" layers)
cells = LX * LY * LZ * W
```

Resource comparison (W fixed, 64 bytes/cell):

| Geometry | LX | LY=LZ | W | cells | RAM |
|----------|----|-------|---|-------|-----|
| cube (two-bubble) | 7 | 7 | 2 | 686 | ~44 KB |
| cube (reach 401) | 401 | 401 | 16 | ~1.03e9 | ~66 GB |
| **tube (reach 401)** | 401 | 33 | 16 | ~7.0e6 | ~450 MB |
| **tube pilot** | 101 | 15 | 8 | ~1.8e5 | ~12 MB |

Tube vs cube at the same reach and $W$: a factor $\approx LX^2/(LY\,LZ)$
(~150x in the example).  This is the "incision" that makes $G1$-$G3$
feasible on one workstation.

### What the code needs (allocator generalisation)

The current kernel assumes a cubic torus ($EL$ per axis).  The tube requires:

1. `tryAllocate(ELX, ELY, ELZ, W)` (or a tube config) and indexing
   `((x*ELX + y)*ELY + z)*W + w`.
2. Per-axis periodic wrap in `spherical_wrap`/neighbour access and in
   `shortestDelta` (currently `mod = EL`).
3. `calculateParameters`: per-axis `RMAX` bound = interaction-window limit;
   the absorbing outer boundary of `phase_step` must use the short-side limit.
4. `lcenters` relocation clamped/wrapped per axis.
5. CUDA bridge constants updated in lock-step (or tube stays CPU-only first).

Optional cheap first step: **S0 pilot on the existing cube** to validate the
measurement pipeline (G0) before the allocator work.

## 4. Measurement plan (per completed light frame)

- Reuse the worker pattern of the statistics observatory: one thread runs
  `automaton::simulation()`; sampling happens only when a light frame completes.
- State read from `lattice_curr` (read-only), exactly as the attractor/wavefront
  instrumentation modules do.
- Track, for each of the two aggregates:
  - source-centre coordinates (`lcenters[w]`);
  - operational mass $M$ = number of member cells of the island
    ($a \neq W\_USED$) around each centre;
  - separation $r(t)$ = shortest torus distance along $x$ (per-axis delta);
  - relative acceleration from a centred second difference of $r(t)$ over a
    smoothing window $w_s$ (e.g. 11 frames), reported with error bars.
- Reuse the existing encounter diagnostics (`enc_*`) to log whether any
  contact actually occurred (sanity: no contact -> no force -> $a\approx0$).

Outputs: per-frame CSV (frame, r, d2r, M1, M2, enc counters) + terminal
summary table, in the style of `alpha_probe`.

## 5. Planned implementation order

1. **S0 — pilot on cube**: two complementary aggregates at rest, $SEP$ cells
   apart, small $L$; measure $d(t)$ and $a(t)$; validate G0 and the CSV path.
2. **S1 — tube allocator**: per-axis edges (CPU path first), keep the cubic
   path as the default configuration; re-run the L=7 reference (6996/18/0)
   to prove bit-identical behaviour when ELX=ELY=ELZ.
3. **S2 — tube runs**: reach sweep $LX = 25, 51, 101, 201, 401$ at fixed
   short side and $W$; estimate $\beta$ per reach.
4. **S3 — composition/equivalence + orbit tests** (G2/G3), then write the
   report card.

## 6. Risks / caveats

- Wall effects: if the pulse hits the short boundary before the two fronts
  meet, the measured law is contaminated — the short side and frame budget
  must keep the active shell inside the interaction window.
- Two-bubble contact is known to be "sticky" (pre/post window absent) at
  L=7 in the campaign; aggregates with internal structure may behave
  differently — that is exactly what G2 checks.
- A null result at accessible reach does not disprove the conjecture; it
  bounds it (report reach and budget with the negative).

## 7. S0 pilot — implemented (Sep 2026)

Harness: `experiments/gravity_probe.cpp` (build: `experiments/build_gravity.bat`,
output CSV per frame).  Two single-source bubbles at rest, SEP cells apart,
x axis; measures d(t), second-difference dd(t), operational masses, and
encounter counters each light frame.

Observed (L=11, SEP=5, 120 light frames, S=16384): contacts fired
(calls=5616) but s2B=0, d(t)=4.0 constant — no force branch opens at the
default modulus; pipeline stable (G0 MEASURED, d span 0).  With S=64 both
"attract" and "repel" collapse to d=0 within a few frames (pair/contact
sticky behaviour of the two-bubble limit, as in the campaign), i.e. there is
no clean two-body regime at L=11 with W=2 — reinforcing the case for the
tube (S1) and for aggregates with internal structure (G2).

## 8. S1 — tube allocator (next; not yet implemented)

Required code slices (CPU path first, cube kept as default):

1. Per-axis dims + allocator: add tube entry point that sizes the three
   lattices with edges (ELX, ELY, ELZ) and W; cube stays `tryAllocate(EL,W)`.
2. Indexing/wrap: `getCell` indexing and per-axis wrap/`shortestDelta` on
   ELX/ELY/ELZ (simulation.h, geometry, neighbor access).
3. Wavefront/distance-field + phase-step absorbing limit keyed to the short
   sides; relocation and pair-consumption bounds per axis.
4. Re-validation: with ELX=ELY=ELZ the reference run must stay bit-identical
   (6996/18/0); only then unequal-edge runs are trusted.
5. CUDA bridge updated in lock-step (or tube is CPU-only initially).

**Slice 1 — done (Sep 2026).** Globals `ELX/ELY/ELZ` added (cube default
ELX=ELY=ELZ=EL, set in `calculateParameters`) and `tryAllocateTube(LX,LY,LZ,W)`
declared/implemented: equal edges delegate to the cubic allocator; unequal
edges are refused with an explanatory error (no silent wrong physics).
Re-validated bit-identical: L=7 reference run 6996/18/0 and full CPU GUI
build (`EXITCODE=0`).

**Slice 2 — done (Sep 2026).** Per-axis indexing/wrap in the CPU path:
`getCell` uses per-axis strides `((x*ELY+y)*ELZ+z)`, `spherical_wrap` and the
neighbour/wavefront/phase-step wraps use `ELX/ELY/ELZ`, and
`applyMomentum` wraps each axis separately (`wrapCoordAxis`).  Re-validated
bit-identical (L=7: 6996/18/0) and full CPU GUI build (`EXITCODE=0`).

**Slice 3 — core passes per-axis (done, Sep 2026).** All per-cell position
loops in `simulation.cpp` (wavefront distance field, `phase_step`, the
interaction FSM, the partner refresh) now iterate `ELX/ELY/ELZ`, and the
radial distance-field shortcut uses per-axis bounds.  ELX/ELY/ELZ externs were
moved before the inline accessors so the header compiles.  Re-validated
bit-identical (L=7: 6996/18/0) and full CPU GUI build (`EXITCODE=0`).

**Slice 4 — done (Sep 2026).** `interaction.cpp` source motion/wrap is
now per-axis (`moveOneStep`, `moveOneStepAway`, `reemitAtContact`, hunting
`c[]`, forward-delta slot); re-validated bit-identical (L=7: 6996/18/0) and
full CPU GUI build (`EXITCODE=0`).

**Slice 5 — in progress (Sep 2026).** Per-axis seeds + charge census:
`initSim` `index()` stride and `initGeneral` seed loops use `ELX/ELY/ELZ`;
`charges` census position loops converted (read-only).  `polarization` fully
per-axis: per-layer visited scratch sized `ELX*ELY*ELZ`, `cellCode` strides,
walker `moveOK` bounds, `elect` scan and bstamp reset loops, `diffusePass`
relax bounds and both sweep directions.  Re-validated bit-identical
(L=7 reference 6996/18/0 on both probe runners after every module).

**Executed-closure audit (done, Sep 2026).** For the W=2 headless two-bubble
run the linked set is attractor/charges/geometry/initSim/interaction/
polarization/simulation/utils/wavefront.  Executed legacy-`EL` geometry is now
gone: `utils::rotatePartners()` (called every frame while `repr.k <
ENCOUNTER`) converted to per-axis slice rotation; `wavefront` profile
accumulator loop converted to per-axis; remaining `EL` occurrences in the
closure are cosmetic prints (`printParams`, `attractor`/`wavefront` reports)
or semantic canonical constants (`nineL` hash modulus, `ISLAND_COUNT`,
guard `EL==0`).  `geometry` geodesic helpers are referenced only by
`simulation.cpp`'s own inline `spherical_wrap`, not by the module.  Dead-in-
closure utils helpers (`relocateGlobal`, `printLattice`, `sanityTest2/3`)
stay cube-scoped and are documented as not part of the tube W=2 path.

Still blocking a trustworthy unequal-edge run:
- ~~tube parameter configuration~~ **done (Sep 2026)**: `tryAllocateTube`
  released for unequal edges — odd edges >= 5, W >= 2; derives the full
  anisotropic parameter set (schedule scale = long edge as legacy `EL`,
  `RMAX` = short side, `CENTER` on the short-side centre), allocates the
  rectangular block, keeps equal-edge delegation bit-identical.  Cube
  reference re-validated (L=7: 6996/18/0) after the release;
- W-island topology stays canonical (W = 3L^2) — tube runs use the small-W path
  (W=2), which does not derive W from the edges.
- **Tube smoke harness — done (Sep 2026).** `gravity_probe` tube mode:
  `gravity_probe tube LX LY LZ SEP FRAMES SIEVE mag mode csv` —
  per-axis `placeSource` clearing, bubbles SEP apart on the long axis,
  per-axis shortest-distance wrap, `tryAllocateTube` path, and a SEP guard
  against unsigned underflow.  Smoke runs (all rc=0):
  - `51 x 11 x 11`, SEP=20, S=16384, 8 light frames: allocate 12342 cells,
    static d=20.000 throughout (gate closed, calls=0), CSV intact — first
    unequal-edge run on the anisotropic geometry;
  - `21 x 5 x 5`, SEP=8, S=16384, 12 frames: 0.74 s, static d=8.000.
  Cube regression after harness edits: L=7 d-profile unchanged
  (d0=3.0, dmin=2.0@119).  Note: per-tick cost scales with the full tube
  volume, so long-reach runs should keep frames small.

Until these land, `tryAllocateTube` keeps refusing unequal edges (no silent
wrong physics).  Proceeding slice-by-slice with a cube re-validation after
every module; do not merge half-converted anisotropic behaviour into the
default cube path.


## 9. Tube strategy — refined (Sep 2026)

Cost comparison shows the tube only pays off where the long axis is used for
long-range travel at small interaction window:

| Probe | Geometry that fits | Cost driver | Tube needed? |
|-------|--------------------|-------------|--------------|
| G1 force law (r up to ~RMAX) | cube, W=2 | L^3*2 | PARTIAL: cube is memory-cheap, but per-tick CPU scales L^3, so long multi-octave reach needs the tube |
| G2 equivalence | cube, W=2 (repeat G1) | L^3*2 | NO |
| G3 orbit / long evolution | cube: x-wrap kills it | L^3*2 with small RMAX = waste | YES (long axis) |
| vacuum graviton runs (large W) | canonical-like, needs W>>2 | W*L^3 | YES |

Recommendation:

- **Track A (now):** run G1/G2 on cubes with W=2 at CPU-feasible L (typically
  L <= 21-31; `gravity_probe` EL cap raised to 511 with an 8 GB guard, but
  per-tick CPU cost scales L^3).  This validates the measurement pipeline and
  gives a few octaves of SEP; the long multi-octave reach (and orbits) is a
  tube job (Track B).
- **Track B (later, dedicated):** finish the per-axis conversion slice-by-slice
  (`initSim` seeds/W-topology + per-axis CENTER, `polarization` packing,
  `charges` census, `utils`/`geometry`, tube RMAX=short/2) and only then
  release `tryAllocateTube` for unequal edges.  Needed for G3 orbits and for
  vacuum/graviton media with large W.

Unequal edges are now released through `tryAllocateTube` with the documented
anisotropic parameter set (Track B, Sep 2026); the default cube path stays
bit-identical, and the slice-1..5 partials are cube-safe (6996/18/0) after
every step.

## 10. Track B — S2 reach pilot on the tube (Sep 2026)

Fixed short cross-section 7 x 7 (RMAX = 3), W = 2, SEP = 4 (overlapping
shells, d0 = 4), sieve S = 64, attract, mag = 0.  Light-frame length scales
with the long edge (FRAME = 388/792/1542 ticks for LX = 25/51/101), so rows
at the same frame count are NOT the same wall-clock physics; compare within
reach and by tick totals:

| LX | frames | ticks | d0 | dmin @ f | dmax | dfinal | calls / s2B / pairs | note |
|----|--------|-------|----|----|----|--------|----------------------|------|
| 25 | 14 | 5432 | 4.0 | 1.0 @ 10 | 12.0 | 5.1 | 187 / 117 / 32 | full contact -> close pass -> recoil |
| 51 | 14 | 10892 | 4.0 | 4.0 @ 0 | 10.0 | 10.0 | 96 / 36 / 36 | recoil still expanding |
| 51 | 7 | 5446 | 4.0 | 4.0 @ 0 | 7.0 | 7.0 | 48 / 24 / 24 | early recoil |
| 101 | 4 | 6112 | 4.0 | 4.0 @ 0 | 4.0 | 4.0 | 24 / 0 / 0 | contact started, no motion yet |

Reading: the unequal-edge tube runs cleanly at every reach; all d stay well
below LX/2 (no wrap contamination inside the windows), contact + separation
events fire with the sieve open.  Frame-granularity artifacts (rotatePartners
once per light frame, FRAME growing with LX) mean exact tick-equal worldlines
differ across reaches; a quantitative beta estimate needs a per-tick CSV
column plus a frame budget long enough for one full close-pass cycle per
reach (the LX = 101 cycle costs minutes on CPU).

**Per-tick CSV (implemented Sep 2026).** `gravity_probe` rows now carry the
total automaton tick (`frame,tick,d,dd,m0,m1,calls,s2b,pairs`).  Tick-resolved
runs (7 x 7, SEP=4, S=64) reveal a *collapse telegraph*: d changes only on
light frames where a contact burst passes the sieve gate, then holds on a
plateau.  From the CSVs:

- LX = 51, 26 light frames (778 ticks/frame, 20228 ticks total):
  d = 4 -> 7 (burst @ f4-5) -> 10 (@ f10-11) -> 18 (@ f16-17) -> 7 (inward
  burst @ f22-23); outward bursts every ~6 light frames (~4.7 kticks), then a
  single inward collapse step of -11.
- LX = 101, 18 light frames (1528 ticks/frame, 27504 ticks): one outward
  burst 4 -> 12 (@ f10-11), plateaus afterwards; d max 12 << LX/2.

So the burst cadence is ~6 light frames at both reaches (wall-tick interval
scales with FRAME), and each passed burst translates into a quantized
separation jump — the "force" of the model is indeed sieve-gated collapse, in
the tube as on the cube.  Beta as a smooth d(t) exponent is not meaningful at
this resolution; the mean outward creep on the LX=51 train was
(18-4)/17116 ~ 8e-4 cells/tick, realized in bursts.

## 11. Track A — cube mini-sweep (preliminary, Sep 2026)

S=64 (gate open), W=2 single-bubble sources, mag=0, both charge modes:

| L | SEP | mode | d0 | dmin @ frame | dfinal | approach | note |
|---|-----|------|----|----|--------|----------|------|
| 11 | 3 | attract/repel | 2.0 | 2.0 @ 0 | 3.0 | 2.5% | no collapse |
| 11 | 5 | attract/repel | 4.0 | 0.0 @ 3 | 0.0 | 20% | collapse |
| 11 | 7 | attract/repel | 5.0 | 0.0 @ 7 | 0.0 | 5% | collapse via wrap |
| 15 | 5 | attract/repel | 4.0 | 1.0 @ 3 | 6.0 | 8% | contact then recoil |
| 15 | 9 | attract/repel | 7.0 | 5.0 @ 4 | 6.0 | 33% | approach then settle |
| 15 | 13 | attract/repel | 3.0 | 2.0 @ 6 | 2.0 | 17% | wrap-contaminated (SEP~L/2) |

Reading: with two single-source bubbles (W=2) attract and repel are
indistinguishable; evolution is a single "contact event" (collapse or recoil
around frames 3-7), not a distance law.  This is the expected negative for
bare two-bubble "mass": G1 requires aggregates with internal structure and/or
the tube geometry (Track B) — the pipeline itself (d(t), dd(t), encounter
counters, CSV) is exercised and stable.

## 12. Telegraph model — S/SEP sweep on the tube (Sep 2026)

Fixed tube LX = 51, short 7 x 7 (RMAX = 3), SEP over the long axis, W = 2,
attract, mag = 0, 16 light frames (~12.5 kticks).  Result: the contact
dynamics is a **gate-filtered burst train**:

1. **Overlap gate (SEP).** The train exists only when the shells overlap
   (d0 < 2 RMAX, i.e. SEP < 6 here).  SEP = 6 (shells just touch) gives
   calls = 0 at every S (S=32 and S=64 tested): static, no contact at all.
2. **Attempt train (S-independent).** With SEP = 4 the contact attempts fire
   on fixed windows ~every 6 light frames: calls = 120 in 16 frames for every
   S in {32, 64, 128, 256}.
3. **Sieve pass = telegraph step.** A separation jump happens exactly on the
   light frame whose window has s2B > 0; otherwise d stays on its plateau:

   | S | calls | s2B | bursts (tick, Delta d) | final d |
   |---|-------|-----|------------------------|---------|
   | 32 | 120 | 36 | +3 @3890, +3 @8558 | 10 |
   | 64 | 120* | 36 | +3 @3890, +3 @8558 | 10 |
   | 128 | 120 | 0 | none | 4 (static) |
   | 256 | 120 | 0 | none | 4 (static) |

   (*calls at 16 frames from the 26-frame lx51t.csv.)  Later bursts (past
   frame 16) grow to +8, and inward collapse steps of -11 appear — a full
   outward-then-inward cycle at S <= 64.

4. **Interpretation.** Within 16 frames the pass count is flat between S=32
   and S=64 (locked windows), then drops to zero at S >= 128: fewer than one
   expected pass per 120 attempts.  The "force" is the discrete telegraph of
   sieve-gated collapse — jump size and cadence are the observables, not a
   smooth acceleration.

**Amplitude vs shell radius (Sep 2026).** Same tube LX = 51, S = 64, with a
larger cross-section 9 x 9 (RMAX = 4, 2 RMAX = 8).  Overlap gate boundary
confirmed again: SEP = 8 (= 2 RMAX) -> calls = 0, fully static.  Raw burst
train within the sampled window:

| geometry | RMAX | SEP | d0 | first burst (tick, Delta d) | later in window | calls / s2B |
|----------|------|-----|----|-----------------------------|-----------------|-------------|
| 7 x 7 | 3 | 4 | 4 | +3 @ 3890 | +3 @ 8558 (by f16) | 120 / 36 |
| 9 x 9 | 4 | 4 | 4 | +8 @ 10990 | none up to f22 (17116 t) | 393 / 17 |
| 9 x 9 | 4 | 6 | 6 | +3 @ 10990 | none up to f16 | 96 / 16 |
| 9 x 9 | 4 | 8 | 8 | — (no contact) | — | 0 / 0 |

Reading: with the bigger shells the first passed window arrives later and
carries a larger outward jump at the same overlap depth (SEP = 4): Delta d = 8
vs 3 at RMAX = 3.  At overlap depth 2RMAX - d0 = 2 (RMAX=3/SEP=4 and
RMAX=4/SEP=6) the first burst is +3 in both cases.  A 45-frame follow-up at
RMAX=4/SEP=4 (35 kticks, calls=1072, s2B=290) captured 8 passed windows:
bursts +8 @10990, -3 @18055, -8 @29830, +7 @31400, -3 @32185, -3 @33755,
+21 @34540, -16 @35325 (inward collapse to d=1, outward flight to d=22.3).
Outward (+36) vs inward (-33) totals nearly balance: after the early outward
transient the telegraph is symmetric.  CSVs: `c9_sep4.csv`, `c9_sep6.csv`,
`c9_sep8.csv`, `c9_long.csv`.  S/SEP-sweep CSVs kept: `s32.csv`, `s128.csv`,
`s256.csv`, `s32sep6.csv`, plus the tick-resolved `lx25t/lx51t/lx101t.csv`
from section 10.

## 13. Rev 2 — Inertia protocol on the tube (island + propellers, Sep 2026)

**Notation (corrected, Sep 2026).** Three names are kept strictly apart:

- **pair (P)** — `SourceKind::P`: the neutral, charge-complementary pair
  registered at contact (`kind == P`; charges.cpp counts it as hidden anti).
  The pair kind is *generic*: depending on the charge geometry and context it
  is the fragment of any gauge boson of the model (photon, W, Z, gluon,
  graviton, ...), not only of the photon.  A pair is not a propeller by
  itself.
- **propeller** — the transient *functional role* of a pair that is carrying
  a pending momentum impulse (`reloc != 0`), i.e. "a pair currently acting as
  a momentum carrier" (interaction.cpp).  Every propeller is a pair (P) *with
  momentum*; a pair at rest is a pair (P), not a propeller.  Propellers are
  pairs, never W-layers, never islands.
- **layer / W-island** — the structural unit of the fabric.  The island is a
  cluster of `n_i` layers (chief kind K + ordinary layers kind S) with common
  affinity/leader.  Do not count "propeller layers".

In all formulas and tables below: "P" denotes only the pair kind; "propeller"
denotes only the momentum-carrying pair.  The interaction branches that move
the island are **propeller x K** and **propeller x D**; in code they are the
kind branches `P x K / P x D` of `encounter()` (interaction.cpp, branches
7-8): the target receives the propeller's momentum (`reloc += P.m`) and
re-emits at the contact point.

**Correction of course.** The two-bare-bubble runs (W=2, one layer per
"island") could not measure inertia or mass: they have no internal structure
and no momentum-carrier machinery.  Inertia must be measured with real
composite objects.

**Operational definition (per user guidance, Sep 2026).** Inertia is
verified by computing the average velocity of the centre of mass of the
island elements plus the propellers, frame by frame:

    x_cm(t) = (1/N) sum_e x_e(t)        (per light frame, tube x axis)
    v_cm(t) = x_cm(t+1) - x_cm(t)       (cells per light frame)

Elements e: each island layer contributes its source centre (one coordinate
per layer, so `n_i` coordinates) and each propeller pair contributes its pair
centre once (the two halves of a P share the re-emitted contact point).
`N = n_i + n_pi` with `n_pi` = number of propeller pairs (>= 3 in the tests).

If v_cm is *stable* (constant, including zero, across contact windows) the
system is behaving inertially: internal momentum exchanges between the island
and the propellers redistribute momentum without self-accelerating the COM.

**Code facts that ground the design:**
- W-island structure: `islandOf(w) = w / ISLAND_SIZE`, leader =
  `firstWOfIsland` (`simulation.h`); a complete island = `ISLAND_SIZE = n_i`
  layers with common affinity superposed at one centre, charge word per layer
  from the canonical seed (initSim.cpp:135: chief layer K, other layers S).
- Propeller (interaction.cpp): a pair (P) acting as a momentum carrier,
  `reloc != 0` pending; the move is consumed by `applyMomentum()` per light
  frame (`simulation.cpp`), which preserves the unit momentum direction m.
- Momentum exchange happens in encounter: the propeller x K and propeller x D
  branches add the pair's m into the target's `reloc`; D x D different-tribe
  repel does the same.  Collisions with the sieve open (S small) are the
  momentum channel; the overlap gate d < 2 RMAX governs whether a contact
  fires.

**System layout (tube, ELX long, short cross-section ELY=ELZ):**
- island: `n_i` layers (chief K + S) superposed at x_I with m = 0, spherical
  cluster of radius r_I;
- propellers: `n_pi >= 3` pairs (P) with pair momentum m = +1
  along x, placed left of the island on the long axis;
- measure x_e(t) for every element each light frame (lcenters of the island
  layers and of the pairs), compute x_cm, v_cm, and per-element velocities.

**User layout (Sep 2026, overrides the bullet above).** Everything starts in
one corner of the tube, in the *first eighth* of the long axis
(x in [0, LX/8]).  Within that small volume: the propellers (pairs, m -> +x)
on the left; the island on the right as a *spherical cluster* (chief K +
S layers, affinity/leader common, charge word per layer from the canonical
seed).  Radii and positions are pseudo-random (deterministic seed), with
radii proportional to the experiment scales (cluster radius and propeller
extent sized against RMAX and the first-eighth length so that propeller x K
and propeller x D contacts actually occur).  Motion is NOT free flight: it is
the result of the propeller x K and propeller x D interaction branches, where
the target receives the pair's momentum and re-emits at the contact point.

**Checks (incremental):**
1. no-contact isolation (S = 16384, propellers far): v_cm constant, equal to
   the mean of the initial momenta (n_pi/N for the layout above);
2. contact regime (S = 64): v_cm stays stable across the collapse/recoil
   bursts of section 12 (inertia evidence);
3. n_i sweep at fixed external impulse: the island's response (velocity
   change per propeller hit) decreases with n_i -> effective mass grows with
   island size.

**Relation to the manuscript (notation anchor).** The manuscript already
defines this observable in its "Propeller" subsection (doc/manuscript.tex):
"Let N be the total number of
bubbles composing the particle (the 3D island plus its pair dressing). If n
bubbles receive an impulse during one light frame, the mean centre of mass
advances by n/N cells per frame", with n <= N, and "Non-propeller pairs
behave differently: in K x K or D x D between different tribes the source
centres move one light-step away".  The tube experiment measures exactly
this: v_cm(frame) should equal n(frame)/N during a propeller kick and stay
stable (constant or zero) between kicks.  N here = n_i island layers + the
pair halves/elements counted once per propeller, as defined above.

**Scale target and island composition (user spec, Sep 2026).** For the
experiment to succeed it must use the largest feasible number of cells.
Working example: tube 32 x 32 x 512 with the island holding L/3 = 16 bubbles,
and the island elements charged as the *Orbis electron* (manuscript,
subsec. "The electron"): a set of -L fragments (negative singletons),
electron-neutrino pairs and a variable number of propeller pairs; Umbra would
invert all bits, but the first runs are Orbis-only.  Element count and charge
composition are therefore not "one bubble per layer with arbitrary charge":
they follow the manuscript's electron aggregate (island layers share the
common affinity/leader, charge words per the -L / nu_e composition).

**Engineering blockers (to resolve before/while implementing):**
1. *Even edges.*  The current tube allocator only accepts odd edges >= 5
   (parity of CENTER, source parity, sieve algebra).  The 32 x 32 x 512
   target is even; an even-edge path (per-axis centre definition) must be
   added and cube-verified before the target scale can run.
2. *Volume vs CPU runtime.*  A 32 x 32 x 512 tube has 524288 cells per layer;
   with W ~= 20+ layers the block is ~10^7 cells and a single tick costs on
   the order of a second on the current single-thread CPU kernel, while a
   light frame needs thousands of ticks.  At this scale the CPU path is not
   practical except for very short or sparse measurements.
3. *Island seeding.*  The 16-bubble island must reproduce the canonical
   W-island wiring (ISLAND_SIZE = 16, one K chief + 15 S layers) with the
   electron-of-Orbis charge words, embedded in the tube's first eighth.

**CPU scale decision (Sep 2026).** Start on CPU at the largest odd-edge tube
whose block keeps a light frame within ~1-2 s (target BLOCK <= ~5e4 cells,
e.g. LX = 51, short 7 x 7, W ~= 22: ~5.5e4 cells/frame-pass, frames of ~0.8 s
at LX=51 per section 10).  Validate v_cm vs n/N and the electron seed there,
then grow the edges gradually (short side up, LX up) as long as the runtime
stays practical.  32 x 32 x 512 remains the stretch goal (even-edge + GPU).

**Electron-of-Orbis seed card (provisional, from manuscript.tex:1240-1246).**
Fragment order is (q, w1, w0, c2, c1, c0); stored cell word ch is
(w1, w0, q, c2, c1, c0):

    -L      fragment 100000  ->  ch = 0x08   (Orbis, w1 = 0)
    -Lbar   fragment 001111  ->  ch = 0x17   (Orbis)
    nu_e    = one [+L : -L] pair (two complementary layers, kind P at contact)
    e^-     ~= quantized -L singletons + many nu_e pairs + propellers (fig3)

Provisional 16-layer electron island for v0: 1 chief (K) + 15 layers: a
majority of -L singletons plus a few nu_e half-layers, radii/positions
pseudo-random in the first eighth, charges per the card above; the exact
layer charge list and propeller gauge context are tuning knobs to iterate on
with the user before the production seed is frozen.

**v0 smoke (Sep 2026).** `inertia_probe` (build `build_inertia.bat`) runs on
the tube with `n_i` island layers + `n_pi` propeller pairs (each = 2
complementary layers with m = +1 along x) all seeded in the first eighth,
island as a pseudo-random spherical cluster (chief K + S).  First smoke:
LX=21, short 5 x 5, n_i=16, n_pi=4, 10 light frames:

- S = 16384 (gate closed): perfectly static, v_cm = 0 every frame (control).
- S = 64 (gate open): propeller x island contacts fire; at frame 4 the whole
  island cluster displaced collectively by ~+5.6 cells (v_Island = 5.625)
  while x_cm jumped +6.1, then the system kept exchanging (v_cm oscillates
  +1.5/-1.1 cells/frame).  CSVs: `inert_s16384.csv`, `inert_s64b.csv`.
- Census clean after the fix (field cells default to neutral ch = 0;
  earlier leftover canonical charges on unused layers were the artifact).

Open items before the n/N comparison: count n = island elements actually
kicked per frame, fix the pair-centre convention (each propeller counted
once), and run enough frames at each S to separate contact trains from the
steady-state COM drift.



### v0 measurement fix + dispersion finding (Sep 2026)

**Measurement bug found & fixed.**  The first per-frame displacement used a
per-element toroidal wrap (`|d| > ELX/2 => subtract ELX`).  But in this model
a bubble legitimately *relocates to the contact site* far away within one
light frame, so `|d| > ELX/2` happens for teleports, not only seam crossings.
Example (LX=21): island layer w6 moved x 2->13 (real +11) but was corrected
to -10; the corrupted sum reproduced dxIslandSum = +6 exactly where the true
sum was +90.  Displacement is now RAW; a `nSeam` column flags elements whose
raw |d| > ELX/2 (seam-vs-teleport ambiguity).

**Dispersion finding (LX=21, 5x5, n_i=16, n_pi=4, S=64).**  With the honest
measurement the apparent "collective island jump" of the first smoke
(v_Island ~ +5.6 at frame 4) is revealed as *dispersion of the outer S
layers*, not translation:

- chief layer (w0) stays at x = 1.0 for all frames;
- frame 4: 11 island layers scatter across the tube (sum dx = +90, 6 seam-
  ambiguous), bound-core count nBound drops 16 -> 8 while coreX stays ~1.6;
- frames 5-12: layers swing back and forth (dx -31, +31, -50, +54), the core
  reconstitutes (nBound back to 11-15).

Interpretation: with all 16 island layers seeded in a radius-1 ball at x=1 and
the 4 propeller pairs at x=2 (LX=21 first eighth is only 2 cells), the shells
(RMAX=2) all overlap from t=0 -> contact chaos; the particle does not behave
as a coherent mass.  The v_cm = n/N inertia test is not yet meaningful: n/N
assumes bounded impulses on a coherent particle, not layer dispersal.


### Canonical co-located seed + Delta-v_CoM summary (Sep 2026)

**Comparison with the reference implementations (E:\automaton):**
- Canonical island seed (`initSim::initCenters`) is the "Platonic premise": all
  source centres of an island are born **co-located at one point**, sharing
  leader/affinity; they separate only through interaction-driven relocation.
  The earlier inertia_probe "random ball of radius 1" seed was non-canonical
  and caused the observed layer dispersion (chief immobile, S layers flying).
- `tests/scatter_main.cpp` measures the interaction as a **centre-of-mass
  response**: per-frame velocities (shortest toroidal displacement) split into
  incoming/outgoing windows at closest approach and reports
  `|delta v_CoM|`.  inertia_probe now mirrors this: incoming window = frames
  before the first island kick; outgoing = after; plus nIsland/dxIslandSum
  bookkeeping so the manuscript prediction (dx = n cells when each kick moves
  one element) can be checked.

**Seed change.**  Island layers are now co-located at (xI, cy, cz) (same
charge 0x08, common leader, chief K at layer 0).  Propeller pairs are placed
on the left within pulse reach (D <= 2*RMAX), m = +1 x.

**Results (LX=27, cut 7x7, RMAX=3, n_i=6, n_pi=3):**
- S=16384 control: island at rest the whole run (nIsland=0, nBound=6).
- S=64 open: first kick at frame 4 (3 S-layers +4 cells, chief unmoved);
  frame 5 all 6 layers relocate far (+77, seam-ambiguous, chief to x=23);
  island net dx +90 cells vs "predicted 14" if every kick moved one cell ->
  the kicks at this tiny scale are contact re-emissions (reemitAtContact
  teleports) plus multi-cell reloc, NOT 1-cell n/N steps.  The baseline
  v_CoM "incoming" +0.70 is contaminated by the propellers' own self-dynamics
  (they form pairs and are consumed at t==RMAX and drift) - for the island
  alone the pre-kick baseline is exactly 0.

**Electron-composition cohesion test (Sep 2026).**  inertia_probe now accepts
n_pi = 0 (bare island) and an optional composition argument ("electron" =
-L singles incl. a K chief + nu_e dressing pairs [-L:+L] with +L = 0x37, the
word that satisfies canFormPair Rule 1 with -L = 0x08; "same" = legacy all
-L).  Result on LX=31 9x9 (RMAX=4), n_i=9 (3 singles + 3 nu_e pairs),
S=64, 8 frames: the island stays perfectly co-located (nBound=9, x=2.0 all
frames) -- BUT the encounter diagnostics show calls=0, s2B=0, pair=0,
self=0 for the whole run.  I.e. the composed island is *inert*, not *bound*:
no internal dressing ever forms because co-located same-phase island layers
never present simultaneously-active cells to the rotated-partner pass (the
partner content is a k-tick-delayed copy of another layer, so concentric
same-radius shells never overlap in the time-lagged partner).  Co-location +
encounter is therefore NOT the mechanism that binds an island in this model;
cohesion must come from the rest of the machinery (affinity/leader field,
polarization orbital walkers, charge census/turnaround) or from frequent
kicks whose n/N average keeps the composite together - which is precisely
what needs the large-W/real-scale runs that only the CUDA path makes
affordable.

**Output animation plugin (Sep 2026).**  inertia_probe now also writes a
per-light-frame layer dump "<csv>.pos" (header `P LX LY LZ RMAX W_USED n_i
n_pi`; per frame `F <frame> <tick>` plus one line per layer
`w cx cy cz kind m reloc t pB sB pair a`).  `python make_anim.py <file.pos>
<outdir> [px_per_cell] [fps]` renders a side view (core dots coloured by
kind - K red, S blue, D yellow, P cyan - plus breathing shell rings
min(t,RMAX) and a progress bar) to PPM frames and assembles `anim.gif` with
ffmpeg when available.  Cost is negligible (a few thousand pixels per light
frame; the run itself dominates).  Example:
`python make_anim.py anim_27.csv.pos anim27 8 1.5`.


### CUDA path audit (Sep 2026) - needed to run dense/long runs on GPU

Findings in E:\alpha (src/cuda, src/model/simulation.cpp):

1. **Cube-only.**  All kernels (phase_step_kernel, ca_update_kernel,
   rotatePartners/updatePartner kernels, dev lcenter indexing) address the
   lattice as `((x*EL + y)*EL + z)*W + w` from a single `automaton::EL`
   (`total_cells = EL^3 * W`).  The anisotropic tube (ELX/ELY/ELZ, per-axis
   wrap, per-axis loops added on the CPU path) is NOT supported by CUDA:
   a GPU tube requires porting the whole S1 slices 1-4 (per-axis indexing,
   per-axis wrap, wavefront limits, relocation) into the kernels.

2. **Light-frame granularity.**  `cudaSimulationStepWrapper()` runs a FULL
   light frame internally (FRAME kernel calls) and downloads the entire
   lattice to host once per frame (`downloadAndSync`).  So on the CUDA path
   one `automaton::simulation()` call == one light frame (not one tick).
   The GPU path also advances `pulse_tick += FRAME` itself and keeps host
   `lcenters` current via a per-center host/device exchange inside
   `cudaSimulationStep` (applyMomentum is re-implemented host-side there).

3. **Parity risk (gate).**  Host-only machinery that runs on the CPU path
   inside `update_lattice_cpu()` - `chargesSampleTurnarounds/Report` (read-
   only), `applyChargeConjugation` (mm), `polarization::tick` (helical
   orbital walkers) - is NOT invoked on the GPU path; the kernels carry
   their own phase/polarization logic (phiB/pB/sB, partner rotation).
   Before any GPU inertia/animation result can be used it must pass a
   **parity gate**: identical cube seed (e.g., the two-bubble scatter
   configuration or the co-located island) must reproduce the CPU frame-by-
   frame positions/kinds bit-for-bit (or at least quantitatively).

4. **Headless entry points available** (cuda_automaton.cu / cuda_api.h):
   isCudaAvailable, init_cuda_memory(EL,W), setCudaConstants,
   setCudaSourceCenters, uploadLatticeToCuda(CellDevice*,n),
   downloadLatticeFromCuda, cudaSimulationStep(one tick, full constants),
   free_cuda_memory.  bridge_cuda.cu is GUI-coupled (sinc_overlay, voxels,
   tomogram) and should NOT be linked into a headless probe; the headless
   CUDA host must replicate the conversions Cell<->CellDevice.

Plan:  M0 parity gate on a cube (scatter or island seed, CPU vs CUDA,
       frame-by-frame lcenters/kinds);  M1 tube support in the kernels if
       the tube geometry is required (S1-slices 1-4 port);  M2 inertia+
       animation harness on the CUDA path for dense/long runs.

### M0+M1 implementation roadmap (device per-axis port + parity)

Machine: GTX 1650 4GB present, nvcc v11.8 on PATH (Makefile points at
CUDA v13.2 toolkit).  Execution order (each slice ends with a compile gate
`nvcc -c` + full `nmake USE_CUDA=1` to keep the cube build green):

D1. constants: add `__constant__ unsigned dev_ELX/ELY/ELZ`; host entry
    `setCudaTubeDimensions(LX,LY,LZ)` (cuda_constants.cu); set dev_EL=ELX.
D2. index helpers: change `d_getCell` to
    `((x*dev_ELY + y)*dev_ELZ + z)*dev_W_USED + w` (matches host getCell
    spatial-major layout exactly).
D3. wraps: dev_spherical_wrap + d_getNeighbor per-axis modulo (ELX/ELY/ELZ)
    and antipode re-wrap per axis.
D4. kernel coords: phase_step_kernel and ca_update_kernel decompose linear id
    with per-axis strides: `w=id%W; sp=id/W; z=sp%ELZ; y=(sp/ELZ)%ELY;
    x=sp/(ELY*ELZ)`; totals = ELX*ELY*ELZ*W.
D5. per-axis mod in dev_moveOneStep/Away/reemit (shortest_delta mod ELX/
    ELY/ELZ) and SLOT4 centering/clamps per axis (mirror CPU S1).
D6. host totals & applyMomentum loop in cuda_automaton.cu per-axis;
    upload/download sizes from LX*LY*LZ*W.
D7. headless CUDA harness (experiments/cuda_inertia.cu, nvcc host + cl model
    objs, no bridge_cuda GUI deps): convert Cell<->CellDevice, seed identical
    to inertia_probe, drive frames via cudaSimulationStep x FRAME, per-frame
    center download for .pos/anim; parity flag vs CPU run.
D8. parity gate on tube seed (e.g., 27x11x11 electron island): frame-by-frame
    lcenters/kinds CPU vs GPU must match; if divergence appears in
    polarization/mm/charges host-only machinery, decide fix vs document.
D9. dense/long inertia+animation run on GPU; GIF via make_anim.py.

### CUDA D1-D3 done (Sep 2026) - compile gate passed
- D1: dev_ELX/ELY/ELZ constants; setCudaConstants now also sets them = EL
  (cube default unchanged); new extern entry `setCudaTubeDimensions(LX,LY,LZ)`
  (overrides dims + dev_CENTER = short-side centre + dev_EL = LX).
- D2: d_getCell index -> ((x*dev_ELY + y)*dev_ELZ + z)*dev_W_USED + w
  (identical to host getCell spatial-major layout).
- D3: dev_spherical_wrap (both stanzas) and d_getNeighbor clamps are now
  per-axis (ELX/ELY/ELZ).
- Single-file nvcc compile: EXITCODE=0 (cuda_automaton.cu, CUDA 13.2).
  NOTE: src/cuda is NOT tracked by git in E:\alpha - no baseline; proceed
  with extra care, consider committing or copying a backup.
- Next: D4 kernel coords + totals; D5 movement/SLOT4 per-axis; D6 host
  totals/applyMomentum; then D7 harness + D8 parity.

### CUDA D4 done (Sep 2026) - compile gate passed
- phase_step_kernel and ca_update_kernel now decompose the linear id with
  per-axis strides (w=id%W; z=sp%ELZ; y=(sp/ELZ)%ELY; x=sp/(ELY*ELZ)) and
  totals ELX*ELY*ELZ*W.
- Parallel-transport c[] deltas are per-axis (mod ELX/ELY/ELZ).
- Host: init_cuda_memory allocation and cudaSimulationStep total/grid now use
  LX*LY*LZ*W (fallback to cube EL when ELX.. not set).
- Single-file nvcc compile: EXITCODE=0.
- Remaining: D5 dev_moveOneStep/Away/reemit shortest-delta mods + SLOT4
  centring/clamps per axis; D6 applyMomentum host loop per-axis indexing/wrap;
  D7 headless harness; D8 parity.

### CUDA D5 done (Sep 2026) - compile gate passed
- dev_moveOneStep / dev_moveOneStepAway / dev_reemitAtContact now use
  per-axis shortest-delta mods (ELX/ELY/ELZ).
- SLOT4 centring uses per-axis halves and per-axis clamps.
- dev_phase_step_cell: spatial boundary test and the neighbour-u bounds are
  per-axis.  All `(int)dev_EL` device uses are gone (remaining dev_EL uses:
  constant def, kernel guards, dev_hash_random debug mod, and the setters).
- nvcc compile EXITCODE=0.
- Next: D6 - the host-side applyMomentum loop inside cudaSimulationStep
  (per-cell reads/writes and centre wrap still use cube L).

### CUDA D6 done (Sep 2026) - compile gate passed
- Host applyMomentum inside cudaSimulationStep now reads/writes the centre
  cells and partner cells with per-axis spatial strides (LY/LZ) and wraps the
  new centre per axis (LX/LY/LZ), mirroring CPU applyMomentum/wrapCoordAxis.
- No `(int)L)` cube indexing remains in the host step.  nvcc EXITCODE=0.
- Next: D7 headless CUDA harness; also audit updatePartnerKernel/
  rotatePartnersKernel for any residual cube indexing before D8 parity.

### CUDA D7 done (Sep 2026) - headless GPU harness runs on the tube
- experiments/cuda_harness.cu (nvcc) + build_cuda_harness.bat: seeds the
  host model (tryAllocateTube), full Cell<->CellDevice conversion (superset
  of bridge: bstamp/pol/leader_w/pair_count/reloc included), init CUDA
  + setCudaConstants + setCudaTubeDimensions, upload, FRAME x
  cudaSimulationStep per light frame (scenario parametrised, default 7 =
  the full-rules dev_encounter7 kernel), per-frame download + dump of the
  two source centres/kinds to <csv>.gpu.
- Smoke: `cuda_harness.exe 11 7 7 2 64 7 4 gh_smoke.csv` -> rc=0, 2 light
  frames in 0.04 s on GTX 1650 SUPER; per-axis dims set (ELX=11 ELY=7
  ELZ=7 CENTER=3); centres static at (4,3,3)/(6,3,3), kinds S.
- NOTE: GPU dispatch is scenario-specialised (dev_encounter0..7 selected by
  `scenario`; automaton.cfg default is 0 = NO interaction).  Scenario 7 is
  the large full-rules kernel.  D8 must confirm which scenario reproduces
  the CPU full encounter(), or extend the kernel.

### CUDA D8 done (Sep 2026) - parity gate: FAIL (bit-exact); qualitative only

Fix found first: the GPU sieve was HARDCODED to 16384 in dev_phase_step_cell
(SHELL_TARGET), so any GUI/GPU run with a different s2b_target effectively
never opened the interaction channel.  Added `__constant__ dev_S2B` + host
`setCudaSieve(S)` (default 16384 in setCudaConstants to preserve legacy); the
amplitude cap (also 16384) was kept as AMP_CAP.  Harness now calls
setCudaSieve(SIEVE).

Gate test (tube 27x11x11, two complementary bubbles 0x08/0x37 at SEP~6,
S=64, 20 light frames; cuda_harness cpu vs gpu, scenario 7):
- CPU: frames 1-6 at rest; frame 7 bubble0 -> x=1 kind P (pair formed,
  contact relocation); bubble1 stays S at x=16 through frame 20.
- GPU (scenario 7, sieve now 64): frames 1-7 at rest; frame 8 both bubbles
  -> kind P, bubble1 relocates to (6,6,6); later both P wander (x up to 9,
  y/z 0..10).
- Verdict: NOT bit-exact.  Qualitative agreement only (both open the pair
  channel on a similar timescale) but trajectories/kinds diverge from frame 7
  on.  Expected contributors: host-only machinery absent on GPU
  (polarization::tick helical walker, charges census, mm) plus ordering of
  the partner rotation / pair release.

Decision point: bit-exact GPU parity would require porting the host-only
polarisation/charge steps into the GPU path (large) OR treating CUDA as a
separate-but-qualitatively-similar engine for scale exploration only (then
D9 tube-scale inertia/animation can proceed on GPU with the caveat
recorded).  csv/log artifacts: cpu_d8.csv, gpu_d8.csv, gpu_d8b.csv.

### D9 regime gate (island electron, Sep 2026) - GPU FAILS; CPU island is coherent

Ran the actual inertia scenario (co-located electron island n_i=6: -L singles
+ nu_e dressings, + 3 propeller pairs) on tube 27x11x11 S=64, 4 light frames,
cuda_harness cpu vs gpu (scenario 7):

 frame  islandXmean(min,max) kinds      propellers
 CPU 1  2.0 (2,2)   K,S            x=1
 CPU 2  2.0 (2,2)   K,S,P          x=2     (dressing P forms)
 CPU 3  2.0 (2,2)   K,S,P          x=9.5   (propellers roam)
 CPU 4  2.0 (2,2)   K,S,P          x=15.8

 GPU 1  2.0 (2,2)   K,S            x=1
 GPU 2  2.0 (2,2)   K,S,P          x=1.5
 GPU 3  10.8 (3..26) S,D,P        x=11.7   (island DISPERSES, D delegates)
 GPU 4  15.8 (7..26) S,P          x=10.5

Verdict: the GPU does NOT reproduce the CPU regime for the island scenario:
on CPU the electron-composed island stays COHERENT at x=2 (dressing P forms;
propellers roam away) - a key positive CPU result (composition binds); on GPU
the island layers scatter across the tube (frames 3-4).  Therefore the GPU
cannot serve as a qualitative scale engine for the island-push animation until
polarization/charge host-only steps are ported (parity option 1).  CPU-scale
animations remain authoritative but wall-clock-bound (~6 s/light frame at
27x11x11 W=12).

Artifacts: ctl_cpu.pos, ctl_gpu.pos (island regime traces).
