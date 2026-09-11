# pB/sB dressed-island experiment (design)

Status: design + implementation note.  Complementary to LIGHTMATTER_DECOUPLE.md.

## Two-level separation (why the EXCLUSION_FSM conflates two mechanisms)

The model distinguishes two different reasons why two equal-charge systems do
not collapse into one:

1. **Intra-family identical copies** (the L/3 copies of one W-island, `w/3`):
   identical constituents distinguished only by their pB/sB pattern.  Pauli-like
   exclusion / indistinguishability is the natural principle here; it is the
   source of the "9L x L/3" bookkeeping (identical particles are so similar).
2. **Inter-family equal-charge islands** (e.g. two electrons): they must not
   merge, but the *physical* reason is the **electric repulsion between whole
   dressed islands, mediated by photons (pB/sB channel)** — not an identity
   exclusion.  Inside a dressed island there is no repulsion concept.

`EXCLUSION_FSM` implemented a *stand-in* for level 2 by blocking cross-family
identity merge (and pushing).  It is numerically effective but physically a
placeholder.  This design replaces that placeholder with the model's actual
channel: pB/sB (electric/magnetic) flags on the wavefronts of dressed islands.

## Target experiment

- Production path (ordinary simulation()), tube geometry, W small.
- Two **dressed islands** prepared (each: 1 K + 2 D of equal charge + photon
  (P) dressing with the island's affinity), placed so their shells contact
  (distance ~ 2*RMAX), the duo2/duo4 analogue.
- pB/sB is activated not by the (deferred) self-bootstrapping election but by
  an **initial polarization axis seeded per island** (`polarization::seedAxis`,
  topological initial datum, allowed by the manuscript's purity constraints).
- Observable: do the two islands stay two (repel via the electric channel) or
  merge into one (as they do in duo without any protection)?  Counters:
  enc_collapse/enc_adiah/enc_repel, plus centres/chief census per frame.

## Mechanistic finding (from interaction.cpp, Sep 2026)

Reading the production `encounter()` shows that for EQUAL full charge words the
identity path preempts the EM channel:

1. `chiefContact()` runs first and merges equal-charge S/S, K/K (clash) or D/D
   (promotion) across families;
2. K x K / D x D equal charge then `return false` (lines ~431-434) before the
   pB/sB electric/magnetic branches are ever reached;
3. the EM branches (`electricContact/magneticContact`, collapse vs adiabatic)
   therefore only mediate contacts of DIFFERENT full charge words (e.g.
   complementary clouds) — the K x K "electric collapse repels" and S x S
   "same-Q repel" paths are unreachable for two equal-word islands.

Consequence: even with pB/sB seeded, two prepared equal-charge islands will
merge through the identity clash before any EM repulsion can act.  Making the
"complete-system" story (two dressed electrons repel) work requires EM to be
decided before the identity merge for dressed islands, or the repulsion to be
carried by the dressing (photon exchange between clouds that never let the
body islands contact).  This is the concrete gap the pB/sB experiment must
address; without re-ordering, seeding axes alone cannot demonstrate repulsion.

## Empirical confirmation (pbsb_two, tube 21x5x5, W=2)

Two equal-charge S clouds (0x08) at torus distance 2, each with a seeded
polarisation axis (`polarization::seedAxis`, +x/+x and +x/-x variants),
ordinary production path, 16 frames (`build/pbsb_two/`):

| variant | frames 0-1 | frame 2..16 | pB/sB | collapse/adiah/repel |
|---|---|---|---|---|
| same (+x,+x) | 2 S, 2 sites | **K=1, D=1, 1 site (merged)** | 0 everywhere | 0 |
| opposite (+x,-x) | 2 S, 2 sites | **K=1, D=1, 1 site (merged)** | 0 everywhere | 0 |

Both clouds merge in the election frame and the EM counters never fire.  In
this minimal geometry the seeded broadcast did not light pB/sB on the contact
voxels before the identity merge removed the two centres, and even if it had,
the equal-charge early returns (chiefContact / K x K, D x D) precede the EM
branches.  This confirms the code-reading conclusion: with the current
encounter order, EM cannot separate two equal-charge islands; making the
dressed-island repulsion work requires re-ordering EM ahead of the identity
merge (or a dressing-mediated repulsion acting before body shells meet).

## Decision rule (revised)

- If EM contact keeps two islands -> the inter-family EXCLUSION gate can be
  removed (identity rule reduced to intra-family/Pauli scope).
- If EM contact does not repel (current collapse telegraph has no directed
  island-island repulsion) -> the missing piece is precisely a photon-mediated
  island-island repulsion term to design next; the EXCLUSION gate remains a
  documented stand-in until then.

## WP4.1 empirical test: EM_FIRST_FSM reordering (11 Sep 2026)

The reorder is **already implemented** in the code as the `EM_FIRST_FSM` macro
(`src/model/interaction.cpp:884-984`): for equal-charge contacts that carry
live pB/sB and pass the s2B gate, it decides the electroweak channel BEFORE
`chiefContact()`.  Task WP4.1 was to test whether it changes the
two-equal-charge-cloud outcome.

A/B test (harness `pbsb_two`, tube 21x5x5, W=2, seeded axes, 16 frames),
default vs `/D EM_FIRST_FSM` (script `experiments/build_pbsb_two_em.bat`):

| build | frames 0-1 | frame 2..16 | pB/sB | collapse/adiah/repel |
|---|---|---|---|---|
| default (`build/pbsb_two`) | 2 S, 2 sites, dx=2 | K=1, D=1, 1 site (**merged**) | 0 everywhere | 0 |
| `/D EM_FIRST_FSM` (`build/pbsb_two_em`) | 2 S, 2 sites, dx=2 | K=1, D=1, 1 site (**merged**) | 0 everywhere | 0 |

The two traces are identical.  **The reorder is inert**, and the reason is a
missing prerequisite, not the encounter order: the seeded axis never lights
pB/sB on the shells (`DEBUG phase ... pol=(0,0) bstamp=2`; pB/sB = 0 in every
frame), so the guard `curr.s2B && (curr.pB || partner.pB || curr.sB ||
partner.sB)` is never true and control falls through to `chiefContact()`
exactly as in the default order.

Consequence: making two equal-charge dressed islands repel requires the
**polarization broadcast** to reconstruct pol_u/pol_v (hence pB/sB) on the
overlapping shells.  The encounter reorder is necessary but not sufficient and,
on its own, changes nothing.  This matches the broadcast-dormant finding of
`RESULTS.md` (alpha_D unobservable; `seedAxis` reconstructs only a frozen
phase branch).

Reference invariant preserved: the default `alpha_probe 7 4 200 16384 256` run
still reports active-passes = 6468, s2B = 0, pairs = 0, alpha_A = 0.003756878
(1/alpha_A = 266.18), and the model fingerprint still matches `model-ref-v1`.

### Next candidate (WP4.2, untested): inject the prerequisite

To isolate whether the reorder is *mechanically* correct, force `s2B` and `pB`
on both clouds' shells each tick (an in-tick hook placed after phase_step and
before encounter).  If the forced run repels (sites stays 2, `enc_repel` > 0)
the reorder is validated and only the broadcast is missing; if it still merges,
the reorder itself is flawed.  Left as the next increment of WP4.

## WP4.2 result: the reorder is mechanically correct (11 Sep 2026)

WP4.2 isolates the reorder from the (dormant) broadcast by injecting its
prerequisite.  A new macro `EM_FORCE_PREREQ` (`src/model/interaction.cpp`,
immediately before the `EM_FIRST_FSM` block) sets
`curr.pB = curr.s2B = partner.pB = true` at each contact, so the guard fires
wherever two shells overlap -- exactly as if the broadcast had lit the flags
and the sieve were open.

Build/run: `experiments/build_pbsb_two_forced.bat`
(`/D EM_FIRST_FSM /D EM_FORCE_PREREQ`), then
`build\pbsb_two_forced\pbsb_two_forced.exe`.

Result (tube 21x5x5, W=2, two equal-charge S clouds 0x08, seeded axes):

| frame | K | D | S | sites | dx | collapse | adiah | repel |
|---|---|---|---|---|---|---|---|---|
| 0-1 | 0 | 0 | 2 | 2 | 2 | 0 | 0 | 0 |
| 2 | 0 | 0 | 2 | 2 | -1 | 18 | 0 | 18 |
| 3..16 | 0 | 0 | 2 | 2 | **9** | 50 | 0 | 50 |

With the prerequisite present the two equal-charge clouds **repel and stay
two** (`sites = 2`, `dx` grows 2 -> 9, `enc_repel = 50`) instead of merging into
1 K + 1 D.  The role population is unchanged (both stay S).  The `EM_FIRST_FSM`
reorder is therefore **mechanically correct and sufficient**: it needs only a
live pB and an open s2B gate on the overlapping shells.

### Consequence for the campaign

The blocker that keeps two equal-charge dressed islands from merging is the
**polarization broadcast** (supplying pB/sB on the shells), NOT the encounter
ordering.  Adopt `EM_FIRST_FSM` once the broadcast works; on its own it changes
nothing (WP4.1).

### Note on the frozen reference

`EM_FORCE_PREREQ` is a new macro-guarded experimental block in a model source,
so the model source fingerprint advanced.  The reference build is unchanged
behaviourally -- the reference macros are OFF, and `alpha_probe 7 4 200 16384
256` still reports active-passes = 6468, s2B = 0, pairs = 0, alpha_A =
0.003756878 -- but the source hash moved from `5b0944...` (`model-ref-v1`) to
`29ef0e...` (see `doc/REFERENCE_CONFIG.md`, "Post-freeze additions").
