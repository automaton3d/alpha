# WORK PLAN -- completing the alpha project and making the manuscript peer-review ready

Status: **active**.  Last updated: 11 Sep 2026.
Strategy decision: **Route A (falsification/methodology paper) as the backbone,
with WP4 (photon/EM mediation) pulled forward as a time-boxed spike** to try to
land one clean positive before the manuscript rewrite.

This file is the single source of truth for what remains.  It complements (does
not replace) `REVISION_PLAN.md` (the manuscript blueprint) and `RESULTS.md`
(the campaign record).

---

## 0. Executive summary

The scientific campaign is finished and its result is mostly negative and
honest: the six pre-registered candidates for the fine-structure constant all
fail; the superposed Platonic seed does not aggregate into islands; the
photon/graviton mediation is at a documented geometric impasse.  What remains is
to turn that into a publishable artefact.

The defensible assets of the paper are:

1. a **fully specified** cellular-automaton universe, with every claim graded
   as postulate (P), measurement (M) or conjecture (C);
2. a **pre-registered falsification campaign** (alpha_A..F) that is reproducible
   and falsifiable -- all six fail;
3. a small set of **rigorous controlled positives** (K-only island transport;
   the family-preserving `81 x 3` exclusion control);
4. a **strong quantitative negative** (no `1K+nD` self-assembly from the seed).

Everything else (gravity, black holes, spin, partons, observers, the QM bridge)
is conjecture and belongs in clearly labelled appendices.

> **Organising principle:** one frozen *reference build* -> one reproducibility
> script -> **every number in the paper traceable to a figure/table script** ->
> a claim-vs-evidence matrix.  This is what makes or breaks peer review.  Today
> the paper still carries "historical" tables (e.g. 6996/18/0) that no longer
> reproduce (current HEAD gives 6468/0/0).  That is the single biggest risk.

## 1. Diagnosis: solid / fragile / missing

| Layer | State | Evidence |
|---|---|---|
| Model definition (P) | **Solid** | `simulation.h`, `manuscript.tex` 127-423 |
| alpha_A..F (6 negatives) | **Solid but drifted** | `RESULTS.md` (6468/0/0 vs historical 6996/18/0) |
| K-only inertia (positive) | **Solid** | `INERTIA_REVALIDATION.md` -- 4 K-only cases pass |
| Multi-constituent inertia | **Fragile** | does not preserve a single chief; matched fails 7/9 |
| Island aggregation | **Solid negative** | `ISLAND_CENSUS.md` run64 (absorbing plateau) |
| EXCLUSION 81x3 | **Mechanism, not physics** | `ABLATION_81x3.md`; labelled a stand-in |
| Mediated repulsion (photon) | **Impasse** | `PHOTON_MEDIATION_DESIGN.md` schemes A-E; E negative |
| EM between equal-charge islands | **Unreachable in current code** | `PBSB_ISLANDS.md` (encounter order) |
| CUDA / GPU | **Broken** | Makefile expects v13.2, only v11.8 installed |
| Reproducibility (README/scripts) | **Weak -> improving** | README written in WP0; scripts were scattered |
| Historical tables in the paper | **Risk** | labelled, but not regenerated |
| Paper structure | **Bloated** | 1674 lines, ~30 one-line subsubsections, large Conjectures |

**Consequence for review:** an IJUC referee will ask "what is the scientific
contribution?".  The honest answer is *"a rigorous falsification campaign plus a
set of controls"*.  The paper must be rewritten **around that thesis**.

## 2. Strategy

- **Route A (backbone, recommended).** Scope: "a fully specified cellular
  automaton + a pre-registered falsification campaign + rigorous controls".
  All negatives are the result; the controlled positives carry the toolset;
  conjectures move to labelled appendices.  Material already exists; risk low.
- **Route B (stretch, gated).** Only if WP4 yields one clean positive (a
  physical photon-mediated repulsion between equal-charge islands, or a stable
  single-chief multi-constituent transport).  Then the paper's thesis upgrades.

The plan never blocks on Route B: G1 decides, and Route A proceeds either way.

---

## 3. Work packages

Each WP lists objective, tasks, deliverable, **acceptance criterion** and effort
(sessions; simulation wall-clock cited separately -- ~50 s/frame at L=9 on CPU).

### WP0 -- Hygiene, toolchain and reproducibility baseline (foundation) -- ~3-5 sessions
- **Tasks:** write `README.md` (repo map, build, per-campaign run); park pre-WP0
  root clutter in `attic/`; resolve the CUDA v11.8 vs v13.2 mismatch (fix OR
  declare GPU out of scope); provide one `run_all` script that regenerates every
  figure/table.
- **Deliverable:** a clean clone where `run_all` reproduces a published table
  within tolerance; a written toolchain decision (gate G0).
- **Acceptance:** a third party reproduces without contacting the author.
- **Depends on:** nothing.  **Status: DONE** (README, `attic/` cleanup, G0 decision,
  and a validated `run_all.bat`).
- **Known follow-up (hygiene):** 22 of the `experiments\build_*.bat` scripts still
  lack `/Fo`, so they dump their `*.obj` into the repository root; `build_probe.bat`
  (the reference one used by `run_all.bat`) is fixed.  Add `/Foobj\` to the rest in
  a later hygiene pass.

### WP1 -- Freeze the model and the reference configuration -- ~1-2 sessions
- **Tasks:** tag a `MODEL_VERSION`/commit; define the reference build (all
  candidate macros off); write `doc/REFERENCE_CONFIG.md`; flag dead code paths
  (M/Mbar hook, `color_fsm` outside the ordinary build).
- **Deliverable:** commit hash + config doc referenced by *every* number in the
  paper.
- **Acceptance:** every paper table cites the `MODEL_VERSION` (the `.tex`
  citations are applied in WP5, once the numbers are regenerated; WP1 delivers
  the anchor).
- **Depends on:** WP0.  **Status: DONE** -- commit `4501f25`, tag `model-ref-v1`,
  model fingerprint `5b0944...`, `doc/REFERENCE_CONFIG.md` written, and
  `experiments/model_fingerprint.ps1` validates the fingerprint (exit 0).

### WP2 -- Re-validate every quantitative claim (kill the historical tables) -- ~10-20 sessions + heavy runtime
- **Tasks:** re-run alpha_A..F at L=7,9,11,13; measure the dispersion over the
  swept sieve modulus S (NOT seeds -- the reference path is deterministic, see
  `RESULTS_v2.md`) and state a pre-registered convergence criterion; re-run the
  two-bubble scattering and gravity probes at reference parameters; re-run the
  proof-of-concept census.
- **Deliverable:** `experiments/RESULTS_v2.md` with every table regenerated plus
  the driver `experiments/run_wp2_sweep.ps1`.
- **Acceptance:** no table is labelled "historical"; everything reproduces in
  <=1 driver run.
- **Depends on:** WP1.  **This is the heaviest and most review-critical WP.**
- **Status: DONE.** alpha_A..F re-validated at current HEAD; determinism
  established (dispersion is over S, not seeds); the full 32-point S-comb
  measured at L=7 with an L=9 stability check; figure generated. See
  `RESULTS_v2.md`.  (Optional: extend the comb to more L; add further figures.)

### WP3 -- Close the catalogued evidence gaps (`REVISION_PLAN.md` sec.3) -- ~6-10 sessions
1. A single **ablation table for the 81 x 3 quantisation** (duo2/4, cube,
   spread81, production port -- with/without EXCLUSION).
2. **spread81 EXCLUSION+push, 12+ frames** and a pairwise-distance distribution
   (~1.5-2 h/run).  *STATUS: delivered from the kept `sp81xL` groups.csv (no new
   run): membership is a delta at population 3 for all 81 islands across frames
   2-12; the extent broadens 6 -> 0..22.  See `ABLATION_81x3.md`.*
3. **Production 1K+2D**: fix the intra-family promotion (currently 2 K + 1 D).
   *STATUS: WP3.3 fix implemented + validated at rule level (see `ABLATION_81x3.md`).*
4. **Postulate-vs-emergent control**: an ablation showing the 1-cell/tick front
   follows from the rule set (defensive, cheap).
   *STATUS: delivered as `POSTULATE_VS_EMERGENT.md` (table + code anchors,
   answering the editor's "speed/polarisation prescribed" point).*
5. **Scaling with error bars** (multiple seeds) for alpha_A.
- **Deliverable:** new tables/figures ready to drop into the paper.
- **Acceptance:** every number carries an ablation control and/or a statistical
  error.
- **Depends on:** WP2.
- **Status: essentially DONE.** WP3.1 (the single ablation table), WP3.2
  (spread81 membership/extent distribution, from kept data), WP3.3 (the
  production 1 K + 2 D fix, validated at rule level with `promotion_three`) and
  WP3.4 (postulate-vs-emergent table) are delivered; WP3.5 is the WP2 S-scan.
  The only open item is the full-scale WP3.3 confirmation (island_census at L=9,
  heavy).

### WP4 -- (PULLED FORWARD, time-boxed) Deepen one positive -- ~8-20 sessions, uncertain
See the detailed breakdown in section 4.  **Gate G1** decides Route B vs Route A.
**Status: 4.1 and 4.2 done.**  4.1: the reorder (already `EM_FIRST_FSM`) is
inert without live pB/sB.  4.2: with the prerequisite injected
(`EM_FORCE_PREREQ`), two equal-charge clouds **repel** (sites=2, dx 2->9,
enc_repel=50) -- the reorder is **mechanically correct and sufficient**.  The
remaining blocker is the polarization broadcast (WP4.3).  G1 stays open pending
a working broadcast.  See `PBSB_ISLANDS.md` "WP4.2 result".

### WP5 -- Restructure and rewrite `manuscript.tex` -- ~8-12 sessions
Executes `REVISION_PLAN.md` sec.4 on the current structure:
- Reorder: **Model (facts) -> Falsification campaign (moved up from Results) ->
  Controlled positives -> Candidates (clearly experimental) -> Limitations ->
  Appendices**.
- **Merge** the ~30 one-line subsubsections into paragraphs; single notation
  table (today `Nomenclature` at 1455 plus scattered tables).
- Move `Particles` (1371) -> Appendix C; `QM bridge` + no-signalling (1387) ->
  Appendix D, labelled speculative; **halve** the Conjectures section (1222).
- Rewrite the Introduction scope sentence (66) to the falsification framing.
- Add the **postulate-vs-emergent** table (in Limitations, 1426) and a
  **claim-vs-evidence matrix**.
- **Deliverable:** restructured `manuscript.tex` + every figure/table regenerated
  from WP2/WP3.
- **Acceptance:** no unlabelled claim; no historical table; matrix complete.
- **Depends on:** WP2 + WP3 (never touch the `.tex` before the numbers exist).

### WP6 -- Internal review, simulated referees, polish -- ~4-6 sessions
- **Tasks:** third-party reproduction test (WP0); draft 2-3 simulated referee
  reports (hostile to the "toy universe"; sceptical of non-locality; numerics);
  respond point by point; standardise American English, notation, bibliography.
- **Deliverable:** `RESPONSE_TO_REFEREES.md` + a candidate version.
- **Acceptance:** every report point answered with evidence or a reduced scope.
- **Depends on:** WP5.

### WP7 -- Submission and data repository -- ~1-2 sessions
- Zenodo/OSF with `run_all` + CSVs + commit hash; cover letter; a single
  manuscript build pipeline (fold the `build/manuscript_lote*` PDFs into one).
- **Depends on:** WP6.

---

## 4. WP4 in detail -- photon/EM mediation spike (pulled forward)

**Objective:** produce physical (non-stand-in) repulsion between two equal-charge
islands, OR document definitively why the geometry forbids it.

**Known root cause (starting point, do not rediscover):**
- In `encounter()`, `chiefContact()` runs **first** and merges equal-charge
  sources across families; `K x K` / `D x D` equal-charge then `return false`
  **before** the `electricContact`/`magneticContact` branches.  The EM channel
  therefore **never fires** for two equal-word islands (`PBSB_ISLANDS.md`;
  confirmed by `pbsb_two` -- both the +x/+x and +x/-x variants **merge**).
- The photon-mediation line (`PHOTON_MEDIATION_DESIGN.md`) hit a **geometric**
  impasse: two expanding shells overlap on a thin ring; a mediator either stays
  on it (scheme A: a kick every tick, no clean law) or leaves it (propagating
  B/C/E: intermittent windows, weak repulsion).  The documented next candidate is
  **transferring the momentum BY the mediator** (per-engagement, integrated over
  the window).

**Tasks (sequential, with a stop rule):**
1. **4.1 -- Reorder EM for dressed islands.** Decide the EM channel (pB/sB)
   **before** the identity merge when the sources carry dressing; keep
   `chiefContact` only when no EM branch applies.  **Invariant: the default build
   stays 6468/0/0.**  *STATUS: DONE -- the reorder already existed as
   `EM_FIRST_FSM`; A/B tested, it is **inert** because pB/sB are never live at
   the contact voxels.  Default invariant reconfirmed (6468/0/0, alpha_A
   0.003756878).  See `PBSB_ISLANDS.md`.*
2. **4.2 -- Inject the prerequisite (DONE, positive).** Force `s2B`+`pB` at each
   contact (`EM_FORCE_PREREQ`) to isolate the reorder from the (dormant)
   broadcast.  Result: the two equal-charge clouds do not merge but **repel**
   (`sites`=2, dx 2->9, `enc_repel`=50); the 4.1 reorder is mechanically correct
   and sufficient.  See `PBSB_ISLANDS.md` "WP4.2 result".
3. **4.3 -- Make the broadcast light pB/sB (the real blocker, next).** The
   seeded axis never reconstructs pol_u/pol_v on the shells (WP4.1).  Getting
   the polarization broadcast to supply pB/sB on overlapping shells is now the
   critical path to a Route B positive.
4. **4.4 -- Controls.** W-matched bare, broken, freq0, swapped-halves, off-axis,
   mag=0 (already implemented in `10c7cb7`).
5. **4.5 -- Observable.** Do two prepared equal-charge dressed islands stay two?
   (counters `enc_collapse/enc_adiah/enc_repel` + centres per frame).

**Time-box:** a fixed cap (sessions / runs).  On expiry, **close as a negative
with root cause** and resume the Route A backbone.  **Labelling:** the outcome,
positive or negative, is a **candidate (C)** -- never part of the model (P).

## 5. Decision gates

| Gate | When | Question | Consequence |
|---|---|---|---|
| **G0** | before WP2 | GPU (fix CUDA) or declared CPU-only? | runtime budget |
| **G1** | right after WP4 | Is there reproducible EM repulsion with a control? | **Route B** (upgrade) vs **Route A** (documented negative) |
| **G2** | before WP5 | Do all numbers reproduce? | releases the `.tex` rewrite |
| **G3** | end of WP6 | Does it pass the simulated referees? | submission |

## 6. `manuscript.tex` edit plan (concrete, by section)

| Line/section | Action |
|---|---|
| 66 Introduction | rewrite the scope sentence (falsification framing) |
| 106 Related work | ok; check positioning |
| 127-423 Model (fabric/initial/phase/light-frame) | merge subsubsections; single notation table |
| 791 Interactions | fold in the EM-vs-identity ordering finding (`PBSB_ISLANDS.md`) |
| 923 Dynamic charge quantization | tighten |
| 979 Results | **replace every historical table** with WP2 output; add a "Falsification campaign" subsection |
| 1130 island-candidates | rewrite with the WP3.1 ablation table |
| 1222 Conjectures | cut ~50%; `Particles`->Appendix C; `QM bridge`->Appendix D |
| 1412 Conclusion | ok; strengthen Limitations + postulate-vs-emergent |
| 1441 Reproducibility | expand with `run_all` and `MODEL_VERSION` |
| 1455 Nomenclature | merge into the single notation table |
| 1519+ Appendices | reorganise; mark speculation |

## 7. Risks and mitigations

1. **Compute cost** (~50 s/frame, L=9 CPU) -> G0 decides GPU; else budget
   wall-clock and parallelise seeds.
2. **Reproducibility drift** (already happened once) -> WP1 freezes the reference
   build; every table cites the hash.
3. **Scope creep** from the Conjectures section -> WP5 cuts; "move to appendix"
   is the rule.
4. **Referee hostility to the "toy universe"** -> mitigated by the falsification
   framing + P/M/C grading + evidence matrix.
5. **The photon line blocks the paper** -> mitigated by the time-box + G1: Route A
   never depends on it.
6. **EXCLUSION is a stand-in** -> label it explicitly as a candidate; never sell
   it as physics.

## 8. Recommended sequence

`WP0 -> WP1 -> WP4 (time-boxed spike) -> [G1] -> WP2 -> WP3 -> [G2] -> WP5
-> WP6 -> WP7`.

The heaviest, highest-value work is **WP2 + WP3** (regenerate the evidence); the
`.tex` is only touched after that.  WP4 runs early in parallel and is protected by
its time-box.

## 9. Progress log

- **2026-09-11 -- WP0 DONE.** `README.md` written; pre-WP0 root clutter (74
  files: build/run logs, loose `*.obj`, stale `automaton.exe`, duplicate root
  DLLs) parked in `attic/` and `attic/` added to `.gitignore`; **G0 decided:
  CPU-only is the reference, GPU path declared unsupported** (Makefile/CUDA
  version mismatch + engine refuses GPU activation); `run_all.bat` created and
  validated end to end (GUI build OK, probe build OK, probe smoke OK, exit 0).
  Gotcha found and fixed: the ambient `VCPKG_ROOT` pointed at the VS-bundled
  vcpkg without freetype; `run_all.bat` now forces `ALPHA_VCPKG`
  (default `E:\vcpkg\installed\x64-windows`).
- **2026-09-11 -- WP2 started (alpha_A re-validated).** Established that the
  reference path is fully deterministic (two L=7 runs byte-identical except the
  wall-clock line), so dispersion must come from the swept sieve modulus S, not
  seeds.  Re-ran the alpha_A L-sweep at current HEAD: L=7 6468/0/0 (266.18),
  L=9 7700/0/0 (267.42), L=11 12382/32/32 (107.13), L=13 10520/80/80 (101.36)
  -- matches `RESULTS.md`.  Wrote `experiments/RESULTS_v2.md` and the driver
  `experiments/run_wp2_sweep.ps1` (L-sweep + optional S-scan).  alpha_A remains
  strongly L-dependent -> FAILS.  Reference invariant preserved (fingerprint
  MATCH).
- **2026-09-11 -- WP2 DONE.** Full 32-point step-512 S-comb at L=7 (20 open /
  12 closed; open-ratio median 238.6, mean 611.1) plus an L=9 stability check
  (same S, up to ~6.6x larger ratio); sampling sensitivity documented.  Added
  `experiments/plot_wp2_scan.ps1` (dependency-free SVG -> `build\wp2\S_scan_L7.svg`).
  `RESULTS_v2.md` is the canonical reproducible table set.  alpha_A remains
  L-dependent -> FAILS; no dial-free constant.
- **2026-09-11 -- WP3 started (81x3 ablation consolidated).** Re-ran the duo rows
  fresh (duo2/duo4, +/-EXCLUSION) -- they reproduce the documented runs exactly
  (no-rule merges K=1/D=5; EXCLUSION keeps 2 islands of 3).  Consolidated the
  single ablation table + reproduction status in `ABLATION_81x3.md`, including
  the documented cube (`exc_cube`/`exc_cube2`: 81x3 at 3 / 167 centres),
  spread81 (`sp81x`/`sp81xL`: 81x3, 6-12 frames, unresolved 0; `s81t` no-rule
  merges K 81 -> 8) and production rows (`run64` absorbing plateau; `prod_exc`
  162/81 at 17 centres).  Cube/spread81 cost ~25 s/frame and were not re-run
  this session.
- **2026-09-11 -- WP3.3 DONE (rule level).** Root-caused the production 2 K + 1 D
  to the pairwise minimum-address election inside one family.  Added the
  macro-guarded candidate `DD_INTRA_ISLAND_FIX` (`chief_transition.h`): a
  same-family S x S contact elects the family-minimum chief.  New minimal harness
  `experiments/promotion_three.cpp` (+ `build_promotion_three.bat`) shows
  reference K=2/D=1 vs fix K=1/D=2 for a 3-copy family.  Reference build
  re-verified 6468/0/0; model fingerprint advanced `29ef0e...` -> `937449...`.
- **Next:** commit the WP0-WP3 milestone; then the full-scale WP3.3 confirmation
  (island_census L=9, heavy) or WP4.3 (broadcast).  Heavy work needs long batches.
- **2026-09-11 -- WP3.2 DONE (from kept data).** From the existing
  `sp81xL_seed_1/groups.csv` (no new run): all 81 islands have population exactly
  3 at every sampled frame (2-12), `families == 1`, unresolved 0; the extent
  (`max_distance_to_chief`) broadens 6 -> 0..22.  Recorded in `ABLATION_81x3.md`.
- **2026-09-11 -- WP3.4 DONE.** Wrote `experiments/POSTULATE_VS_EMERGENT.md`: a
  postulate/emergent/candidate table with code anchors for every model
  ingredient, answering the editor's "speed/polarisation prescribed" point (the
  1-cell/tick clock and the polarization broadcast are POSTULATES; only charge
  conservation and the observer reading of c are emergent).
- **2026-09-11 -- WP2 continued (S-scan at current HEAD).** Measured the S-scan
  comb at L=7 (15-point sample, 128..16384): non-monotone, 10 open / 5 closed
  (gate shuts for S >= 7808); open ratios min 7.15, median 108.4, mean 129.6
  (1/mean ~ 1/129.6 -- near but not 1/137).  L-stability check at the same S:
  L=9 gives much larger ratios (S=5248: 164 -> 1081), confirming the coupling is
  lattice-size specific.  Added `experiments/analyze_wp2_scan.ps1`; the driver
  now accumulates S-scan rows in batches.  `RESULTS_v2.md` updated.
- **2026-09-11 -- WP4.2 DONE (positive).** Added the macro-guarded
  `EM_FORCE_PREREQ` hook (`interaction.cpp`, before the `EM_FIRST_FSM` block)
  that forces the reorder's prerequisite (s2B + pB) at each contact, and the
  build `experiments/build_pbsb_two_forced.bat` (`/D EM_FIRST_FSM
  /D EM_FORCE_PREREQ`).  Result: with the prerequisite present the two
  equal-charge clouds **repel and stay two** (sites=2, dx 2->9, enc_repel=50,
  enc_collapse=50) instead of merging -> the reorder is mechanically correct and
  sufficient; the blocker is the (dormant) polarization broadcast.  Reference
  build re-verified behaviour-identical (6468/0/0, alpha_A 0.003756878); the
  model fingerprint advanced from `5b0944...` to `29ef0e...` (macro OFF in the
  reference), recorded in `doc/REFERENCE_CONFIG.md`.  G1 stays open pending
  WP4.3.
- **2026-09-11 -- WP4.1 DONE (negative/inert).** Found the reorder already
  implemented as `EM_FIRST_FSM` (`interaction.cpp:884-984`).  A/B test of
  `pbsb_two` (default vs `/D EM_FIRST_FSM`, new script
  `experiments/build_pbsb_two_em.bat`): identical traces -- both equal-charge
  clouds merge at frame 2, pB/sB = 0, EM counters = 0.  Root cause: the seeded
  axis never lights pB/sB (`pol=(0,0)`, `bstamp=2`), so the guard is never true.
  Reorder is necessary but not sufficient; the blocker is the polarization
  broadcast.  Reference invariant reconfirmed (active-passes 6468, s2B 0,
  pairs 0, alpha_A 0.003756878) and model fingerprint still MATCH.  Documented
  in `PBSB_ISLANDS.md`; G1 stays open pending 4.2.
- **2026-09-11 -- WP1 DONE.** Froze the reference: annotated git tag
  `model-ref-v1` at commit `4501f25` (branch `polarization-broadcast-fix`);
  model source fingerprint (SHA256 over `src/model/*.cpp|inc` +
  `src/include/model/*.h`) = `5b0944258b8f9b750688375f2e9a4cd99e74f0355f1c6b1aa8edc769ab743673`;
  wrote `doc/REFERENCE_CONFIG.md` (MODEL_VERSION, compiler flags, the full
  candidate-macro inventory, per-file fingerprint, reference recipes, G0
  decision, dead/experimental paths); added `experiments/model_fingerprint.ps1`
  (validated: matches, exit 0).  Note: 22 `experiments/build_*.bat` still lack
  `/Fo` (hygiene follow-up from WP0).


