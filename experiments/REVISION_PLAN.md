# Revision plan for doc/manuscript.tex (editor response)

Working document.  Goal: re-frame the paper as a *falsifiable computational
study of a fully specified discrete toy universe*, with claims scoped to what
is measured, taxonomy and quantum material moved to clearly labelled
speculation, and the structural complaints (tiny subsections, definition
tables) fixed.  Keep the .tex frozen until the evidence gaps below are
filled; this file is the blueprint.

## 1. Proposed outline (replacing the current section order)

1. **Introduction** - rewritten scope sentence: "We present a fully specified
   discrete dynamical toy universe together with a pre-registered
   falsification campaign and controlled candidate mechanisms.  We do not
   claim this reproduces physics until a quantitative match is demonstrated."
2. **Model definition (factual)** - cells/lattices/W, the clock postulate
   (front advance = 1 cell/tick is the *definition* of the information clock,
   not an emergent prediction), charges, light frame, encounter.  One compact
   notation table.  Merge the ~30 one-paragraph subsubsections (current
   `Particles`, `Light frame` fragments) into full paragraphs.
3. **Falsification campaign (moved up from Results)** - pre-registered
   alpha_A..F (all FAIL, with the acceptance criterion restated), two-bubble
   scattering/gravity negatives (no distance law; sieve-gated collapse
   telegraph), and the **island-aggregation negative in this codebase**:
   superposed Platonic seed -> absorbing 235 K / 8 D at one centre
   (`experiments/ISLAND_CENSUS.md`), no 1K+nD.
4. **Positive controlled results** - prepared-island inertia (velocities
   0 / 0.25 / 0.75 etc.); each with full parameters and an acceptance rule.
5. **Candidate mechanisms (clearly experimental)** - colour/matched +
   decouple regimes; sparse micro-islands (geometry-guaranteed); the
   Pauli-like identity hard core + push (`EXCLUSION_FSM`): robust 81 x 3
   quantisation in duo2/4, cube, spread81 and the production port, each with
   its ablation control (without the rule -> merge).  A preamble states these
   are not part of the model and change `canElectChief` semantics.
6. **Limitations** - answers the editor directly: postulate-vs-emergent
   table; current non-localities (host-side arbitration, whole-layer
   `applyMomentum`, per-layer reissue flags); no-signalling claim restricted
   or removed; scaling curves with error bars and a pre-registered
   convergence criterion (the current alpha_A table is L-dependent and the
   text already says so; the paper should stop presenting it as neutral).
7. **Appendices** - A notation; B charge-combination census (combinatorial,
   already flagged); C taxonomy zoo (moved `Particles` + charge-name labels)
   with an explicit "naming analogy, not demonstrated particles"; D quantum
   bridge & no-signalling (flagged: not reconciled with the automaton's
   irreversibility; non-local updates); E conjectures (gravity, spin, atoms,
   EM repulsion between dressed islands, partons...).

## 2. Where each editor point is answered

| Editor point | Where in the revision | Evidence already in repo |
|---|---|---|
| Claims exceed results | New scope sentence + sections 3-5 separation | RESULTS.md negatives; ISLAND_CENSUS.md |
| Speed/polarisation prescribed | Postulate-vs-emergent table (sect. 2 and 6) | simulation.cpp clock/update; phase_step reconstruction |
| Labels unsupported | Taxonomy to appendix C, explicitly analogical | manuscript already admits mass-ratio 35% deviation |
| No-signalling vs non-locality | Limitations + appendix D | INERTIA.md / colour docs admit non-local arbitration |
| QM vs irreversibility | Appendix D flagged speculative | sec:bridge is conjecture only |
| Scaling deteriorates | Section 3 with error bars + criterion | RESULTS.md alpha_A table |
| Structure (tiny subsections/tables) | Merge fragments; single notation table | section map at bottom of this file |

## 3. Missing quantitative evidence to generate first

Before touching the .tex:

1. **Ablation table for the 81 x 3 quantisation** (all already run, needs a
   single table): duo2/4 with and without EXCLUSION; cube superposed with and
   without; spread81 with and without; production port.  Sources:
   `build/lightmatter_decouple/{xd2,xd4,exc_cube,exc_cube2,sp81x,...}`.
2. **Spread81 EXCLUSION+push, longer stability** (12+ frames) and per-island
   pairwise-distance distribution (command: `lightmatter_decouple_exc.exe 1 12
   sp81xL spread81`, ~1.5-2 h).
3. **Production 1K+2D**: decide the intra-family promotion fix and re-run the
   production port (currently 2 K + 1 D per family).
4. **Postulate-vs-emergent control**: ablation run showing the 1-cell/tick
   front follows from the rule set (defensive; cheap).
5. **Scaling with error bars**: repeat alpha_A at L=7,9,11,13 several seeds to
   give means +/- std (RESULTS.md has point values).

## 4. Current section map (doc/manuscript.tex, for the re-organisation)

- 64 Introduction; 102 Related work; 123 Universal fabric (136 cells, 219
  lattices, 224 purity, 241 charges); 251 Initial state; 290 Phase step
  (292 metric, 342 wave cloud, 360 polarization pair); 419 Light frame
  (447 defs, 469 bubble/pair/..., 500 encounter, 569 diffusion, 620
  translation, 648 annihilation, 652 updating, 695 algorithm);
- 805 Interactions (807 charge-driven, 823 self-interference); 937 Particles
  (940 fermions, 944 bosons); 951 Dynamic charge quantization (953 objects,
  982 attractor, 994 W origin); 1006 Results (1007 inertia, 1012 speed/size,
  1028 census, 1031 proof of concept, 1091 sieve sweep, 1129 scaling);
- 1155 Quantum formalism bridge; 1175 Conjectures (gravity, spin, zoo, decay,
  atoms, observers/no-signalling 1370); charge-combination appendix.

Main moves: `Particles`->appendix C; `Cellular automata and quantum
formalism` + no-signalling->appendix D; `Conjectures` prose halved; the new
falsification/candidate sections take material from `Results` +
`experiments/{RESULTS.md,ISLAND_CENSUS.md,LIGHTMATTER_DECOUPLE.md,PBSB_ISLANDS.md}`.

