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
