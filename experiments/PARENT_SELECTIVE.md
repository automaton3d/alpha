# `PARENT_SELECTIVE_FSM` -- breaking the cap of 2: implemented, measured, and the piece that is missing

**Rule set specified by the author (2026-09-19), restated precisely.**

| contact | required behaviour | code |
|---|---|---|
| `S x S` (same charge) | one becomes `K`, the other its `D` | unchanged (T1, `chiefContact`) |
| `D x D` same island | they stay delegates -- **cohesion**, no transition | T2 disabled |
| `D x D` different islands | **one step apart per pair per light frame** | new, see below |
| `K x K` (same charge) | one demoted to `D` and the two **agglutinate** (stay close) | T3 unchanged + no push applied |

The discriminator is the **dynamical parent identity** (`islandChief`, the emergent membership) and never
the seed family: this is what separates this candidate from the retired `FAMILY_SELECTIVE_FSM` /
`EXCLUSION_FSM` line, which keyed on `w/3` and therefore on the declared partition.

**Implementation (all inside `#ifdef PARENT_SELECTIVE_FSM`, OFF in the reference build).**
`chief_transition.h:17-26` disables `promotesDelegate` (T2); `interaction.cpp:140-145` declares the
pair list `parentRepel`, cleared per tick (`:545-547`, `:576-578`) and at reset; `:1607-1630` records
each equal-charge `D x D` pair whose two `islandChief` differ; `:788-819` is
`resolveParentRepulsion()`, which pays **one antisymmetric unit step per pair per light frame**
through `reloc[]` exactly like the exclusion push, skipping any pair that has become one island by the
frame edge; `:907-909` calls it at the frame edge next to `resolveExclusionPush()`.
Build: `experiments/build_island_census_parent.bat` (a **new** programme, not one of the eleven retired
`L/3` routes). **Inertness proof:** with the macro undefined the preprocessed `interaction.cpp` is
byte-identical to `HEAD` after stripping `#line`/blank lines (verified 19 Sep 2026), so no reference
number can move.

## Measured, L=9, sieve 16384, frame 2 (the frame in which the cascade happens)

| build | K | D | groups | unresolved | centres | max_pop | max_span | births | size histogram | verdict |
|---|---|---|---|---|---|---|---|---|---|---|
| reference | 235 | 8 | 235 | 0 | **1** | 2 | 0 | 235 | 1:227, 2:8 (one per word) | the old plateau |
| fsm only | **1** | **242** | 1 | **241** | **57** | 2 | 4 | **1** | collapse | cap gone, structure broken |
| fsm + absorb | 1 | 242 | 1 | 241 | 57 | 2 | 4 | 1 | collapse | **absorb changes nothing** |
| fsm + absorb + **nomination repair** | **162** | **81** | **162** | **0** | **57** | 2 | 5 | 162 | 1:81, **2:81** (spread 0) | **healthy, resolved, spatially separated** |

**The cap is gone** (no promotion can eject a member) and **the spatial separation happened**
(`distinct_centers` `1 -> 57`, and it comes from a rule that reads only the emergent parent).

**Where the orphans actually came from -- not where I first assumed.**  The first fix targeted `K x K`
fusion (`PARENT_FUSION_ABSORB`), and the instrumentation showed it is *inert here*: `K x K`
encounters are **zero** because only one chief ever exists in the fsm-only build (the diagnostic
`pselKClashes = 0` at every frame edge, and `births = 1`).  The 241 orphans come from a different
place: the generic merge of `chiefContact` names as parent the **minimum-W address of two `S`**, which
is not yet a chief, and T4 can also copy a `D`'s parent.  In the reference path the **T2 promotion
cascade repaired those pending nominations globally**; with T2 disabled nothing repairs them, so they
stay `unresolved` forever and the structure freezes at 1 chief.

**The repair that completes the rule set (`PARENT_NOMINATION_REPAIR`, frame edge):** any source that a
delegate names as its parent but that is not a chief is **promoted** -- which is exactly what T1/T4
intended ("two `S` meet, one becomes `K`, the other its `D`") and what T2 was doing globally.  One
pass suffices.  Measured at the frame-2 edge: `pending parents promoted = 161`, `unresolved: 241 -> 0`,
`K: 1 -> 162`, and the population becomes a *uniform* spectrum: **81 bound pairs and 81 singletons**,
all non-singleton islands of size exactly 2 (`spread = 0`), every one of the 8 charge words carrying
one, with the islands still spread over 57 distinct centres.

**What this does and does not show.**  It shows that the rule set *can* be made self-consistent with a
single frame-edge rule, and that the result is the first island population in this project that is
simultaneously **resolved** (no orphans), **uniform** (spread 0) and **spatially separated** (57
centres, against 1 in the reference).  It does **not** yet show growth past 2: `max_population = 2`
still, because by frame 2 no `S` is left to join a group.  And the count `162 = 2 x 81` suggests the
pattern is **per seed family** (81 families of 3 copies -> 2 K + 1 D each), not per charge word as in
the reference (8 pairs, one per word) -- which makes the pattern potentially **seed-multiplicity
dependent**, the opposite of what the reference showed.

**The decisive next test (registered).**  Run the same three builds at `L=15`, where the seed
multiplicity is 5 instead of 3.  *If the pattern tracks the seed's multiplicity* (e.g. `K = 2 x 135`
with one delegate per family, or any `ell`-dependent rule), then the regularity is a **partition
effect** and the candidate is in the same class as the retired `L/3` routes.  *If the per-family
count is instead independent of `ell`* (as the reference's one-pair-per-charge-word is), then the rule
set produces something the seed did not put in.  Cost: `L=15` is ~31 min per light frame
(`L^5` scaling), so this is a background job -- 2 frames is about an hour.

**The release companion: built, and inert at this timescale -- with a hard number.**  The companion
`PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR + SURFACE_ESCAPE_FSM`
(`build_island_census_parent_escape.bat`) was measured at `L=9`: frame 2 is *identical* to the repair
build (`162 K + 81 D`, 162 groups, 0 unresolved, 57 centres, `max_pop 2`) with **`escapes = 0`**.  The
reason is a timescale inside the rule: `applySurfaceEscape()` releases a delegate only when
`++framesWithoutContact[w] >= W_USED` -- `W_USED` **consecutive light frames without a same-charge
contact** (`interaction.cpp:762-764`), i.e. **243 frames at `L=9`** (675 at `L=15`).  At ~50 s per
light frame that is **~3.4 h** of run time before a single release can fire.

**This also explains a standing observation of the project.**  The flux harness found `escapes = 0` in
*every* reference run (4 to 64 frames, `quantization/FINDINGS.md`).  It was read as "the plateau is
frozen"; the code adds a second, independent reason: **the release rule's own timescale is `W` light
frames -- longer than any run the project has made.  Inside such runs a *frozen plateau* and an
*inoperative release rule* are indistinguishable, which means the flux harness's INCONCLUSIVE verdict on
four archived builds was partly a run-length artefact and not only a statement about the dynamics.

## The placed multi-island fixture: the first controlled turnover, and a structural tension

The fixture the balance needs is fixed by four conditions: at least two islands separated beyond the contact
range (so cross-island `D x D` pairs exist and delegates can lose contact), free `S` in contact with each
chief (so recruitment can fire), small `W` (a 2-6 frame timer instead of 243), and enough frames for several
release/recruit cycles.  `island_rate_probe placed FRAMES CSV` plants exactly that in a 21x5x5 tube
(`RMAX = 2`, contact range 4, `W = 6`):

    w0 K_A@2   w1 D_A@9   w2 K_B@18   w3 D_B@10   w4 S1@5   w5 S2@15    (all ch = 0x08)

| arm | islands | captures | escapes | releases (D->S) | promotions | demotions | recruits |
|---|---|---|---|---|---|---|---|
| `ref` | **3** | 2 | 1 | 0 | **2** | 0 | 2 |
| `cand` | 2 | 2 | 0 | 0 | 0 | 0 | 2 |

**The reference turnovernises** (2 promotions + 2 recruits, and its third island is born from the promotion
of a stray delegate) -- the first controlled turnover the campaign produced.  **The candidate recruits but
never loses anyone.**  Neither arm releases anything, and two structural facts explain it:

1. **The escape timer is reset by contact with ANY same-charge source, not only with its own group.**
   `contacted[w]` comes from `surfaceContactSeen`, which counts any same-charge contact
   (`interaction.cpp:141-145`, `:763`).  A stray delegate that happens to sit next to *another* island's
   bubble therefore never expires: in this fixture the reference's promoted stray (x = 9) sits one cell from
   `D_B` (x = 10) and resets its timer for the whole run.
2. **The candidate's repulsion HEALS strays back into their own islands.**  With the two strays between the
   two chiefs, pushing them apart from each other pushes each one toward its own chief (`D_A` from x = 9
   back toward `K_A`@2; `D_B` from x = 10 back toward `K_B`@18), so the repulsion reunites the island
   instead of freeing the member.

**The structural tension.**  Releasing requires isolation from every same-charge source for `W` frames;
recruiting requires contact with a chief.  Isolation and contact are mutually exclusive unless something
TRANSPORTS the bubble between the two states -- and a released singleton has `m = 0` (its own inertia does
not move it, J0), so locomotion cannot come from the freed bubble.  **The cycle release -> recruitment
therefore requires a CARRIER**, which is what the drive pairs supply (0.25 cells per light frame, measured).
This, and not the population dynamics, is why the flux harness never saw turnover and stationarity together
in any archived dataset: none of them contained a transporter between the two regimes.


## The driven fixture: turnover achieved, the first real rate fit, and it fails as registered

`island_rate_probe driven FRAMES CSV` adds the carrier the tension requires: a reciprocal drive pair
affinity-matched to island B, `m` along `-x`, which walks B across the gap at 0.25 cells per light frame
(island A carries a different affinity so the pair can dress only B).  **Measured, 60 light frames:**

| arm | islands (end) | captures | escapes | releases (D->S) | promotions | demotions | recruits | verdict |
|---|---|---|---|---|---|---|---|---|
| `ref` | 3 | 1 | 2 | **1** | 1 | 0 | 1 | release YES, turnover YES |
| `cand` | **1** | 4 | 3 | **1** | 0 | 1 | 1 | release YES, turnover YES |

**Both arms now release *and* recruit** -- the first configuration in the project where the release ->
recruit cycle closes, and it closes exactly as the structural tension predicted: the pair carries B into
contact with the bubble that island A released.

**Then the flux harness was run on it (`quantization/quantize_stdlib.py ... --by-run`) -- the first time it
has ever received data with turnover.**  Sections:

* *Sanity*: `driven_cand` 2 islands, 29-39 frames/island, `sum(cap)=4 sum(esc)=3`; `driven_ref` 2 islands,
  6-61 frames, `sum(cap)=1 sum(esc)=2`; **islands with zero escapes = 0** in both.  The frozen-mode warning
  that fired on every archived dataset does not fire.
* *Binned fluxes*: `N=1`: cap 0.000 / esc 0.035; `N=2`: 0.077 / 0.077; `N=3`: **0.130 / 0.000**.  Mean
  `dN`: `N=1` `-0.03`, `N=2` `0.00`, `N=3` `+0.13` -- monotone increasing, **no zero crossing**.
* *Drift fit*: `a = -0.1156`, **`b = +0.0754`** over 135 island-frames -> `b >= 0`, no restoring drift, no
  `N*`.  Per run: `cand` `b = +0.0923`, `ref` `b = +1.0303`.
* *Verdict*: **FAIL (no restoring drift)**.

**Reading.**  The structural hurdle is passed (a release -> recruit cycle now exists and is measurable), but
the population balance is **runaway**: recruitment outpaces release, bigger islands grow faster, and the
candidate ends the run with a single island.  This is the failure mode the note registered in advance --
*"falsifier: if it collapses into one island per charge word while the repulsion is on, then fusion wins and
the repulsion must become size-dependent (or range-limited) before any equilibrium exists"* -- and it is the
same sign and shape as the directional-channel candidate (`b = +0.49`).

**Therefore the next ingredient is a size-dependent brake, not another channel:** the repulsion (or an
equivalent loss term) must grow with the island's member count, so that `Gamma_esc(N)` eventually overtakes
`Gamma_cap(N)`.  Candidates to compare, cheapest first: (i) the number of repulsion steps paid per light
frame proportional to the number of *delegates* rather than one per pair; (ii) a threshold (`|J|`-style, per
the spin programme's S4) with the cap from the cavity radius; (iii) release triggered by size rather than by
the contact-free timer alone.

## Brake (i): size-proportional repulsion DISPLACEMENT is falsified by the model step invariant

The first brake was `PARENT_REPULSION_PRESSURE` (nested inside `PARENT_SELECTIVE_FSM`): the per-pair impulse
magnitude became the number of *delegates* in the two islands (`nA + nB`, the pair itself counting two),
clamped per source to the contact range `2*RMAX`, so that an island's ejection pressure would grow with its
own membership.  It is preserved verbatim in `attic/brake1_pressure_block.txt`, and it does not survive
contact with the harness:

    [pressure] frame-edge: pairs=1 cap=4 max_push=2 clamped=0        (frame 1, placed fixture)
    Assertion failed: length<=1, file E:\alpha\experiments\inertia_fixture.h, line 95

`inertia_fixture.h:95` asserts that **every constituent, P halves included, moves at most ONE cell per light
frame** -- `m` is a direction, not a jump.  With one delegate per island the brake already asks for two
cells, so the `press` arm aborts in frame 2 while `ref` and `cand` complete 40 frames on the same fixture.
The limitation is structural, not a tuning problem: *any* magnitude above one cell violates the invariant, so
**size-proportional displacement is not expressible in this lattice**.  A size-dependent loss has to come
from a size-dependent *transition* (a release), not from a longer step.

Secondary but worth recording: on the `driven` fixture the brake is inert by construction --
`[pressure] frame-edge: pairs=0` at every frame edge, because the two islands' delegates never touch there
(island B's delegate travels with its chief).  The `press` arm reproduced `cand` exactly on that fixture
(`4 captures, 3 escapes`, same mechanism counters), which is a clean control: no cross-island `D x D` pair,
no brake effect.

**Consequence for the brake menu.**  Candidate (i) is dropped.  Both remaining candidates avoid the invariant
because they act on *transitions* rather than on displacement: (ii) a `|J|`-style threshold with the cap
taken from the cavity radius (the spin programme's S4), and (iii) a release trigger that depends on island
size rather than on the contact-free timer alone.  The parameter-free form of (iii) suggested by the geometry
is the island's own radius: a delegate farther from its chief than the contact range `2*RMAX` is outside the
shell cohesion can hold, so a growing island sheds its outer members while a small one does not -- size
dependence with no new knob, using the model's own length scale.

**Tree state.**  The pressure variant was removed from `src/model/interaction.cpp` (reference
pre-processed output re-verified byte-identical to `HEAD` after the revert) and the third build arm was
dropped, so `build_island_rate_probe.bat` is `ref` + `cand` again.  The one-line orphan fragment left in
this note by the earlier section reorder ("every same-charge source until their timers expire.") is removed
in the same edit.

## Brake (iii), parameter-free form: the shell release fires and limits the island, but is blind to the co-located regime

`PARENT_SHELL_RELEASE_FSM` implements the brake on a TRANSITION, where the size dependence is free: a
delegate whose torus distance to its chief exceeds the model's own contact range `2*RMAX` is outside the
shell cohesion can claim and is released.  It reads only `lcenters`, `parent` and `RMAX` -- no seed
partition, no `ISLAND_SIZE`, no `L` -- and it is independent of the contact-free timer (which needs
`W_USED = 243` light frames in the cube and therefore never fires in a feasible run).

**Rate probe, `placed` fixture (40 frames).**  The brake fires where the strays are: `[shell] frame-edge:
range=4 max_dist=8 releases=2` on frame 1, then `max_dist <= 1` and no further releases.

| arm | islands | captures | escapes | releases (D->S) | promotions | demotions | recruits |
|---|---|---|---|---|---|---|---|
| `cand` | 2 | 2 | **0** | **0** | 0 | 0 | 2 |
| `shell` | 2 | 3 | **2** | **2** | 0 | 0 | 3 |

So the intended transition is real: a loss channel that did not exist in the completed rule set.

**Rate probe, `driven` fixture (60 frames).**  `shell`: captures 4, escapes 3, releases **2** (was 1 for
`cand`), promotions 0, demotions 1, recruits 2.  Fed to the flux harness (`driven_ref` vs `driven_shell`):
sanity passes (zero islands with zero escapes), binned mean `dN` `N=1` `-0.04`, `N=2` `+0.07`, `N=3`
`+0.05`, still no zero crossing; drift fit `a = -0.0811`, **`b = +0.0525`** over 130 island-frames (was
**`+0.0754`** for `ref` vs `cand`); verdict still **FAIL**.  **The brake moves the balance in the right
direction but does not restore it.**

**Cubic census, L = 9 (frames 0-4): blind while the island is co-located, a size limit once it has
 extent.**  The brake cannot fire while cohesion is geometrically trivial, and the diagnostic says so:
`[shell] frame-edge: range=8 max_dist=0 releases=0` at the first two frame edges, with frames 1-2
byte-identical to the same rule set without the brake.  The state then reorganizes, the delegates acquire
 extent, and the brake acts:

| frame | `repair` (no brake) | `shell` (+ brake) |
|---|---|---|
| 2 | 162 K, 81 D, 0 S; 57 centres; **max_pop 2**; span 5 | identical (`max_dist = 0`) |
| 3 | 8 K, **235 D, 0 S**; 59 centres; **max_pop 33**; captures 235 | 8 K, **163 D, 72 S**; 59 centres; **max_pop 24**; captures 163 |
| 4 | (not run) | 40 K, 154 D, 49 S; 61 centres; **max_pop 18** |

`[shell] frame-edge: range=8 max_dist=12 releases=72` on the frame-3 edge, and 49 releases on the frame-4
edge.  **The brake does not merely add a transition: it limits the largest island (33 -> 24 -> 18) and
returns the members it removes as singletons.**  That is the first rule in this campaign that limits a
growing island population in the cubic census -- the runaway the rate probe measured (one large island that
keeps recruiting) is precisely what it cuts down.

**What it cannot do, stated precisely.**  Until the frame-2 -> 3 reorganization every delegate sat exactly
on its chief (`max_dist = 0`), so at frames 1-2 no geometric rule could have acted: an island whose members
share one cell has no extent to measure.  `parent` is a global label, so an island can hold an unbounded
population in ONE cell, and the runaway measured by the rate probe is population growth of that kind.  The
shell brake limits an island once cohesion acquires extent; it is blind to the co-located regime, which is
the regime the reference census lives in for its first two frames.

**Consequence for the remaining candidate.**  The `|J|` ceiling (candidate (ii)) is measured inert in the
same census: `J = sum r x m` vanishes whenever every `r = 0`, and the census prints `max_Jmag2 = 0` at every
frame, frame 3 included.  So of the two surviving geometric candidates only the shell release acts, and it
acts on EXTENT, not on count.  A rule that limits by counting members remains the only route to a
size-dependent loss for a co-located island.  That is not exotic -- the model already selects populations
with a count (`<= 2`, one group of 2 per charge word) -- but a count threshold is one step away from a
DECLARED knob, the `L/3` lesson; any such rule must register its prediction before it is run and must be
checked against the trap that closed the old route (the selected population may not track the seed
multiplicity).

**Tree state.**  `PARENT_SHELL_RELEASE_FSM` in `src/model/interaction.cpp` (placed OUTSIDE the
`SURFACE_ESCAPE_FSM` guard -- the first placement put it inside, which the census build caught as
`error C3861: applyShellRelease: identifier not found` when built without that macro); third probe arm
`rb_shell.exe` and census arm `build_island_census_parent_shell.bat`; reference pre-processed output
always re-verified byte-identical to `HEAD`.
**Pre-registered prediction (written BEFORE the extended runs were launched).**  Frames 0-4 showed
`max_pop = 33 -> 24 -> 18` with the brake on and `32 pp` above the reference without it.  The extended
`L=9` census (10 frames, `shell` and `repair` side by side, same sieve 16384) is registered against three
outcomes:

* **P1 -- size-selected plateau.**  `shell`'s `max_pop` stops falling and holds a value `> 2` for at least
  three consecutive frames while `repair` stays far above it.  That would be the first size-selected
  plateau this campaign has measured, and the value becomes a candidate `N*` whose dependence on `L` can
  then be tested.
* **P2 -- no brake.**  `shell` and `repair` converge, or keep falling together to `max_pop = 2` (or 1).
  The brake then only delays the collapse and the size limit is not a selected state.
* **P3 -- collapse by shedding.**  `shell` falls to `max_pop = 2` while `repair` stays large.  The brake
  then acts as a hard cap rather than as a balance, and the flux harness (not the census) remains the
  instrument for `N*`.

**Registered prediction: P2 or P3, not P1** -- the "extent, not count" finding says the brake acts on a
geometric quantity that the reference geometry does not maintain, so a plateau would require the dynamics
to *regenerate* extent continuously rather than produce it once in the frame-2 -> 3 reorganization.  If P1
appears anyway, that regeneration is the discovery, and it is falsifiable by the `L=15` arm (the plateau
must then track `L` through `2*RMAX`).


## Ten-frame census: the registered prediction is falsified in letter, and the spectrum says why

Frames 0-10, same sieve, `shell` and `repair` side by side (both runs reproduce frames 0-4 exactly, which
is also a reproducibility check on the 4-frame run):

| frame | `shell` max_pop | `repair` max_pop | `shell` caps / esc | `repair` caps / esc |
|---|---|---|---|---|
| 3 | 24 | 33 | 163 / 235 | 235 / 235 |
| 4 | 18 | **33** | 58 / 35 | **0 / 0** |
| 5 | 23 | 33 | 62 / 80 | 0 / 0 |
| 6 | 26 | 33 | 65 / 19 | 0 / 0 |
| 7 | 24 | 33 | 40 / 71 | 0 / 0 |
| 8 | **26** | 33 | 37 / 15 | 0 / 0 |
| 9 | **26** | 33 | 0 / 3 | 0 / 0 |
| 10 | **26** | 33 | 30 / 4 | 0 / 0 |

**P1 appeared, so the registered prediction (P2/P3, not P1) is falsified in letter.**  `shell` holds
`max_pop = 26` for the last three frames while `repair` sits at 33 -- and `repair` is FROZEN from frame 4
on: seven consecutive frames with exactly zero captures, escapes, births and deaths.  `shell` keeps turning
over throughout (releases 3-72 per frame, `max_dist` 10-12 at every edge).  **The brake does something no
other candidate has: it keeps the cubic census out of the frozen state.**

**The size spectrum, however, says the plateau is not a selected quantum.**  At frame 10 the `shell`
spectrum is `1:1 2:2 3:7 6:1 22:1 26:7` -- seven islands of exactly 26 plus debris -- with
`spread(max-min) = 24`, i.e. NOT "approximately equal sizes".  The `repair` spectrum at the same frame is
`30:7 33:1`, `spread = 3`.  **So by this note's operational reading of the phenomenon (same charge,
approximately equal sizes, one island per charge word) the FROZEN build is more quantised than the build
with the brake**; what the frozen build lacks is movement -- its equality is static, the members never
leave.

Both builds condense to one island per charge word (`8 of 8 reachable words`, `mixed_charge = 0`) with
sizes of order `243/8 ~ 30`.  The brake does not select a small size: it converts a static condensate into
a dynamic one plus debris (`7 x 26 + 22 + nine small islands = 243`).  The 26 is the condensation level
after shedding, not a preferred quantum, and the honest verdict for brake (iii) is **it buys dynamics, not
quantisation.**

**What would separate a selected size from a condensation level.**  A selected `N*` must be independent of
`W / N_words` -- it must survive a change in seed multiplicity -- and it must appear as a peak far below
the condensate.  Neither is measured here.  **Prediction registered for the `L=15` arm** (243 -> 675
sources, still 8 charge words, `2*RMAX` 8 -> 14): if the mode simply tracks `W/8`, the plateau is
condensation and the campaign's size limit remains geometric bookkeeping; if the mode moves with `2*RMAX`,
the brake is selecting EXTENT and the plateau is a real (if not yet charged) quantum.

## Diagnostics the builds print (keep them; they found the mechanism)




    [absorb]     frame-edge: K x K encounters=N pairs=M delegates re-pointed=P
    [nomination] frame-edge: pending parents promoted=P
    [shell]      frame-edge: range=2*RMAX max_dist=D releases=R      (PARENT_SHELL_RELEASE_FSM)

`K x K encounters = 0` at every frame edge in the fsm-only build is what redirected the first fix from
fusion absorption to nomination repair; `pending parents promoted = 161` is the repair firing.  `[shell]`
reads `max_dist = 0` while every delegate sits exactly on its chief (frames 1-2 of the cubic census), which
is what shows that membership has no spatial extent in the co-located regime, and `max_dist = 10-12` from
frame 3 on, which is what lets the brake act there.

## Reproduce (four builds, one ablation line)

    experiments\build_island_census_parent.bat          rem fsm only            -> 1 K, 241 unresolved
    experiments\build_island_census_parent_absorb.bat   rem fsm + fusion absorb -> identical to fsm only
    experiments\build_island_census_parent_repair.bat   rem completed rule set  -> 162 K, 0 unresolved
    experiments\build_island_census_parent_shell.bat    rem + shell brake (iii) -> identical to repair (max_dist=0)

    build\island_census_parent\island_census_parent.exe         2 16384 build/island_census_parent/run2 9
    build\island_census_parent\island_census_parent_absorb.exe  2 16384 build/island_census_parent/abs2 9
    build\island_census_parent\island_census_parent_repair.exe  2 16384 build/island_census_parent/rep2 9
    build\island_census_parent\island_census_parent_shell.exe   2 16384 build/island_census_parent/shell 9
    python experiments\analyze_island_spectrum.py build\island_census_parent\rep2

And the small-`W` rate balance, where the brake is NOT inert (`rb_shell.exe` is the third arm):

    experiments\build_island_rate_probe.bat                        rem builds rb_ref, rb_cand, rb_shell
    build\rate_probe\rb_ref.exe   placed 40 build/rate_probe/placed_ref.csv
    build\rate_probe\rb_cand.exe  placed 40 build/rate_probe/placed_cand.csv
    build\rate_probe\rb_shell.exe placed 40 build/rate_probe/placed_shell.csv
    build\rate_probe\rb_shell.exe driven 60 build/rate_probe/driven_shell.csv
    python quantization\quantize_stdlib.py build\rate_probe\driven_ref.csv build\rate_probe\driven_shell.csv --by-run


One light frame at `L=9` costs ~50 s, so a 2-frame run is about 2.5 min; the `L=15` test registered
above costs ~31 min per frame and should run in the background.
