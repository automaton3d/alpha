# Candidate version — manuscript change summary (WP6)

**Manuscript:** `doc/manuscript.tex` / `doc/manuscript.pdf` (revision of 14 Sep 2026).
**Build:** `doc\latexpdf.bat` (`pdflatex` + `biber` + `pdflatex` + `pdflatex`);
compiles with **no undefined references and no missing citations** (eight
overfull hboxes remain).
**Model reference:** `MODEL_VERSION` `model-ref-v1` (`doc/REFERENCE_CONFIG.md`);
fingerprint verified by `experiments/model_fingerprint.ps1`.

This is the change summary for the editor; it lists what the revision changed
relative to the pre-revision manuscript.  Point-by-point responses to the
simulated referees are in `experiments/RESPONSE_TO_REFEREES.md`.

## Structural

- Added a dedicated section **"The falsification campaign: six candidates for the
  fine-structure constant"** (`sec:campaign`, `tab:campaign`) immediately before
  Results.
- Moved the particle taxonomy (`Particles`), the quantum-formalism bridge, and the
  Speculative particle spectrum **to the appendix** (as sections).
- Merged one-line Conjectures subsections (Color + Weak quantization; Spin +
  radial alignment) and demoted two short model subsections (The host lattices;
  What the seed does not contain) to paragraphs.

## Claims and evidence

- Abstract and Introduction now state the **pre-registered falsification
  campaign** explicitly.
- Added the **claim-evidence map** (`tab:claim-evidence`) and the
  **postulate-vs-emergent** table (`tab:postulate-emergent`).
- Toned down unsupported claims (Novelty list, Future inclusions, Final thoughts);
  the taxonomy and the QM bridge carry an explicit Status (C).
- Added an explicit **determinism / no-seeds** note to the campaign section.

## Scope reduction (13/14 Sep 2026)

The revision that followed the aggregation campaign **reduces the paper's central
claim to what the rules support**; this is the largest change of the revision and
it is a reduction, not a defence:

- New subsection **"The group population is fixed by the transitions"**
  (`subsec:fixed-point`) with the membership transition list (T1-T5), the
  fixed-point statement (*every group at most 2; at most one group of population 2
  per charge word*), its proof sketch, the co-location scope paragraph, and Table
  `tab:ledger` (reference `235 K + 8 D`; open gate `145 K + 5 D` with `93` pair
  halves; candidate family rules `81 K + 162 D`).
- The abstract and the `L/3` hypothesis sentence now state the outcome: the
  reference dynamics freezes at population **2**, and the charge unit `L/3` is a
  **topological axiom of the internal dimension**, not a dynamical output (the
  same epistemic position as hypercharge normalisation in the Standard Model).
- Limitations gained the item **"The population quantum is `2`, not `L/3` (M)"**,
  with the verification at two lattice sizes.
- New measurements folded in: the full-scale candidate census (`81 K + 162 D` at
  `L=9`/`W=243`), the sieve-modulus sweep on the production census (the
  electroweak channel rearranges roles but never reaches `L/3`), the
  distinct-family mediated-duo probe (no mediator-based separation), the `P7`
  co-location control, and the `L=15` lattice-side check (`667 K + 8 D`,
  `max_population = 2`, `pop == L/3 = 0`).

## Data and reproducibility

- **Re-ran the sieve sweep** at the current kernel and replaced `tab:sieve`
  ($S=16384 \to 6468/0/0$; pairs form for every $S \le 8192$); the dependent text
  was aligned.
- Updated the **Reproducibility** section to cite `MODEL_VERSION` `model-ref-v1`,
  `model_fingerprint.ps1`, `run_all.bat` and `RESULTS_v2.md`.

## Style

- **American English** standardization (no British spellings remain).
- Float specifiers `[h]` -> `[htbp]`: no float-placement warnings remain (eight
  overfull hboxes are left in the current build).
- Split the two largest paragraphs ("Distinct bubbles"; "Diffusion, translation
  and collapse in one reading") into readable paragraphs via blank-line breaks
  only (no text change).
- **Dropped the Hofer attribution** (author request): the subsection is now
  `Spin and radial alignment` (label `subsec:spin-alignment`), and its paragraph
  states the idea without naming or citing anyone --- spin down/up as the radial
  alignment of the polarization bits around a spherical aggregate, with magnetic
  effects from breaking that spherical symmetry.  The three cross-references (the
  conjectures table, the polarization-diversity text, the $W$-island multiplicity
  text) were updated; the bibliography loses one entry (33 -> 32).

## Evidence artefacts

`experiments/` : `RESULTS_v2.md`, `ABLATION_81x3.md`, `POSTULATE_VS_EMERGENT.md`,
`PBSB_ISLANDS.md`, `RESPONSE_TO_REFEREES.md`, `WORK_PLAN.md`,
`DYNAMIC_QUANTIZATION_DERIVATION.md`, `SIEVE_SWEEP_PRODUCTION.md`,
`PHOTON_MEDIATION_FAR.md`, and the harnesses `p7_concentration.cpp`,
`island_census.cpp`, `alpha_probe.cpp` with their build scripts.
`doc/REFERENCE_CONFIG.md`; `run_all.bat`; `experiments/model_fingerprint.ps1`.

## Gate G1 — resolved: Route A

**G1 (Route A vs Route B) resolved to Route A.**  The thesis is the pre-registered
falsification campaign; the equal-charge EM repulsion (`pbsb_two_wide`,
`EM_FIRST_FSM`/`EM_NOS2B_FSM`) is a labelled **candidate** (Status C) in the
claim-evidence map.  Rationale: the positive is reproducible only with a geometry
where both EM flags light ($R \ge 5$) and with the probabilistic sieve gate either
lowered or removed (the candidate `EM_NOS2B_FSM`, a model change); at the reference
sieve ($S=16384$) the effect is zero and the reference build stays 6468/0/0.  The
negative battery is the robust, complete contribution.

*Override (Route B):* would require reframing the title, abstract and Introduction
around the repulsion and presenting `EM_NOS2B_FSM` as a proposed model extension —
a larger edit, deferred by choice.

**Strengthened (14 Sep 2026).**  The distinct-family probe closes the last
interpretation gap.  With the two equal-charge bodies in *different* charge
families (so the identity merge cannot act) and the EM reorder live
(`EM_FIRST_FSM`), the R2 photon and R1 graviton mediators produce **bit-identical
trajectories** and the pair always reaches contact; beyond the contact range there
are zero contacts at all (`PHOTON_MEDIATION_FAR.md`).  The Route-B mechanism is
therefore not merely unproven but **measured negative in its best-case
configuration**, and the earlier `W=4` "repulsion" is explained as the
identity/cohesion background being partially counteracted.  Route A stands on
stronger ground than at the time of the gate decision.
