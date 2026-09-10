# Photon/graviton-mediated interaction between composite particles through the orphan flux — design note v2

Status: **design** (no code yet).  Branch: `polarization-broadcast-fix`.
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

1. **Orphan flux**: after each reissue the particle sheds a concentric
   orphan shell (`a == W_USED`) — its EM field.
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
   (orphan; assumption flagged: `a = W` rather than `a = w`).  This
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

Still open (implementation detail, not conceptual):
(a) **orphan creation/retention rule** — today `a == W_USED` is written only
by the diffuse orphan-propagation slots (which require an existing orphan
seed) and by the free-pair release in `applyMomentum` (which orphans the two
released S singletons).  The Platonic seed writes `a = island index` for
every cell and **no K/D is ever orphaned**.  For the physical picture (a
field of concentric orphans around a K/D island) an explicit
creation/retention rule is required;
(b) where the island charge is read at the gate (the charge word shared by
the island's members);
(c) measuring the 1/r vs 1/r^2 exponent.

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
- an orphan creation/retention rule so K/D islands carry a concentric ghost
  region (see section 4a);
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

