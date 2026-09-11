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

## WP4.3 result: the broadcast is not dormant; the reorder works in a window (11 Sep 2026)

Two corrections to the WP4.1 "dormant broadcast" conclusion.

**1. The `pbsb_two` dormancy was a short-edge artifact.**  `pbsb_two` uses a
21x5x5 tube (RMAX = 2) and `phase_step` calls
`reconstructPair(bstamp, RMAX-2, ...)`; RMAX-2 = 0 forces pol = (0,0) for every
cell.  On a wider tube the reconstruction is live
(`experiments/pbsb_two_wide.cpp`, `build_pbsb_two_wide.bat`):

| tube | RMAX | pol_u range | pB | sB |
|---|---|---|---|---|
| 21x5x5 | 2 | [0,0] | 0 | 0 |
| 21x7x7 | 3 | [-1,0] | 0 | 0 |
| 21x9x9 | 4 | [-4,0] | 0 | 0 |
| 21x11x11 | 5 | [-3,0] | 0 | 970 |
| 21x13x13 | 6 | [0,0] | 0 | 1790 |

So the broadcast DOES light the polarization -- in `pol_v` (sB) here; `pB`
needs `pol_u > 0`, which this geometry pins away.

**2. With live sB and an open sieve, `EM_FIRST_FSM` changes the outcome** of two
equal-charge clouds (21x13x13, W = 2, axis-seeded, 16 frames):

| build | sieve | final | centres | enc_repel |
|---|---|---|---|---|
| ordinary | any | K=1 D=1 | **1** (merged) | 0..9 |
| `EM_FIRST_FSM` | default (16384) | K=1 D=1 | 1 | 0 (gate shut) |
| `EM_FIRST_FSM` | 16 | K=1 D=1 | **2** | 90 |
| `EM_FIRST_FSM` | 32 | K=1 D=1 | **2** | 90 |
| `EM_FIRST_FSM` | 64 | K=1 D=1 | **2** | 74 |
| `EM_FIRST_FSM` | 256 | K=1 D=1 | 1 | 58 |

In the window s2b <= ~64 the two centres stay apart with `enc_repel > 0`
instead of collapsing onto one; at the reference sieve the gate never opens and
they merge.  The roles still resolve to K + delegate (a stretched pair, not two
clean islands).

**Reading.**  The encounter reorder is correct (WP4.2) **and** its prerequisite
can be supplied by the ordinary dynamics on a sufficiently wide lattice -- the
polarization broadcast is *sign-pinned*, not dormant.  The repulsion is
parameter-windowed (sieve modulus), like every electroweak effect here
(`RESULTS.md` s2B sweep).  This is the strongest Route-B lead so far; making it
robust needs the broadcast to supply sB at a controllable phase and rate, which
stays open.

## WP4.4 result: the polarization signs are geometry-pinned, not axis-steerable (11 Sep 2026)

Probed with `alpha_probe <EL> 4 <fr> 16384 256 x x 1 pol <axis>` (cubic,
SEP=4, sieve 16384).  `<|pol_v|>/<|pol_u|>`, the pol_v sign census and the
"no pB cells" message give:

| EL | R = RMAX-2 | pol_u | pol_v | pB | sB |
|---|---|---|---|---|---|
| 7 | 1 | < 0 | 0 | no | no |
| 9 | 2 | < 0 | 0 | no | no |
| 11 | 3 | < 0 | > 0 | no | 19400 cells |
| 13 | 4 | = 0 | > 0 | no | 53700 |
| 15 | 5 | > 0 (ratio 4) | > 0 | **yes** (alpha_D = 1) | 54600 |
| 17 | 6 | > 0 (ratio 2) | > 0 | **yes** | 84120 |
| 19 | 7 | > 0 (ratio 2) | > 0 | **yes** | 118760 |
| 21 | 8 | > 0 (ratio 1.5) | > 0 | **yes** | 82780 |
| 23 | 9 | > 0 (ratio 1.2) | > 0 | **yes** | 109940 |

**R threshold.**  Reading down the table: `pB` (electric) lights only for
**R >= 5**; `sB` (magnetic) lights for **R >= 3**; for **R <= 2** neither lights
(the reconstruction is dead, `pol_u <= 0`, `pol_v = 0`).  The |pol_v|/|pol_u|
ratio falls toward 1 as R grows (4, 2, 2, 1.5, 1.2 for R = 5..9).

The seeded axis does **not** change the result: `pol m`, `pol z` and `pol za`
give identical censuses at EL=13 (positive = 8946, sB = 53700) and EL=7 (all
zero).

**Reading.**  `reconstructPair(bstamp, R)` maps the broadcast arrival tick to a
point on `pol_u^2 + pol_v^2 = R^4`; the sign of `pol_u` (hence `pB`) depends on
the arrival phase `j = (tick % 2R^2) / R`, and the broadcast's stamp pattern
pins that phase as a function of the lattice size `R` -- the "frozen phase
branch" of `RESULTS.md`, now measured.  The axis seed sets the walk direction,
not the phase reference, so it cannot steer `pB` vs `sB`.

**Consequence for Route B.**  Whether the electric (`pB`) or magnetic (`sB`) EM
branch is available is decided by the **geometry**, not the bootstrap: `sB` is
reachable for R >= 3, `pB` is hard (only EL=15 lights both among the tested
sizes), and neither lights for R <= 2.  A robust Route B therefore needs either
an EL=15-like geometry (both flags live) or a change to the reconstruction's
phase reference.

## WP4.5 result: with both EM flags live, two equal-charge clouds do not merge (11 Sep 2026)

At R = 5 (tube 21x15x15 -- the geometry where both pB and sB light: pB = 2730,
sB = 2730, pol_u in [0,5]), `EM_FIRST_FSM` with an open sieve keeps the two
equal-charge clouds as **two distinct S singletons** for the whole run:

| build | sieve | final | centres | enc_repel |
|---|---|---|---|---|
| ordinary | any | K=1 D=1 | 1 (merged) | 0 |
| `EM_FIRST_FSM` | 32 | **S=2 (no merge)** | **2** | 182 |
| `EM_FIRST_FSM` | 64 | **S=2** | **2** | 182 |
| `EM_FIRST_FSM` | 256 | K=1 D=1 | 1 | 66 |
| `EM_FIRST_FSM` | 16384 | K=1 D=1 | 1 | 0 (gate shut) |

Trace (s2b = 64): from frame 1 both clouds carry pB = sB = 2730; they stay S = 2
at two sites every frame, with `enc_repel` rising 18 -> 50 -> 98 -> 140 -> 182.
They never elect a chief and never collapse -- the cleanest repulsion found in
this campaign.

**Reading.**  When both EM flags are live (R = 5) the reorder is a clean
repulsion: two equal-charge clouds remain two.  The effect is windowed in the
sieve (s2b <= ~64), like every electroweak effect here.  This is the strongest
Route-B lead; `pB` (the electric flag) being live is what lets the electric
branch act alongside the magnetic one.

### Outcome map across R (EM_FIRST vs ordinary; sieve s2b = 64, 16 frames)

| tube | R | flags | ordinary | `EM_FIRST_FSM` |
|---|---|---|---|---|
| 21x9x9 | 2 | none | K=1 D=1, **1 site** | K=1 D=1, 1 site (no flag -> no effect) |
| 21x11x11 | 3 | sB | K=1 D=1, **1 site** | K=1 D=1, **2 sites** (rep 50) |
| 21x13x13 | 4 | sB | K=1 D=1, **1 site** | K=1 D=1, **2 sites** (rep 74) |
| 21x15x15 | 5 | pB+sB | K=1 D=1, **1 site** | **S=2, 2 sites** (rep 182) |
| 21x17x17 | 6 | pB+sB | K=1 D=1, **1 site** | K=1 D=1, **2 sites** (rep 82) |

So for **R >= 3** the reorder stops the spatial collapse of the two centres
(2 sites vs the ordinary 1); at **R = 5** it additionally stops the identity
merge (the clouds stay two separate S singletons).  For **R <= 2** no flag is
live and nothing changes, and the ordinary path always collapses to one centre.
The strongest configuration is R = 5 (both flags live), which is also the
smallest R at which `pB` lights.
