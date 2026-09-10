# Photon/graviton-mediated interaction between composite particles through the orphan flux — design note v2

Status: **design agreed** (A1, B1, C, D, E1, F1) — implementation next.
Branch: `polarization-broadcast-fix`.
Supersedes v1 (commit 11e24ec) which concluded the carrier "must be R1
graviton, not R2 photon".  The consolidated rules (Sep 2026) split the
picture: the **graviton (R1) is always attractive and universal** (residual
static/cross-sector coupling), while the **sectoral photon (R2) carries the
charge-sign channel** (like charges repel, opposite attract) — and it is the
photon (dress) channel that must mediate the equal-charge *repulsion* the
quantisation hunt needs.  The manuscript's orphan-flux narrative
(manuscript.tex:794-796, 1229, 1374, 1382) is kept, under this two-channel
reading; the tex may be revised to fit the consolidated rules.

## 1. Carrier structure (revised conclusion)

Two free-pair families play distinct physical roles:

- **R1 graviton** — all six bits complementary (`ca ^ cb == 0x3F`): one
  Orbis + one Umbra half, **charge-neutral by construction**, cross-sector,
  universal (manuscript.tex:491, 1229).  By the spin-parity analogy its
  exchange is **always attractive**: the residual "gravity"/static
  cross-sector coupling.  Orphans engage it (manuscript.tex:1382, 794).
- **R2 photon** — same-sector complementary (`w1` equal, q/w0/color
  complementary): **sector-specific**; its exchange is **charge-sign
  dependent** (vector exchange): equal charges repel, opposite charges
  attract.  This is the model's electromagnetism, and the *dressing* of a
  particle (its drive pairs are R2 photons, e.g. 0x00/0x1F in the inertia
  tests).

Equal-charge islands cannot synthesise an R2 photon from their own cores
(charges are immutable; no same-sector complement Q' is present), but each
island **already carries R2 photon dresses**; the repulsive exchange is an
exchange of those dresses between the two fields, before any core contact.
The graviton alone would make equal charges *attract* (merge) — consistent
with it being the always-attractive channel.

## 2. Actors: particle = island + dresses + field

- **Island**: 1K+nD cores of charge Q (the body; annihilation involves any
  representative — K or D — of two particles with opposite charge).
- **Dresses**: bound R2 photon pairs (drive pairs) carried by the island.
- **Field**: the concentric orphan region — cells with **affinity disabled
  (`a == W_USED`)**, ghost, non-collapsing (manuscript.tex:616, 1374).
  The definition is **cell-level and kind-agnostic**: it applies to the
  orphan region of ANY source kind, **S, D and K alike** (a particle's
  island cores are K/D, so its field is the orphan region around them).
  P pairs are the carriers/dresses, not recruiters.  The recruiter of free
  pairs is therefore this orphan region, not an "S-only" object.
- **Mediators**: free R2 photon pair (dressing/EM channel) and free R1
  graviton pair (universal attractive channel), present in the vacuum
  between the two particles.

## 3. Mechanism (consolidated rules; not yet implemented)

1. **Orphan field (thin shell)**: the thin concentric layer **just ahead of
   the active wavefront** (`r == f + 1`, `f = effective_t(t)`, inside the
   cavity), i.e. the part of the layer not yet **overlapped** by the front.
   Concentric with the source and **co-moving** — the translation moves the
   active front and its shell together, so at the destination address they
   remain concentric — and renewed as the front breathes.  One cell thick, so
   light-matter events stay as **rare** as they are in nature (a thin shell,
   not a volume).  (The older "fades through wrapping" phrasing is superseded
   by the spherical cavity.)
2. **Engagement (relay)**: the gate is the **electromagnetic interaction**.
   The half of the free pair that **provokes attraction** (charge-complementary
   in the electric sense to the charge of the orphan region) engages first.
   Per manuscript.tex:794, the engaged orphan and the pair half are **both
   reissued at the contact point** (reusing `reemitAtContact`); the relay then
   advances along the flux toward the other body — no invented gradual
   "guidance" step.
3. **Relay advance**: the reissue point moves along the flux toward the other
   body (today free P pairs only expand and are consumed at radius; the relay
   replaces that with directed reissue).
4. **Affinity handover**: on reaching the target the pair acquires its
   affinity and becomes one of its drive pairs (existing P-join path);
   internal P x K/D contacts then transport the target's constituents in
   the pair's m direction (inertia: 0.25/0.75 cells/frame; CoM conserved).
5. **Reciprocity**: the same happens from the target toward the source
   ("The same occurs in reverse", manuscript.tex:794).  Net effect:
   - R2 photon channel: **the sign is decided by the two quantized islands
     alone** (their charge words): equal charge -> repel, opposite -> attract;
   - R1 graviton channel: **always attractive**;
   - the dresses/drive pairs are only the **vehicle** (relay + momentum);
     their orientation does not enter the sign decision;
   - both symmetric in the mean (drive-pair transport conserves CoM).

**Annihilation boundary**: annihilation requires two representatives of the
two particles — **K or D indiscriminately**, different parents
(`islandChief` differs), **opposite charges, in any sector** — overlapping
the same site.  Both are then **demoted to S** and reissued at the contact
point, with affinity default `a = W` (orphan singleton).  Carriers never
annihilate charges at a distance.

## 4. Consolidated rules (v2) — agreed

1. Recruitment gate = **electromagnetic interaction** (orphaned sources, not
   arbitrary orphan cells — the EM branches act on sources).
2. The engaging half = **the one that provokes attraction** (electric
   complementarity), fixing the direction of the flux without an ad hoc rule.
3. **Graviton exchange is always attractive**; sectoral (R2) photons are
   required for the attraction/repulsion dichotomy.  Repulsion between
   equal-charge islands is the R2 dress-exchange channel.
4. Distance dependence is **automatic and geometric** (spherical flux, no
   dial); the observable-defined exponent must be fixed by measurement
   (potential 1/r; force 1/r^2).
5. **Annihilation** (revised, agreed): two representatives — K or D
   indiscriminately — with **different parents** (`islandChief` differs),
   **opposite charges, in any sector**, at the same site -> both **demoted
   to S**, reissued at the contact point, affinity default **`a = W`**
   (orphan; agreed Sep 2026: `a = W`, not `a = w`).  This
   supersedes the manuscript's same-sector-only annihilation and the
   inter-sector "singularization" split (tex :536/:551 to be reconciled);
   the tex affinity wording (:497 "revert to layer indices" vs "a=W marks an
   orphan") also needs one consistent statement.
6. **Engagement semantics = relay** (agreed Sep 2026): the engaged orphan
   region and the pair half are both **reissued at the contact point**
   (manuscript.tex:794), reusing `reemitAtContact` and the existing
   affinity handover — no gradual guidance rule.
7. **The sign is decided by the two quantized islands alone** (agreed Sep
   2026): equal charge -> repel, opposite -> attract, read from the islands'
   charge words.  The dresses/drive pairs are the vehicle (relay and
   momentum) only; the sign has **no** dependence on dress orientation
   (m/pB-sB) or on which relay arrives first.  Derived falsifiable control:
   swapping the dress orientations must leave the sign unchanged (section 6d).
8. **Orphan field (F1, agreed Sep 2026; P1 implemented)**: with the spherical
   cavity an orphan does not "fade through wrapping": it is the thin
   concentric layer **just ahead of the active wavefront** (`r == f + 1`,
   inside the cavity), the part of the layer not yet overlapped by the front.
   It persists until the front naturally overlaps it and is renewed as the
   front breathes.  **Translation moves everything** (active front + shell)
   so the shell stays concentric with the source at the destination address.
   One cell thick: light-matter interactions must stay **rare**, so a volume
   field is explicitly rejected.
   Implementation: **derived, not stored** — `isOrphanShell(c)` (local, from
   the per-cell `r` and `f` that `phase_step` already writes) with **no
   lattice write** and no change to the affinity/reissue chain.  Rationale: a
   stored `a = W` marker ahead of the front also shields the FSM's outward
   affinity fill and leaves the whole not-yet-swept volume orphaned
   (measured W ~ 49k of 50k cells); the derived predicate has **zero** side
   effects and the census stays bit-identical to the reference.
9. **Observable (E1, agreed Sep 2026)**: `d(t)` between the two island
   centres; the pre-contact window lasts while **no annihilation** occurs
   (rule 5); a new `recruit_events` counter records orphan-field x pair
   encounters separately from `enc_*`; swapping dress orientations is a null
   control (rule 7).

Still open (implementation detail, not conceptual):
(a) where the island charge is read at the gate (the charge word shared by
the island's members);
(b) measuring the 1/r vs 1/r^2 exponent.

## 5. Physics dashboard

| Interaction | Carrier | Sign | Reaches before cores meet | Requires |
|---|---|---|---|---|
| EM (charge sign) | R2 photon (dress) | by the two islands' charge: equal repel | yes (field/flux) | dress relay via EM gate; sign read from the islands |
| Static/gravity residual | R1 graviton (vacuum) | always attractive | yes | orphan flux + reciprocity |
| Annihilation | — (cores) | neutral | no — only D x D overlap | complementary delegates in one voxel |

## 6. First experiment (prepared, dressed two-body, production path)

Setup: two equal-charge bodies (0x08, tube/cube W >= 6) at SEP, **each
dressed with R2 photon drive pairs** (the dressing/EM channel under test);
a free R1 pair (0x00/0x3F) in the vacuum between them is the optional
graviton (attractive) control.  Bodies reissue periodically to shed orphan
shells.  Boot macros optional; EM_FIRST_FSM off for the pure dressing test.

Measure: do the bodies repel (d grows) through **dress exchange between the
two fields** before any core contact?  Per-frame: R2 dress-pair affinity
flips (a dress joins the other body), source kinds, d(t), CoM, enc_*
counters, pair consumption/annihilation events (annihilation only when two
representatives of different parents meet with opposite charge).

Controls:
(a) bodies without dresses -> merge baseline (d -> 0, identity contact);
(b) dresses but no orphan flux (no reissues);
(c) free R1 pair only (graviton) -> always-attractive behaviour expected
    (d decreases or merge), separating the two channels;
(d) dress orientations swapped: expected **null control** — the sign must not
    change (it is set by the two islands' charges alone).

Success: d grows monotonically while both bodies keep their K/D cores (no
identity merge, no D x D overlap), CoM conserved in the mean, reproducible
across seeds/axes.  If it fails, the missing piece is the directed
guidance/sign rule (rule 3/2 in section 4), not the carrier choice.

## 7. Code mapping (exists vs to add)

Exists: orphan cell semantics and orphan expansion (`a == W_USED`); free P
pairs (expand, consumed at max radius); P joins a contacted island and
acquires its affinity (interaction.cpp); drive-pair P x K/D transport with
CoM conservation (resolveInternalContacts / propeller); EM branches
(bootstrap + first-arrival wave + EM-first builds).

To add (macro-guarded candidate: ORPHAN_GUIDANCE_FSM):
- orphan region (cells with `a == W_USED`, any kind S/D/K) as the recruiter;
- orphan field = the thin shell just ahead of the active front
  (`isOrphanShell`: `r == f + 1`, inside the cavity), derived and co-moving
  with translation; **no** lattice write (section 4, item 8);
- EM-gated engagement that selects the attracting half of the free pair;
- directed re-emission/guidance of the engaged pair along the flux;
- R2 dress-exchange bookkeeping between two dressed bodies;
- affinity handover on approach (may reuse the existing P-join on contact);
- symmetric reverse bookkeeping (a pair may flip target);
- island-charge comparison at the gate for the sign (equal -> repel, opposite
  -> attract); no dress-orientation dependence;
- annihilation branch (K or D indiscriminately, different parents, opposite
  charges, any sector) with demotion to S and orphan affinity (`a = W`); no
  distant annihilation;

References: manuscript.tex:491, 616, 794-796, 1229, 1374, 1382;
experiments/PBSB_ISLANDS.md (decision rule: EXCLUSION stays the stand-in
until a dressing-mediated repulsion is demonstrated).  Note: the orphan
"photons" wording in earlier text should be read as sectoral R2 photons for
the charge-sign channel and R1 gravitons for the always-attractive channel
(see sections 1 and 5).

## 8. Status

- **P1 done** (commit `4ed2ef3`, revised here): the orphan shell is the thin
  derived band just ahead of the active front, `isOrphanShell()` in
  `src/include/model/simulation.h`, macro `ORPHAN_GUIDANCE_FSM`.
  - default build (no macro): reference run EL=7 SEP=4 200 frames is
    **bit-identical** (`active-passes=6468  s2B-passes=0  pairs-formed=0`).
  - macro build, canonical run EL=7 (W=147 layers), `alpha_probe_orphan.exe
    7 4 3 16384 120 canon`: the shell is a single radius per frame moving with
    the front — frame 1 `r=1` (3822 cells), frame 2 `r=2` (9702), frame 3
    `r=3` (23226) — i.e. one cell thick and concentric (26/66/158 cells per
    layer).
  - the matter/antimatter/orphan census is **unchanged** by the macro
    (`orphanM=0 orphanA=0`, as in the reference): the field is derived, so it
    cannot perturb the FSM.
- **P2/P3 RESULT (macro `ORPHAN_GUIDANCE_FSM`, EL=11 SEP=4, bodies 0x08 equal
  charge, impulse applied through the production helpers `moveOneStepAway` /
  `moveOneStep` so the centre of mass is conserved and the reemission ledger is
  used)**:

  | mode | recruit | signals | d(t) |
  |---|---|---|---|
  | bare (W-matched, no mediator link) | 0 | - | 4, 4, 2, 0 (merge) |
  | R2 photon (equal charge -> repel) | 354 | **repel = 22** | **4, 3, 4, 3, 4, 3 (NO merge)** |
  | R1 graviton (always attractive) | 482 | attract = 4 | 4, 0, 0 (immediate merge) |

  This is the E1 observable the quantization hunt needs: the R2 (orphan/EM)
  channel **holds two equal-charge islands apart** while the R1 channel always
  attracts and the bare control merges.  Controls (a) bare, (c) graviton-only
  and (d) swapped-halves (null) all pass.
- **Implementation note**: writing raw `reloc` into a source whose kind is not
  an island (a mediator / unaffiliated layer) drove the downstream
  identity/charges machinery out of bounds and segfaulted (bisected with the
  `ORPHAN_GATE_ONLY` build: gate-only runs clean).  Using `moveOneStepAway` /
  `moveOneStep` on the two ISLAND centres fixes it (no crash in any mode) - the
  same guarded helpers the electroweak branches use.
- **Impulse reformulated to the production pattern (frame edge)**: the gate now
  only RECORDS each engagement (island A, island B, sign) in `recruitPushes`;
  `resolveRecruitPush()` applies one antisymmetric face step per island pair
  per frame inside `commitSourceTick`, exactly like `resolveExclusionPush`
  (centre of mass conserved, no reloc writes in the middle of the phase
  sequence).  Coincident centres are skipped (no defined push direction), and
  the relay now fires only for a FREE mediator AT REST (a bound dress is
  already co-located with its body; a driving pair must not have its halves'
  clocks desynchronised).
  Verified after the change (EL=11 SEP=4, bodies 0x08, 30 frames):
  | mode | recruit | signals | min d |
  |---|---|---|---|
  | bare | 351 | - | 0.00 (merge) |
  | R2 photon (equal charge -> repel) | 202 | repel = 4 | **1.00 (never fully merges)** |
  | R1 graviton | 39 | attract = 2 | 0.00 (merge) |
  and the default build is unchanged (6468/0/0).
- **Annihilation branch IMPLEMENTED, trigger still unreachable** (rule 5 /
  decision D): the branch exists in `encounter()` under `ORPHAN_GUIDANCE_FSM`
  (two BODY representatives K or D, `islandChief` distinct and non-null,
  matter vs antimatter by the census's colour-popcount convention, same site ->
  both demoted to S, `parent = NO_PARENT`, `leader_w = NO_LEADER_W`, `a = W`,
  `pair_idx/pair_count` cleared, both reissued at the contact point).  New
  counters: `annihilations` (interaction.cpp/probe) and the ledger's
  `chargesMarkAnnihilation()` printed as `ann=` on the `[charges] retina` line.
  No reachable trigger found yet:
  - the deterministic scenario (probe mode `annih`: two island CHIEFS at the
    same site with opposite charges) crashes at INITIALISATION in BOTH the
    macro and the default builds - two co-located chiefs are simply not a
    supported model state (0 frames ran; the probe mode is kept as a
    reproducer).  Setting `a = 3*fam` did not change this.
  - in the real layouts tried (NEAR W=4, ch = +8 / -1, mag = 0/1, 25 frames)
    the two bodies either never reach the same site or are merged by the
    identity path, so `annihilations` stays 0 in every run (and the default
    build is unaffected).
  Design question to settle: the "same site" condition needs a REACHABLE
  definition, e.g. (a) annihilate at the contact of two islands'
  representatives (the moment the identity merge would fire) when the charges
  are opposite, or (b) restrict to D x D contacts (which in practice sit behind
  the `s2B` gate).
  **RESOLVED with trigger (a)** (agreed): "distinct islands" is read as
  different `islandChief` once both exist, otherwise different charge FAMILIES
  (`w/3`, the island grouping before the chief election).  The probe scenario
  `two` (W = 6, two CHIEFS planted at w=0 and w=3 - families 0 and 1 - with a
  probe-level driver closing the gap one light-step per frame) exercises it:
  - opposite charges (0x00 vs 0x3F): **`ann = 18`** annihilation events in the
    first census window, the two bodies demoted to S and reissued, d(t)
    oscillating (4,1,3,5,3) - the islands keep re-forming and re-annihilating;
  - equal charges (0x08 both): **`ann = 0`** with the two bodies merging
    normally - the exact design prediction, and a clean control.
  The equal-charge merge also confirms the branch does not over-fire.
  Remaining: the opposite-charge run SEGFAULTS a few frames after the burst
  (tick ~1021), i.e. in the post-annihilation state (two demoted S sources
  sharing a site); this is the next investigation item and may share a root
  cause with the `dressed` crash below.
  **ROOT CAUSE FOUND AND FIXED** (bisected with a new `ORPHAN_NO_ANNIH` build:
  with the branch disabled the same scenario runs all 25 frames clean, so the
  crash came from the annihilation state, not from the scenario):
  demoting a CHIEF (K) to S leaves its island's MEMBERS pointing at an anchor
  that is no longer a chief, and the identity machinery then walks a dangling
  reference and crashes a few frames later.  Demoting DELEGATES (D) is safe.
  The branch is therefore restricted to **D x D** (which is also what the
  manuscript states: annihilation happens on a D x D overlap).  New probe
  scenario `twod`: two islands, chiefs at w=0/w=3 (anchors, never annihilated)
  and delegates at w=1 (parent 0) and w=4 (parent 3), driven together by the
  probe driver.  Measured (EL=11, 25 frames):
  - opposite charges: **exit 0, ann = 1368** events (the delegates annihilate,
    re-form and annihilate again, the anchors surviving throughout);
  - equal charges:    exit 0, ann = 0 (clean control);
  - the anchor x anchor scenario `two` now runs clean with ann = 0 (chiefs are
    no longer demoted), and the default build is unchanged (6468/0/0).
  Open (design): K-member annihilation (any representative) needs
  island-dissolution semantics - demote or re-anchor the surviving members -
  before a chief can be annihilated safely.


- **OPEN BUG in the `dressed` layout - RESOLVED (root cause found)**: the crash
  was NOT in the physics.  `getCell()` neither wraps nor bounds-checks its
  coordinates (`((x*ELY + y)*ELZ + z)*W_USED + w` on size_t), and the gate's
  six-neighbour orphan-shell test used raw `cx +/- 1`: whenever the contact site
  sat on a lattice FACE, `x = -1` became a huge size_t and the read went out of
  the lattice -> segfault.  That is why the crash needed the macro, survived a
  read-only gate build (zero mutations - a pure OOB read), and was layout
  dependent (the NEAR geometry never touches a face; the driven `dressed`
  geometry does).  Fix: periodic wrap applied in the gate before `getCell`
  (`v %= m; v < 0 ? v + m : v`) plus a defensive `x[3] < W_USED` guard.
  With the fix the section-6 primary setup runs clean (EL=11, 20 frames):
  - equal charges (0x08 both):   exit 0, recruit = 3746, d(t) = 4, 4, 5, 5, 5, 3
    (the two dressed bodies are HELD APART - the channel works with bound
    dresses, no merge);
  - opposite charges (0x00/0x3F): exit 0, recruit = 9656, d(t) = 4, 4, 0, 0, 0
    (they merge - attraction, the design's sign).
  The bisection builds used (all under ORPHAN_GUIDANCE_FSM unless noted):
  build_probe_gateonly.bat (ORPHAN_GATE_ONLY), build_probe_noapply.bat
  (ORPHAN_NO_PUSH_APPLY), build_probe_noannih.bat (ORPHAN_NO_ANNIH),
  build_probe_norelay.bat (ORPHAN_NO_RELAY), build_probe_printonly.bat
  (ORPHAN_PRINT_ONLY - default model, probe prints only).




- **What still merges the bodies**: the two planted bodies start unaffiliated
  (a == W_USED) and the identity machinery elects a chief between them
  (`[src]` shows w=0 becoming K and w=1 becoming D with the same leader), so
  the island cohesion pulls them together - this is the pre-existing `none`
  background, not a mediator effect.  The quantization payoff therefore needs
  the two bodies in DISTINCT islands from the start (separate charge families,
  e.g. w=0 and w=3 with W >= 6), where the mediated channel is the only
  coupling left.

- **Next**: (1) plant the two bodies in DISTINCT islands (separate families,
  w=0 and w=3 with W >= 6) so the identity merge is out of the way and the
  mediated channel is the only coupling; (2) control (b) of section 6 (dresses
  but no orphan flux / no reissues); (3) the annihilation branch (different
  parents, opposite charges, same site -> both demoted to S) with its own
  counter; (4) then re-run the quantization hunt with the separated copies.


Build scripts: `experiments/build_probe.bat` (default reference),
`experiments/build_probe_boot.bat` (bootstrap + broadcast wave),
`experiments/build_probe_em.bat` (`EM_FIRST_FSM`),
`experiments/build_probe_orphan.bat` (`ORPHAN_GUIDANCE_FSM`).

## 9. Geometric sweep

Impulse granularity settled: the mediated push is **one light-step per TICK**,
applied inline in `encounter()` through `moveOneStepAway()` / `moveOneStep()`
(production helpers, CoM conserved), deduplicated per tick and skipped for
coincident or adjacent centres.  The earlier frame-edge variant (one step per
FRAME) did not scale with the frame's tick count and gave geometry-dependent
outcomes (EL=13 merged where EL=9/11 held); scaling it by the frame's tick
count overshot badly and was discarded.

Bodies 0x08 (equal charge), mediator = R2 photon pair, initial d = 4, 20 frames,
mag = 0 (`alpha_probe_orphan EL 4 20 16384 120 no no 0 no m 8 photon`):

| EL | photon min d | photon signals | graviton min d | bare min d |
|---|---|---|---|---|
| 9  | 1.00 | repel    | 0.00 | 0.00 |
| 11 | 3.00 | repel 38 | 0.00 | 0.00 |
| 13 | 4.00 | repel 30 | 0.00 | 0.00 |
| 15 | 4.00 | repel 18 | 0.00 | 0.00 |

- **The R2 channel holds or increases the separation at every scale**; the R1
  graviton and the W-matched bare control always merge.
- **Control (b) - new `ORPHAN_NO_SHELL` build** (the orphan shell is switched
  off): EL=11 SEP=4 photon -> `recruit = 0`, `repel = 0`, `attract = 0`,
  min d = 0.00: the channel DISAPPEARS and the bodies merge like the bare
  control.  The effect therefore requires the orphan flux.
- **Regression suite (per-tick build)**: default `6468/0/0`; NEAR (W=4) photon
  min d = 3.00 / graviton 0.00 / bare 0.00; `dressed` (W=6) equal charges
  min d = 1.00, opposite 0.00; `twod` annihilation `ann = 1368` vs 0 (equal),
  no crash.
- **Distance law (no clean power law)**: the engagement rate per frame, pooled
  over the photon runs and binned by separation, is ~36 events/frame for
  d <= 4 and 18 at d = 5 - essentially flat, not 1/d^2.  Two reasons: the
  impulse magnitude is a saturated one light-step per tick (force
  distance-independent by construction), and the number of coincidence cells of
  two expanding shells depends on the wavefront RADIUS as much as on d (the
  intersection circle has radius sqrt(r^2 - (d/2)^2)).  Fixing the exponent
  needs a dedicated probe that parks the bodies and logs the shell radius per
  frame (open).



### Parked probe (distance law) and the frozen mediator

Probe mode `park`: the two bodies are placed in DISTINCT families (the `far`
layout, W = 9: bodies w=0 and w=3, free mediator pair w=6/7) and are **re-pinned
to their sites at the start of every frame** by a bijective layer shift (the same
transport `applyMomentum` performs), so the separation is exactly the pinned one
while the shells keep cycling.  It logs `d`, both shell radii and the per-frame
engagement count (`[park]`, `[recruit]`).

Measured (EL=11, 12 frames, equal-charge bodies, R2 mediator, d pinned):

| pinned d | engagements (12 frames) | per frame |
|---|---|---|
| 2 | 484  | 40.3 |
| 4 | 354  | 29.5 |
| 5 | 272  | 22.7 |

- Genuine **monotone distance dependence**, but **no clean power law**: the local
  log-log slope is about -0.45 between d=2 and 4 and -1.18 between 4 and 5, i.e.
  between 1/d^(1/2) and 1/d.
- The engagements do NOT follow the two-expanding-shells intersection geometry
  (events occur at radii where the two ISLAND shells cannot meet), because the
  **mediator is not a propagating wave**:

**Root cause of the absent distance law - the mediator's clock is pinned.**
Two independent mechanisms freeze the free pair's clock:
1. `commitSourceTick()` advances `t` only for `body(s) || isBoundPropeller(s)`; a
   free pair is neither, so it would stay at its initial `t` for ever.
2. The recruit RELAY itself: `reemitAtContact()` sets `t = 0` and fires on every
   engagement, so it re-pins the pair to t=0 continuously.
Evidence: with the new `ORPHAN_MEDIATOR_PROPAGATES` build (a free pair advances
its clock like a body) the parked counts are EXACTLY the same (484/354/272) -
the relay still pins the clock.  With `ORPHAN_MEDIATOR_PROPAGATES +
ORPHAN_NO_RELAY` the count at d=2 jumps from 484 to **10576** (~20x): the photon
then really propagates and engages an order of magnitude more often.

**Consequence**: any distance-law measurement needs the relay to stop resetting
the mediator's clock (reissue without `t=0`, or relay only the orphan side).
Until then the "photon in the vacuum" is a stationary marker and the observed
rate is set by the ISLAND fronts sweeping it - hence the irregular exponent.
The default build is unaffected (`ORPHAN_*` macros only).

### Relay semantics A/B/C (the tension found)

Three relay variants were measured (all macro-guarded; the parked probe gives the
distance law, the NEAR configuration gives the repulsion):

| variant | relay action | NEAR photon min d | parked d=2/4/5 | local log-log slope |
|---|---|---|---|---|
| A (default, validated) | `reemitAtContact` (move + t=0) | **3.00 (repels)** | 484 / 354 / 272 | -0.45 / -1.18 |
| B | `reseatAtContact` (move, keep t) | 0.00 (merges) | 8438 / 3934 / 2505 | -1.10 / -2.05 |
| C | no-op (nothing) | 0.00 (merges) | 10576 / 6394 / 5544 | -0.36 / -0.76 |

- Variants B and C **give a monotone distance law** (approaching 1/d^2 at larger
d for B) but **lose the repulsion**: once the mediator propagates,
`applyMomentum` CONSUMES the pair at `t == RMAX` (`pair_count--`, and at zero the
two halves are released as singletons), so the channel dies after one cycle.
- Variant A (the validated base) keeps the mediator's clock pinned, which keeps
the channel alive and the repulsion working, but kills any clean distance law.
- **To get both**, the mediator must not be consumed: give the free pair a stack
  (`pair_count > 1`) or re-emit it at the turnaround (the manuscript's "pairs
  are consumed at maximum radius" mechanism, but with re-emission).  That is the
  next implementation step; until then variant A stays the default and B/C are
  opt-in (`ORPHAN_RELAY_KEEPT`, `ORPHAN_RELAY_NOOP`, `ORPHAN_MEDIATOR_PROPAGATES`).
- Regression suite re-verified on variant A: default 6468/0/0; NEAR photon 3.00 /
graviton 0.00 / bare 0.00; dressed equal 1.00 / opposite 0.00; control (b)
`ORPHAN_NO_SHELL` recruit = 0 and min d = 0.00; `twod` annihilation 1368 vs 0.
