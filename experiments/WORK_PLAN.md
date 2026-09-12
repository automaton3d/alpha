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
**Status: 4.1, 4.2, 4.3, 4.4 done.**  4.1: the reorder is inert without live
pB/sB.  4.2: with the prerequisite injected it is mechanically correct (clouds
repel).  4.3: the broadcast is **not dormant** -- it is sign-pinned (lights sB on
a wide tube); with live sB and an open sieve, `EM_FIRST_FSM` keeps two
equal-charge clouds apart (2 centres, `enc_repel` > 0) in a window s2b <= ~64.
4.4 (deepen): the reconstructed signs are **geometry-pinned** by R = RMAX-2 (sB
for R >= 3; pB only for R >= 5; neither for R <= 2) and **not** steerable by the
seeded axis (m/z/za identical).  4.5: at R = 5 (both flags live) with an open
sieve, `EM_FIRST_FSM` keeps two equal-charge clouds as two distinct S singletons.
4.6 (decisive): relaxing the sieve gate (`EM_NOS2B_FSM`) makes the separation
hold **at the reference sieve** (16384) for 32 frames -- the probabilistic gate
was the only blocker.  G1 is now a **robust positive** (candidate).  See
`PBSB_ISLANDS.md` "WP4.3".."WP4.6".

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
- **Status: in progress (build-verified).**  Done: the **postulate-vs-emergent**
  table (Limitations), the **claim-evidence map** (Results), the **sieve sweep
  `tab:sieve` re-run at current HEAD**, and the **move of `Particles` + the QM
  bridge out of Conjectures into the appendix** (now `\section`,
  `sec:Particles`/`sec:bridge`; the Conjectures intro sentence updated);
  **merged two pairs of one-line Conjectures subsections** (Color + Weak
  quantization; Spin + the Hofer effect, label kept); **moved the
  Speculative-particle-spectrum block (~60 lines) to the appendix** (now
  `\section`); and **rewrote the Introduction** to state the pre-registered
  falsification campaign explicitly and to fix the (now stale) section-map
  sentence (taxonomy/QM bridge -> appendices); **demoted two short model
  subsections** (The host lattices; What the seed does not contain) to
  paragraphs; and **strengthened the Reproducibility section** (frozen
  `model-ref-v1`, `model_fingerprint.ps1`, `run_all.bat`, `RESULTS_v2.md`) plus
  fixed the stale `6996/18/0` reference row; and **added the falsification-campaign
  outcome to the abstract**; and **added a dedicated "Falsification campaign"
  section** (`sec:campaign`, `tab:campaign`) before Results presenting the six
  pre-registered candidates and their failure at the current HEAD -- closing the
  gap where the abstract/intro promised a campaign the body did not show.  Clean
  two-pass `pdflatex` reports no warnings.  Remaining: prose tightening of the
  Conjectures.

### WP6 -- Internal review, simulated referees, polish -- ~4-6 sessions
- **Tasks:** third-party reproduction test (WP0); draft 2-3 simulated referee
  reports (hostile to the "toy universe"; sceptical of non-locality; numerics);
  respond point by point; standardise American English, notation, bibliography.
- **Deliverable:** `RESPONSE_TO_REFEREES.md` + a candidate version.
- **Acceptance:** every report point answered with evidence or a reduced scope.
- **Depends on:** WP5.

### WP7 -- Submission and data repository -- ~1-2 sessions
- **Tasks:** a data/code deposit plan (Zenodo/OSF with `run_all` + CSVs + commit
  hash + fingerprint); a submission cover letter; a single manuscript build
  pipeline (`doc/latexpdf.bat`).
- **Deliverable:** `experiments/DATA_DEPOSIT.md`, `experiments/COVER_LETTER.md`;
  the actual DOI deposit is performed by the author.
- **Depends on:** WP6.  **Status: started** -- both notes written.

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
   *STATUS: DONE (positive in a window).  The pbsb_two dormancy was an RMAX=2
   artifact; on a wide tube the broadcast lights sB, and with an open sieve
   EM_FIRST keeps the clouds apart.  See `PBSB_ISLANDS.md` "WP4.3 result".*
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
- **2026-09-11 -- WP4.5 DONE (cleanest Route-B positive).** At R = 5 (tube
  21x15x15, both pB and sB live = 2730 cells) with an open sieve, `EM_FIRST_FSM`
  keeps two equal-charge clouds as two distinct S singletons for 16 frames
  (`enc_repel` 182); at s2b=256 and the reference sieve they merge.  Recorded in
  `PBSB_ISLANDS.md`.
- **Next:** commit the WP7 notes + the readability pass; then DOI deposit and submit.
- **2026-09-11 -- Readability pass (author request).**  Split the two largest
  paragraphs ("Distinct bubbles"; "Diffusion, translation and collapse in one
  reading") into readable paragraphs by inserting blank lines at logical
  boundaries (15 breaks).  No words changed: the `.tex` grew by exactly 15 lines
  and the word content is identical.  Clean two-pass `pdflatex`: no warnings.
- **2026-09-11 -- H-SHELL instrument (`rest_shell_probe`) + first measurements.**
  New falsification instrument for the "drive-pair shell" hypothesis
  (m_rest = island + n_close), documented in `experiments/DRIVE_PAIR_SHELL.md`
  with layout (stack vs spatial shell), closure (even/odd cancel count) and
  impulse-response controls.  The instrument's four falsifiers P1-P4 all **fail**
  at the reference sieve (no pairs form there, as expected), and the control
  measurements give a new exact positive relation: with one aligned pair and
  whole-era windows, `v_body = 3/(4N)` exactly (0.250000000 / 0.150000000 /
  0.107142857 for N = 3/5/7), i.e. the inertia is the **island count** and the
  dressing is a momentum budget, not a mass.  A spatially distributed pair shell
  is not maintained by the rules (4 of 5 pairs lost, survivor detaches, no
  propulsion).  Instrument only: `experiments/` is outside the model fingerprint;
  reference build untouched.
- **2026-09-11 -- Vacuum layout: is the dressing self-formed?**  Added a `vacuum`
  layout (unformed complementary singleton material, zero seeded pairs) and an
  open-channel build `build_rest_shell_probe_boot.bat`
  (`POLAR_BOOTSTRAP_ADDRESS` + `POLAR_BROADCAST_WAVE`).  Measured: **no** pair
  forms at the reference sieve (12/12 layers stay unformed); at `S = 64` and
  `S = 32` the 12 layers become **6 pairs in a single light frame** and freeze
  (`[charges] pairM=6 form=286`).  With the election enabled the dressing acquires
  vectors and is **open** (`sum_m_x = -7`), and it stays exactly `-7` for 17
  consecutive frames: no closure (P2 falsified with a *self-formed* dressing), no
  turnover, and **no propulsion at all** (`v_body = 0.000000000`, `span = gap = 0`).
  So step 1 of H-SHELL (formation from the vacuum) is real and sieve-governed, but
  what self-forms is a static pair *stack*, not a dynamic shell.
- **2026-09-11 -- Hofer attribution removed (author request).**  Renamed
  `Spin and the Hofer effect` -> `Spin and radial alignment`, with the label
  `subsec:hofer` -> `subsec:spin-alignment` and its three cross-references (the
  conjectures table, the polarization-diversity text, and the $W$-island
  multiplicity text); reworded the paragraph as an unattributed statement of the
  radial-alignment prediction (spin down/up as the radial alignment of the
  polarization bits; magnetic effects from breaking the spherical symmetry, read
  as a Stern-Gerlach interpretation); removed `\cite{hofer}` and the bibliography
  entry.  The physics is unchanged -- only the attribution and the
  accessible-reference note went.  Build: 56 pages, zero warnings, no undefined
  references; the bibliography drops from 33 to 32 entries.
- **2026-09-11 -- Removed the appendix "Controlled two-bubble scattering and the
  rectangular tube"** (`app:gravity-probes`, 85 lines): the cube-scattering probe,
  the rectangular-tube long-axis probe, their tables and the closing status
  paragraph.  The label was referenced nowhere else and the section contained no
  other labels, so no cross-reference needed rewiring (verified: zero undefined
  references).  Build: 55 pages (was 56), zero warnings.  The harness itself
  (`experiments/gravity_probe.cpp`, `gravity_probe_DESIGN.md`, `RESULTS.md`) is
  untouched; only the paper's presentation of it is gone.
- **2026-09-11 -- Removed the Reproducibility subsection** (Conclusion, 13 lines):
  the intro paragraph (kernel revisions, `MODEL_VERSION`, fingerprint, `run_all.bat`,
  `RESULTS_v2.md`), the eight-item command list (wavefront fidelity, sieve sweep,
  scaling study, polarization fidelity, CUDA status, attractor, sieve statistics,
  charge census) and the closing inertia-probe caveat.  No `\ref` pointed at it,
  but one *textual* pointer did exist ("(program `scatter_main.cpp`,
  Reproducibility)" in the open-channel paragraph) and was fixed in a follow-up
  commit.  **Loss to note:** the words "CPU"/"GPU" appear nowhere else in the
  manuscript, so the statement that the reference build is CPU-only, that the
  archived CUDA kernels are not parity, and that the implementation must not be
  described as a strictly local integer-only transition now live only in
  `doc/REFERENCE_CONFIG.md` (gate G0) and in the experiment notes -- not in the
  paper.  Build: 55 pages, zero warnings.
- **2026-09-11 -- Emptied the Limitations subsection (author request).**  Replaced
  the entire content of "Limitations and open problems" (itemised limitation list
  plus the postulate-versus-emergent table, 33 lines) with a TODO comment.  That
  table (`tab:postulate-emergent`) was referenced once from *outside* the
  subsection -- the claim-evidence map row "One-cell-per-tick clock" -- and that
  reference was dropped in the same pass, so the build stays warning-free.  The
  classification itself survives in the Introduction's (P)/(M)/(C) grading and in
  `experiments/POSTULATE_VS_EMERGENT.md`.  **Note:** the paper now carries an
  *empty* Limitations subsection -- a gap to fill before submission.  Build: 53
  pages, zero warnings.
- **2026-09-11 -- Removed the "Future inclusions" subsection** (author request):
  the heading plus its Status (C) paragraph (preliminary observations --
  wave-packets, stable spiral patterns, macroscopic rotations, and the
  $r\cdot\sin(r)$ density cloud motivating a bridge between discrete local rules
  and smooth spacetime symmetries).  It carried no label and no cross-reference.
  The $r\cdot\sin(r)$ cloud remains documented and figured elsewhere (the wave-cloud
  text and caption), so only the conjecture paragraph went.  Build: 53 pages, zero
  warnings.
  **G1 resolved: Route A** (falsification backbone; EM repulsion a labelled
  candidate) -- see `experiments/CANDIDATE_VERSION.md`.
- **2026-09-11 -- G1 RESOLVED (Route A).**  Decided to keep the pre-registered
  falsification campaign as the thesis and the equal-charge EM repulsion as a
  labelled candidate (Status C), because the positive is reproducible only under a
  candidate model change (`EM_NOS2B_FSM`) and / or a lowered sieve, while the
  reference stays 6468/0/0.  No manuscript change is required (already in this
  framing).  Recorded in `CANDIDATE_VERSION.md`.
- **2026-09-11 -- WP7 started.**  Wrote `experiments/COVER_LETTER.md` (submission
  letter with revision summary and reproducibility) and
  `experiments/DATA_DEPOSIT.md` (deposit plan: what to deposit, the three-command
  reproduction, the table->script map).  The actual DOI deposit is the author's
  step.
- **2026-09-11 -- WP6 DONE.**  Wrote `experiments/CANDIDATE_VERSION.md` (the editor
  change summary: structural, claims/evidence, data/reproducibility, style, and the
  open G1 item).  WP6 deliverables (`RESPONSE_TO_REFEREES.md` + `CANDIDATE_VERSION.md`)
  are complete; the manuscript builds with zero warnings.
- **2026-09-11 -- WP6 (3rd increment).**  Bibliography + notation pass: `biber`
  runs clean (no warnings/errors) and the full pipeline (biber + two `pdflatex`
  passes) compiles with **zero warnings** and no undefined citations; notation is
  consistent (`\mathrm{pol}_u`, `RMAX`, `s2B`).  No changes were required.
- **2026-09-11 -- WP6 (2nd increment).**  American-English standardization per the
  manuscript's own policy: fixed the remaining British spellings
  (labelled -> labeled, programme -> program, behaviour -> behavior,
  colour(s) -> color(s), centre(s) -> center(s), neighbouring -> neighboring);
  zero remain; line count unchanged.  Clean two-pass `pdflatex`: no warnings.
- **2026-09-11 -- WP6 started.**  Wrote `experiments/RESPONSE_TO_REFEREES.md`:
  three simulated referee reports (scope/novelty; causality/no-signalling;
  numerics/reproducibility) with point-by-point responses and a consolidated
  7-item action list.  Six items were already satisfied by the WP5 revision; the
  seventh (an explicit determinism / no-seeds note in the campaign section) was
  implemented and verified (clean two-pass `pdflatex`, zero warnings).
- **2026-09-11 -- WP5 (9th increment).**  Added a dedicated "The falsification
  campaign: six candidates for the fine-structure constant" section
  (`sec:campaign`, `tab:campaign`) immediately before Results, with the six
  pre-registered candidates and their current-HEAD outcomes (all FAIL), the
  reproducible-table pointer, and the dial-free S-scan note.  This makes the
  abstract / Introduction / claim-evidence map consistent with the body text
  (previously the campaign was promised but not shown).  Clean two-pass `pdflatex`
  (aux removed): no warnings.
- **2026-09-11 -- WP5 (3rd increment).**  Moved the `Particles` taxonomy and the
  `Cellular automata and quantum formalism` bridge out of the Conjectures section
  to the appendix (converted to `\section`, labels untouched), and updated the
  Conjectures intro sentence that announced them.  A clean two-pass `pdflatex`
  (aux deleted) now reports **no warnings**.  Gotcha: appending the block after
  `\end{document}` leaves the labels undefined -- it must precede it.
- **2026-09-11 -- WP5 (2nd increment).**  Re-ran the manuscript's sieve sweep at
  current HEAD (L=7, SEP=4, 200 frames; `alpha_probe`) and replaced
  `tab:sieve`: $16384\to6468/0/0$; pairs form for every $S\le8192$ (26 at 8192,
  1,144 at 64, 2,602 at 16).  Aligned the two sentences that quoted the old
  6996/18/0 and $\{4096,128,64,32\}$ values, and added a "Current-HEAD values"
  note after the table.  `pdflatex` exit 0.
- **2026-09-11 -- WP5 started.**  Added two build-verified tables to
  `doc/manuscript.tex`: postulate-vs-emergent (Limitations) and claim-evidence
  (end of Results).  Reminder: `doc/manuscript.pdf` is tracked, so rebuilds show
  as modifications.  Full build: `doc\latexpdf.bat`; a syntax check is one
  `pdflatex` pass (exit 0, no `!` errors).
- **2026-09-11 -- WP4.6 DONE (decisive Route-B positive).** Added the candidate
  macro `EM_NOS2B_FSM` (`interaction.cpp`): the EM reorder guard drops the
  probabilistic `s2B` requirement.  At R = 5 with the **reference sieve (16384)**
  the two equal-charge clouds now stay S=2 at 2 sites for 32 frames (enc_repel
  182->434) -- where the gated build merged.  The sieve gate was the only
  blocker.  Reference build re-verified 6468/0/0; fingerprint advanced
  `937449...` -> `428ece...`.  Recorded in `PBSB_ISLANDS.md`.
- **2026-09-11 -- why R=5.** R = 5 is NOT qualitatively special: R = 6 also keeps
  S=2 but only for s2b <= 32 (R=5 holds s2b <= 64).  The window width tracks the
  shell amplitude u_shell(R) (centre u ~280 at R=5 vs ~34 at R=6), so it is a
  quantitative amplitude effect.  Since both clouds share the seeded axis the
  contact flags match, so the merge only happens on gate misses.  Recorded in
  `PBSB_ISLANDS.md`.
- **2026-09-11 -- window mechanism.** The sieve window closes because the `s2B`
  gate fires with P ~= u_active/S: at R=5 the identity merge sets in between
  S=64 and 128 (shell u ~64-128; centre u ~280) and the spatial collapse at
  S=256.  The merge is two-stage (identity at frame 8, collapse at frame 11 at
  S=256).  Recorded in `PBSB_ISLANDS.md`.
- **2026-09-11 -- R=5 robustness.** The R=5 separation (two equal-charge clouds
  kept as S=2) persists for 32 frames (enc_repel 182->308->434) and is
  axis-independent (same vs opposite seeded axes give the identical census).  The
  only fragile parameter is the sieve modulus (window s2b <= ~64).  Added a frames
  argument to `pbsb_two_wide`.  Recorded in `PBSB_ISLANDS.md`.
- **2026-09-11 -- R map (flag thresholds).** Extended the WP4.4 sweep to R = 6..9
  (EL = 17..23, `alpha_probe pol m`): `pB` (electric) lights only for **R >= 5**;
  `sB` (magnetic) for **R >= 3**; neither for **R <= 2**.  The |pol_v|/|pol_u|
  ratio falls toward 1 with R.  Recorded in `PBSB_ISLANDS.md`.
- **2026-09-11 -- WP4.4 DONE (broadcast structure).** `alpha_probe ... pol <axis>`
  sweep: the polarization flags are **geometry-pinned** by R = RMAX-2 -- sB
  lights for R >= 3, pB only at EL=15 among the tested sizes, neither for
  R <= 2 -- and are **independent of the seeded axis** (m/z/za identical).  The
  axis seed cannot steer pB vs sB.  Recorded in `PBSB_ISLANDS.md`.
- **2026-09-11 -- WP4.3 DONE (positive in a window).** New harness
  `experiments/pbsb_two_wide.cpp` (ordinary + `/D EM_FIRST_FSM`): the pbsb_two
  "dormant broadcast" was a short-edge artifact (RMAX-2 = 0 zeroes pol); on a
  21x13x13 tube the broadcast lights `sB` (1790 cells).  With live sB and an open
  sieve, `EM_FIRST_FSM` keeps two equal-charge clouds at 2 centres (enc_repel>0)
  in the window s2b <= ~64 (merge at the reference sieve).  G1 is now
  positive-in-a-window.  Recorded in `PBSB_ISLANDS.md`.
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


