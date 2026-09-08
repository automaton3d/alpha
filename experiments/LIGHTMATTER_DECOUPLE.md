# Light-matter decoupling candidate (colour + pB/sB stand-in)

## Hypothesis under test

The three colour bits give the initial symmetry breaking: the permanent
colour rule (`step = sign*(1-2*((ch>>axis)&1))`, `color_contact.h` /
`color_fsm.inc`) deterministically separates equal-charge sources along the
four toroidal body diagonals.  Sources that share a chief ("bound") remain
coincident at the centre: the matched attraction is zero at coincidence, so
the resting state collapses to an absorbing superposition (COLOR_MATCHED:
1 centre from frame 3 on).  The question is whether a light-matter channel
then decouples those still-superposed constituents.

## Provisional stand-in (real polarisation deferred)

The real pB/sB channel requires the polarisation broadcast, which cannot
bootstrap from the zero-polarisation seed (SEED_ASYMMETRY.md).  That part is
deferred.  For immediate use, each source receives a deterministic random
two-bit "light-matter label" (SplitMix64, same pattern as the coagulation
random experiment).  Under the `LIGHT_MATTER_FSM` macro, when two bound,
coincident, equal-charge constituents carry different labels, the matched
impulse (zero at coincidence) is overridden by one antisymmetric separation
step.  The rule is implemented as a hook in `color_fsm.inc` and in
`experiments/lightmatter_decouple.cpp` (no default-build change).

## Build and run

    experiments\build_lightmatter_decouple.bat
    build\lightmatter_decouple\lightmatter_decouple_basic.exe 1 12 basic
    build\lightmatter_decouple\lightmatter_decouple_dd.exe    1 12 dd

- `basic`: colour + chief + matched cohesion + decoupling (no D x D
  promotion / K x K clash) — the resting-plateau regime (8 K / 235 D).
- `dd`: basic + D x D promotion + K x K clash (coagulation macro set).

Outputs: `build/lightmatter_decouple/{basic,dd}_seed_1/` with trace.csv
(frame,K,D,S,centers,chief_centers,groups_of_three,unresolved,decouple_pairs),
groups.csv and labels.csv.  Labels are deterministic per seed; changing the
seed changes only the label assignment, not the rules.

## v1 result (measurement bug in the unresolved counter; rerun as v2)

v1 runs predate a fix to the `unresolved` counter (delegates without a valid
chief were not counted), so their member sums exclude ~162 unaffiliated D.

- `basic` (v1, 16 frames): K=8, D=235 constant from frame 2; centres
  9, 25, 44, 62, 77, 87, 83, 75, 71, 62, 52, 60, 52, 50, 51 over frames
  2..16 — i.e. no re-collapse to the absorbing single centre (baseline
  COLOR_MATCHED returns to 1 centre at frame 3).  8 chief groups of ~10
  members each had valid parents; the rest stayed D without a living chief.
- `dd` (v1, 16 frames): strong dispersal to ~180 centres with alternating
  K/D counts (44 K / 199 D, 201 K / 42 D, ...), 0..71 unresolved, and
  transient groups of population L/3=3; no persistent 1K+nD islands.

## v2 result (corrected census, seed 1, 12 frames)

Both runs exited zero with identity/transport checks green (charge, intrinsic
w, affinity, no P/m).  The `unresolved` counter now includes delegates without
a valid living chief.

### basic (colour + chief + matched + decouple, no promotion)

| frame | K | D | centres | unresolved | decouple_pairs |
|---|---|---|---|---|---|
| 2 | 8 | 235 | 9 | 0 | 822 |
| 3 | 8 | 235 | 25 | 0 | 110 |
| 4 | 8 | 235 | 44 | 0 | 71 |
| 6 | 8 | 235 | 77 | 0 | 33 |
| 8 | 8 | 235 | 83 | 0 | 69 |
| 10 | 8 | 235 | 71 | 0 | 69 |
| 12 | 8 | 235 | 52 | 0 | 93 |

Roles stay constant (8 K, 235 D) from frame 2 to 12 and every delegate keeps a
living chief (unresolved = 0 throughout).  The aggregate remains spatially
extended (centres oscillate 44..87) instead of re-collapsing to the absorbing
single centre of the baseline COLOR_MATCHED run.  Final frame: one chief
group of 33 members (11 seed families, 20 occupied sites, toroidal radius 4)
and seven groups of 30 members (10 families, 15-21 sites, radius 4-7);
sum = 243.

### dd (basic + D x D promotion + K x K clash)

| frame | K | D | centres | unresolved | groups size3 | decouple_pairs |
|---|---|---|---|---|---|---|
| 2 | 8 | 235 | 9 | 0 | 0 | 822 |
| 3 | 235 | 8 | 25 | 0 | 0 | 0 |
| 4 | 30 | 213 | 87 | 42 | 8 | 46 |
| 6 | 26 | 217 | 135 | 71 | 2 | 17 |
| 8 | 27 | 216 | 163 | 64 | 2 | 6 |
| 10 | 24 | 219 | 176 | 53 | 0 | 4 |
| 12 | 40 | 203 | 164 | 40 | 3 | 10 |

Promotion/clash turns the role population over every journey (K oscillates
~24..235) and leaves 40..71 unresolved delegates on the low-K frames; chief
groups fragment and re-form, with transient groups of population exactly
L/3 = 3.  The system is far from the absorbing plateau but does not reach a
stable 1K+nD census in 12 journeys.

## basic 32-frame stability run (seed 1)

`build/lightmatter_decouple/basic32_seed_1/` (audit: `analyze_lightmatter.py`).

- K=8, D=235, unresolved=0 on **every** frame 2..32.
- The **same 8 chiefs persist** over all 31 post-frame-1 samples — chiefs
  0, 3, 6, 9, 12, 15, 18, 21, i.e. the minimum W address of each of the
  seed's 8 charge words — with **exactly constant populations**: chief 0 has
  33 members, the other seven have 30 (std = 0.00), summing to 243.  The
  per-chief population equals the charge-word multiplicity of the seed, so
  N here is the seed composition, not a dynamical attractor.
- Attractor OLS over 240 consecutive points gives dN = 0.0000*N (r2 = 1.00):
  no restoring slope, N* undefined — affiliation is a fixed partition, not a
  homeostatic equilibrium.
- Meanwhile positions keep rearranging: occupied centres oscillate between
  38 and 87 over frames 7..32 with decouple_pairs between 33 and 822 per
  frame.  The light-matter stand-in therefore prevents the absorbing
  one-centre collapse while affiliation itself is frozen.
- No group of size L/3 = 3 ever appears in this regime.

Conclusion (basic): colour + light-matter decoupling yields a spatially
dynamic but affiliation-frozen state partitioned by the 8 charge words.  It
is robust (identical census for 32 journeys) but is not the predicted 9L
islands of L/3 constituents; producing those requires acting on the
family/copy partition (the seed has only 8 distinct charge words over 81
families), not only on superposition.

## Sparse local subset — the predicted micro-islands form (seed 1 and 2)

`mode=sparse8`: tube 41x5x5, W=24 = 8 families x 3 copies, one family per
charge word, family f (layers 3f..3f+2) at site (2+5f, 2, 2).  Site spacing
5 > 2*RMAX=4, so shells of different families never overlap; the only
contacts are inside each family.

Command: `lightmatter_decouple_basic.exe <seed> <frames> <tag> sparse8`.

Seed 1 (20 frames) and seed 2 (12 frames) — identical structure:

| frame(s) | K | D | S | centres | size3 groups | unresolved |
|---|---|---|---|---|---|---|
| 0-1 | 0 | 0 | 24 | 8 | 0 | 0 |
| 2..end | **8** | **16** | 0 | 8..24 (breathing) | **8** | **0** |

- Every frame from 2 onward has **exactly 8 chief groups of population 3**:
  `1K + 2D` per family, chief = the family minimum W address (0,3,...,21),
  single seed family per group (families=1).  Per-chief population = 3.00,
  std = 0.00, over 19 consecutive frames (seed 1).
- Positions breathe: each family's three copies alternate between a single
  superposed site (group sites=1, radius 0) and three separated sites
  (group sites=3, radius 2), driven by the decoupling kick; the group census
  never changes.
- Unresolved = 0 on every frame; identity/transport checks green.

This is the first configuration in the campaign that produces the predicted
`1K+nD` islands with n = L/3 - 1 = 2 robustly and repeatably (two label
seeds).  The necessary condition it isolates: families must start (or
become) spatially separated before election, so that cross-family merging
into the 8 charge-word tribes of the superposed seed cannot happen.

## Discriminating test: same-charge families in contact (duo2 / duo4)

`mode=duo2` (distance 2) and `mode=duo4` (distance 4): tube 21x5x5, W=6 =
two families of 3 copies that share the **same charge word** (island 0),
placed within shell reach (2*RMAX = 4).  This is the case the sparse runs do
not exercise: equal-charge families whose shells actually contact.

| mode | frame 2 | frames 4..16 | outcome |
|---|---|---|---|
| duo4 (d=4) | K=2, D=4, size3=2 (two islands of 3) | K=1, D=5, size3=0 | **merge into 1 island of 6 by frame 4**, then stable |
| duo2 (d=2) | K=1, D=5, size3=0 | K=1, D=5, size3=0 | **merge immediately in the election frame** |

Identity is therefore **not preserved under equal-charge contact**: at d=4 the
two family islands survive for one frame and then absorb into a single
1K+5D tribe; at d=2 they never even appear as separate islands.  The
light-matter stand-in decouples only bound, *coincident* pairs of the same
chief; it does nothing for coincident-but-unbound (different-chief)
same-charge families, so it cannot prevent cross-family absorption.

### Consequence for the sparse results

The 8/24/81-family "islands of exactly 3" of the previous section are
**geometry-guaranteed**: same-charge families are placed beyond shell reach,
so no cross-family election/clash can ever fire.  The isolated-family
1K+2D organisation is real and robust, but the *hard* part of the predicted
island population — keeping families separate when equal-charge families do
contact — is not solved by the current colour+decouple rules.  That is the
gap to attack next (e.g. decouple acting on coincident equal-charge contacts
regardless of chief, or family-local election before cross-family contact).

## Full-scale same-charge contact: spread81 (81 families, charge-major order)

`mode=spread81`: same 81-family tube (LX=85, W=243) but families are placed
in **charge-word-major** order (same-charge families adjacent, spacing 1 <=
2*RMAX), so equal-charge families are in contact from the first journey.
This measures the merging that `sparse81` geometrically excludes.

| frame | K | D | centres | groups of pop 3 | unresolved |
|---|---|---|---|---|---|
| 0-1 | 0 | 0 | 81 | 0 | 0 |
| 2 | 8 | 235 | 235 | 57 | 171 |
| 3 | 8 | 235 | 81 | 25 | 91 |
| 4 | 8 | 235 | 210 | 9 | 27 |

Final (frame 4) group census: 8 tribes whose chiefs are the charge-word
minima (0,3,...,21), each with population 27 from 9 absorbed seed families
(spread over ~21-26 sites, radius 10-13), plus 9 residual single-family
groups of 3 whose parent is not (yet) a K (unresolved = 27).  Sum 8*27+9*3 =
243.

Interpretation: at full scale, same-charge contact drives the partition from
81 family islands toward the 8 charge-word tribes of the superposed seed at
a measurable absorption rate (per tribe: 3 families at frame 2 -> 9 families
at frame 4; unresolved 171 -> 27).  The surviving "islands of 3" at frame 4
are unresolved clusters (chief not K), not protected islands.  Combined with
the duo test, this confirms: family identity is preserved only while
same-charge families are out of shell reach; contact merges them into
charge-word tribes under the current rules.

## Pauli-like identity hard core (EXCLUSION_FSM)

Candidate rule (`color_fsm.inc`, macro `EXCLUSION_FSM`): equal-charge
contacts between DIFFERENT seed families (w/3 — the L/3 identical copies of a
W-island) are not recorded for election/cohesion/colour kicks.  Families
cannot absorb each other into charge-word tribes; the copies inside one
family still interact and elect their own 1K+nD island.  This is a candidate
identity rule, distinct from the manuscript's `canElectChief` aggregation
semantics.

### duo2 / duo4 (same-charge families at d=2 and d=4, in contact)

With EXCLUSION_FSM both runs keep **K=2, D=4, two islands of exactly 3,
unresolved=0 for all 16 frames** (controls without the rule merge into one
1K+5D island at frame 2 (d=2) or frame 4 (d=4)).

### Superposed Platonic cube (L=9, W=243, all 243 copies at one centre)

| frame | K | D | S | centres | groups of 3 | unresolved |
|---|---|---|---|---|---|---|
| 0-1 | 0 | 0 | 243 | 1 | 0 | 0 |
| 2..12 | **81** | **162** | 0 | 1..9 (breathing) | **81** | **0** |

All 81 chiefs are the family minima (0,3,...,240), each with population
exactly 3.00 (std = 0.00) over 11 consecutive frames.  The EXCLUSION_FSM
makes the predicted quantization (9L islands of L/3 constituents, 81 x 3)
emerge directly from the fully superposed Platonic seed, with no sparse
geometry.  Residual limitation: the 81 islands remain largely co-located
(centres 1-9); identity quantization is robust, spatial localisation of the
islands is not yet (no cross-island repulsion/localisation mechanism).

### Exclusion + spatial push (EXCLUSION_FSM extension)

The EXCLUSION_FSM candidate additionally records cross-family equal-charge
contacts in a separate set and, at movement reconstruction, adds one
antisymmetric separation step per such contact (axis of largest
shortest-torus separation; W-derived axis when coincident).  Families are
pushed apart when their shells meet, in addition to never merging.

- duo4: **K=2, D=4, two islands of 3 for all 16 frames**, centres 4-5
  (families separated), unresolved=0.
- Superposed Platonic cube (L=9, W=243), frames 2..12: K=81, D=162,
  unresolved=0, size3=81 on every frame, and the occupied centres now spread
  spatially: 63, 97, 134, 162, 161, 163, 163, 165, 170, 159, 167 over
  frames 2..12 (chief centres 50-63).  All 81 chiefs (family minima) keep
  population exactly 3.00 (std = 0.00).

Interpretation: identity exclusion + shell-contact push turns the fully
superposed Platonic seed into the predicted 9L x L/3 quantisation (81 islands
of 3) that is both robust (fixed for 11 consecutive frames) and spatially
distributed (~160 occupied sites).  Both ingredients are experimental
candidate rules (macros in color_fsm.inc), not current model rules; the
exclusion changes the manuscript's equal-charge cross-family aggregation
semantics (canElectChief), and the push is a Pauli-like hard core, not the
photon-mediated electric repulsion of the model.

### spread81 with EXCLUSION+push (full-scale same-charge contact)

`mode=spread81` (families ordered by charge word, same-charge adjacent at
spacing 1) with EXCLUSION_FSM + spatial push, 6 frames:

| frame | K | D | centres | chief centres | groups of 3 | unresolved |
|---|---|---|---|---|---|---|
| 2 | 81 | 162 | 201 | 65 | 81 | 0 |
| 3 | 81 | 162 | 170 | 73 | 81 | 0 |
| 4 | 81 | 162 | 189 | 68 | 81 | 0 |
| 5 | 81 | 162 | 197 | 78 | 81 | 0 |
| 6 | 81 | 162 | 198 | 75 | 81 | 0 |

Control (spread81 without the rule) merged toward the 8 charge-word tribes
(K=8, D=235) with unresolved 27..171.  With EXCLUSION+push the predicted
quantisation (81 islands of exactly 3, population std = 0.00) survives at
full scale while the same-charge families are adjacent and in contact, and
the islands are spatially distributed over ~170-200 occupied sites.

### Production-path port (interaction.cpp, EXCLUSION_FSM)

The identity hard core and frame-edge separation push were ported to the
ordinary `simulation()` path (macro-guarded in `interaction.cpp`: encounter
gate before `chiefContact`; `resolveExclusionPush()` at the light-frame edge;
default builds unaffected).  Build: `build_island_census_exc.bat`; run:
`build/island_census/island_census_exc.exe 12 16384 build/island_census/prod_exc`.

Superposed Platonic cube (L=9, W=243), frames 2..12 (run
`build/island_census/prod_exc/census.csv`):

| K | D | centres | groups | unresolved | mixed charge |
|---|---|---|---|---|---|
| **162** | 81 | 15-19 | 162 | 0 | 0 |

Every frame is identical after the first interaction journey.  The
cross-family charge-word tribes are gone (production without the rule: K=235,
D=8, 1 centre) and the 81 seed families are never mixed (unresolved = 0,
mixed charge = 0).  Two differences from the reduced colour-FSM candidate:
the production D x D promotion yields **2 K + 1 D per family** (162 K / 81 D,
two chief groups per family) instead of 1 K + 2 D, and the exclusion push
keeps the clusters at only ~15-19 occupied centres (weak spatial
localisation), because contact events are rarer and the absorbing layout
settles early.

## Interpretation and limits

- The provisional light-matter stand-in (random per-source labels replacing
  the deferred pB/sB polarisation channel) combined with the colour rule
  breaks the absorbing superposition: in the `basic` resting regime the
  8K/235D aggregate stays spatially extended (~50-90 centres) with full
  affiliation (unresolved = 0) for 12 journeys, whereas COLOR_MATCHED
  without decoupling returns to a single centre at frame 3.
- It does not produce the predicted 9L islands of L/3 constituents: `basic`
  yields one ~30-member tribe per charge word (the seed's 8 charge words),
  not 81 three-member islands; `dd` adds chief turnover that prevents any
  stable census.
## Campaign summary and open questions

This file is an experiment log of the island-quantisation campaign (all
candidates are macro-guarded experimental rules; ordinary builds and the
manuscript are unaffected).  In sequence:

1. Production path, superposed Platonic seed: absorbing plateau 235 K / 8 D at
   one centre (census run, 64 frames).
2. Colour + light-matter stand-in (random pB/sB labels): breaks the absorbing
   collapse; affiliation freezes into 8 charge-word tribes (8K/235D,
   ~30 members each, stable 32 frames).
3. Sparse geometry (families out of shell reach): the predicted micro-islands
   form (8, 24 and 81 families -> one 1K+2D island of exactly L/3=3 each),
   but this is geometry-guaranteed, not dynamical.
4. Same-charge contact without protection (duo2/duo4, spread81): families
   merge into charge-word tribes (K=8, D=235), showing identity is not
   preserved under contact by the colour/decouple rules alone.
5. EXCLUSION_FSM (Pauli-like identity hard core) + spatial push (reduced
   colour FSM): the 81x3 quantisation becomes robust even from the fully
   superposed seed and under same-charge contact, with ~160-200 occupied
   centres in the cube/spread runs; duo2/duo4 no longer merge.
6. Production-path port of EXCLUSION_FSM: cross-family tribes disappear
   (K=162, D=81, 81 family clusters, unresolved=0, stable 12 frames), but the
   production D x D promotion yields 2 K + 1 D per family and the push gives
   only ~15-19 centres (weak spatial localisation).

Open questions / next candidates:

- Reduce the production intra-family promotion to a single K per family
  (1 K + 2 D) without re-enabling cross-family absorption.
- Strengthen spatial localisation in the production port (dedup push per
  light frame at encounter level; tune the separation step).
- Replace the random light-matter stand-in with the real pB/sB channel
  (polarisation bootstrap from an initial axis) and test whether the
  photon-mediated electric repulsion between dressed islands preserves the
  same quantisation (the model's "complete-system" version of this story).
- Reconcile the identity hard core with the manuscript's equal-charge
  cross-family aggregation semantics (`canElectChief`): the candidate is a
  deliberate departure, and its physical justification (a conserved family
  quantum number / Pauli-like exclusion among identical copies) is not yet
  established.


