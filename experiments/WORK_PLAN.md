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
- **2026-09-11 -- Removed the "Charge conservation" diagnostic paragraph** (Results,
  in-simulation diagnostics): the closure-identity check with its run numbers
  ($D_{\mathrm{tot}}=+375$, $D_{\mathrm{isl}}=+279$, $D_{\mathrm{orph}}=+96$,
  $D_{\mathrm{Orb}}=+2500$, $D_{\mathrm{Umb}}=-2125$, $\mathrm{freeM}=4875$,
  $\mathrm{freeA}=4500$, constant over $29$ checkpoints).  Two sentences that
  announced it were aligned in the same pass: the run scope ("for the charge and
  bound-state diagnostics" -> "for the bound-state diagnostics") and the
  not-yet-measured list (dropped "charge conservation,").  The closure identity
  itself survives in the proof-of-concept list ("a live charge census whose closure
  identity ... is verified every 256 ticks"), so the claim is not lost -- only its
  dedicated numbers.  Build: 53 pages, zero warnings.
- **2026-09-11 -- Removed the "Claim--evidence map" subsection** (Results, 22 lines:
  intro paragraph plus the `tab:claim-evidence` table mapping seven claims to their
  artefacts).  Nothing referenced it: `tab:claim-evidence` and `subsec:claim-evidence`
  were cited only from its own intro, and the one external pointer (from the removed
  falsification-campaign section) was already gone; the Limitations TODO comment that
  mentioned the map was reworded for accuracy.  **Note:** the paper no longer carries
  a claim-to-artefact map, so provenance now rests on the per-table captions and on
  the experiment notes.  Build: 52 pages, zero warnings.
- **2026-09-11 -- Referee response: Standard-Model naming confusion.**  Added to the
  Nomenclature (i) a **naming convention** -- a Standard-Model name carrying a
  qualifier (fragment / pair / pattern / cluster / channel / -type) denotes a *model*
  object, a bare name denotes the *observed* particle -- and (ii) a **decoder table**
  mapping each Standard-Model name to the model object it denotes here, with its
  status.  Qualified the model-object uses throughout: interaction channels, the
  magnetic channel, neutrino-type fragments, the up/down-quark fragment subsection,
  the colour-combination legend, the algorithm comments for rules R1..R6, the
  pair-rule table, the combinatorial-census prose and table, the Coulomb relay
  mechanism, and the gravity conjecture.  Retitled `Particles` ->
  `Conjectured identifications with Standard-Model particles` and added the
  `sec:spectrum` label.  New instrument `experiments/audit_sm_terms.ps1` reports every
  unqualified Standard-Model noun outside the exempt taxonomy sections: **86 -> 35**
  candidates, and the remaining 35 were reviewed individually and are legitimate
  (cosmology bookkeeping -- CMB photon, proton rest mass (CODATA), Eddington number;
  observer/relativity text; conjectures tagged (C)).  Build: 52 pages, zero warnings.
- **2026-09-11 -- Terminology fix: "tick" (author-flagged ambiguity).**  The word "tick"
  was doing three jobs: the housekeeping step $k$, the wavefront counter $t$, and the
  light frame itself (L1196 said "one cell per tick" and "1.0000 cells/frame" in the
  same bullet; L1014 said "one cell per frame" and "$N_t$ = number of ticks per frame"
  in the same sentence; L702 called the light frame "one synchronous $k$-tick" against
  L1037's "a light frame of FRAME ticks").  Fixes: added a **Timescale convention** to
  the Nomenclature ("tick" unqualified = housekeeping tick $k$, atemporal; the physical
  clock is the light frame, counted by $t$; an era is $L$ light frames; host cost
  $N_k$); renamed the definitions ("Wavefront tick ($t$)" -> "Light-frame counter ($t$)",
  the two symbol-table rows, "$N_t$" -> "$N_k$"); every physical-rate statement that
  said "per tick" now says per **light frame**; the stage names "the DIFFUSION/RELOC
  tick" -> "stage"; and the $v_{\max}$ relation is now $v_{\max}=X/\tau_{\mathrm{lf}}$
  with $\tau_{\mathrm{lf}} \equiv 1$ light frame, since the old $X/N_t=v_{\max}$
  conflated an implementation count with a physical speed.  Same fix applied in
  `PHOTON_MEDIATION_DESIGN.md` and `RESULTS.md` where "per tick" meant per light frame.
  **Finding:** the no-signaling lemma bounds the *dependence* cone at one cell per
  housekeeping tick, whereas $v_{\max}$ is one cell per light frame -- the two differ by
  the factor $N_k$.  A "Note on the two cone rates" now states this explicitly and marks
  the light-frame bound for readable fields as still to be derived.  Build: 54 pages,
  zero warnings.
- **2026-09-12 -- WP8: producer audit and the ported directional producers.**
  Audited every directional field for a *producer* (`homB`, `gB`/`g[3]`, `c[3]`,
  `m`, `bstamp`, `pol_u/pol_v`, `pB`, `sB`) against what the rules say should
  produce it.  Findings: (i) `homB` has a **consumer** in the CPU
  (`interaction.cpp` SLOT II "Homing using homB") but its **four producers exist
  only in the archived CUDA kernel** (`cuda_automaton.cu` `dev_encounter4/6/7`;
  `git log -S 'draft.homB = 1'` shows they were never in `src/model`); (ii)
  `gB`/`g[3]` have no producer in *any* path (only `seed_test.cpp` sets them);
  (iii) `c[3]` is propagated but never *initiated*, so it stays zero and `RELOC`
  has nothing to move; (iv) the `m` elector **exists** but its precondition is an
  explicit fixed point ("no axis can be elected without existing polarization"),
  broken once per layer by the deterministic candidate
  `POLAR_BOOTSTRAP_ADDRESS`.  Measured: with `POLAR_BOOTSTRAP_ADDRESS` at
  `9x7x7` the whole layer-1 chain activates (`bstamp=455`, `pol=(1,0)`, `pB=1`
  in 32/36 samples) whereas the reference shows `bstamp=0`, `pol=(0,0)`,
  `pB=sB=0`; and **`reconstructPair` degenerates below RMAX=4** (`R=RMAX-2`; R=0
  kills both components, R=1 kills the quadrature bit `sB`), so any pB/sB
  measurement in a short-side-5 or -7 tube is void.  Implemented the port as the
  candidate macro **`HOMB_PRODUCER_FSM`** (`interaction.cpp` ~899) with a new
  `homb_events` counter (declared in `interaction.cpp`, exposed in
  `simulation.h`).  Reference verified **bit-identical with the macro OFF**:
  `alpha_probe 7 4 200 16384 256` -> active-passes 6468, s2B 0, pairs 0,
  `alpha_A = 0.003756878`; new model fingerprint
  `5e60eaeb1444d508c2f3dcc7aef34ccba4f5d3eb58e08571f7965688f4d42b15`.
  First measurement of the candidate at `15x9x9` (RMAX=4, R=2): the producers
  **fire** (`homb_events = 5`) and `sB` is live (40/59 samples) -- but the body
  still does not move (`MEAN_V = 0`) and the end-of-run counters for `c`/`homB`
  read zero because those fields are cleared every frame
  (`interaction.cpp:1485-1487`), so the effect must be sampled *within* the
  frame; also the prepared 3-element island is a single family, which disables
  the address tie-break path.  Next: per-frame instrumentation and a
  multi-family seed.
- **2026-09-12 -- WP8 option C: the consumer chain MEASURED end to end.**  Four
  build-time probes were added (all under `HOMB_PRODUCER_FSM`, so the reference
  stays bit-identical: re-verified `6468/0/0`, `alpha_A = 0.003756878` after every
  edit): `homb_seen` (the pre-existing SLOT II homing block finds a homer),
  `c_at_center`/`cB_at_center` (the `c[]`/`cB` field has ARRIVED at a source
  centre, where `applyMomentum` reads `reloc[]`), `reloc_moves` (a source centre is
  relocated), `reloc_cells` (a cell migrates through the lattice in `relocate()`).
  At `15x9x9`, `N=3`, 16 light frames, one family:
  `homb_events=7  homb_seen=184  c_at_center=930  cB_at_center=116
   reloc_moves=0  reloc_cells=30752  MEAN_V=0`.
  Reference control in the same tube: every counter zero, `MEAN_V=0`, i.e. the
  source-centre transport (`reloc[]`) is idle even in the reference here.
  **Reading:** the chain is ALIVE end to end -- producer fires, consumer sees it,
  the field reaches the centre, and 30752 cell migrations follow -- yet the net
  displacement is exactly zero.  There are TWO transport machines:
  (A) `relocate()` (interaction.cpp:1542), `draft = north; draft.c[0]--`, which
  slides the whole state pattern through the lattice keeping the address -- this
  is the archived CUDA kernel's actual transport, and `MEAN_V` cannot see it;
  (B) `applyMomentum()` (simulation.cpp:410), which translates a source centre and
  updates `lcenters[w]` from `reloc[]`, fed only by contacts (moveOneStep,
  P×D/P×K via `m`) and pair release.
  The decisive structural finding came from the CUDA (`dev_encounter7`): `c[]` has
  TWO encodings.  The branches at 661-663/674-676/690-692 write a RAW ABSOLUTE
  coordinate; the branch at 695-699 (`!curr.pB && partner.pB`, "parallel
  transport") writes the SIGNED RELATIVE DISPLACEMENT `ELX + (curr.x - partner.x) %
  ELX`.  The consumer tests `c[i] > 0` and decrements, so only the relative
  encoding is homer-DIRECTED; an absolute coordinate makes every cell drift, which
  in a symmetric island balances out -- exactly the 30752 migrations with zero net
  displacement measured above, and exactly the "self-formation/dressing but never
  separates spatially" every island census has reported.
  **The CPU has no producer writing the relative encoding**, so this is the piece
  to port next.
- **2026-09-12 -- WP8: the relative-displacement producer ported; two hard gates
  measured.**  Producer (4) (CUDA `dev_encounter7`, lines 686-700) was added to the
  macro block: for a same-affinity pair with `pB` differing, the out-of-phase cell
  records `L + (own - anchor) % L` (the directional encoding) and sets `cB`.
  Result at `15x9x9`, `N=3`, 16 frames: `homb_events` 7 -> 23 (the new branch
  fires) but `c_at_center`, `cB_at_center` and `reloc_cells` are BIT-FOR-BIT
  unchanged (930 / 116 / 30752) and `body_x = 0`.  The consumer's test is
  `c[i] > 0`, so the migration pattern depends only on WHICH cells have a nonzero
  `c[i]`, not on the magnitudes: the field is already saturated and the
  pre-existing SLOT III/IV relay rewrites `c[]` every tick, so the producer's
  encoding does not survive to `relocate()`.
  **Gate 1 (reproducibility).**  `N=6` gives literally every counter zero --
  including the new branch -- because the reconstruction landed on `pol = (-4,0)`,
  so `pB = sB = false` for every cell and EVERY producer is keyed on pB/sB.  The
  sign convention (`simulation.cpp:365-366`) is therefore not a nicety but a
  precondition: in the dead quadrant the directional sector cannot fire at all.
  **Gate 2 (the field's own dynamics).**  In the live quadrant the field's relay
  rules dominate the producers' writes, so a faithful port of the producer
  encoding changes nothing downstream.  Any real fix must act on the CONSUMER
  side -- `relocate()` / the `c[i] > 0` gate and the SLOT III/IV relay -- or make
  the producer write after the relay, not before it.
- **2026-09-12 -- WP8 (iii) and (i) implemented; first source-level motion.**
  **(iii) `POLAR_MAGNITUDE_FSM`** (`simulation.cpp`: `pB = (pol_u != 0)`,
  `sB = (pol_v != 0)` instead of the sign test): at `15x9x9` the `N=6` run, whose
  reconstruction is `pol = (0,-4)` and which was therefore completely dead under
  the reference convention, jumps from `homb_events=0` to **82**.  The liveness
  lottery is removed.
  **(i) `HOMB_CONSUMER_TRANSPORT`** (`applyMomentum`): decode the arrived field and
  turn it into the single-step displacement `reloc[]` already consumes.
  Three findings while porting it, each forced by a measurement or a guard:
  1. `c[]` carries **two encodings and the `+ L` offset is the tag** -- which is
     *why* the CUDA writes the relative form as `L + (own - partner) % L`:
     `c < L` is the absolute coordinate of the target, `c >= L` encodes
     `target = own - (c - L)`.  Decoding only the absolute form made the block
     dead code (`consumed = 0` with `c_at_center = 930`);
  2. `cB` and `c[]` arrive *separately* at a source centre (`cB_at_center = 116`
     vs `c_at_center = 930`, disjoint -- the flag is relayed by SLOT III/IV while
     the field is propagated by the homing block), so requiring `cB` is wrong;
  3. the harness's own locality assertion (`inertia_fixture.h:95`,
     "every constituent moves at most one cell per light frame") caught the
     consumer twice: once for adding a step on every axis (up to 3 cells/frame)
     and once for acting on every *tick* rather than every *light frame*.  The
     fix latches on `t`, the per-light-frame counter.
  Result at `15x9x9`, `N=3`, 16 frames: `consumed=5`, **`reloc_moves=9`** -- the
  first source-centre motion anywhere in this campaign, reference included (every
  earlier run, and the reference control in the same tube, measured 0) -- with
  `reloc_cells` rising 30752 -> 31909.  `MEAN_V body_x` is still zero over the
  16-frame window, so the coupling moves sources but has not (yet) separated the
  families; the acceptance test is the L=9 census (81 distinct `lcenters`) and a
  longer window, not `MEAN_V`.
  Build variants: `build_rest_shell_probe_homb2.bat` (producers + consumer) and
  `build_rest_shell_probe_homb3.bat` (plus magnitude liveness).
- **2026-09-12 -- author decisions (a) and (b) settled in the manuscript.**
  **(a) The two cone rates.**  The flagged inconsistency -- the no-signaling lemma
  bounded the dependence cone per *housekeeping tick* while $v_{\max}$ is stated per
  *light frame*, the two differing by $N_k$ -- is settled by an explicit **reading
  convention** added to the Setup of Sect.~\ref{subsec:nosignaling-formal}: an
  observer reads the readable fields $\Phi$ at light-frame boundaries, every
  readable field advances at most one cell per light frame, and the fields that
  sweep the whole lattice *inside* a frame (the W-ledger $\Psi$ and the relocation
  field $c$) are hidden coordination fields that no observer reads.  One step is
  therefore one light frame, $\Delta k$ is in light frames, and the cone restricted
  to $\Phi$ coincides with $v_{\max}$.  The former \emph{Note on the two cone rates}
  (which kept the rates distinct and left the gap open) is replaced by a paragraph
  stating the resolution and keeping exactly one open item: the local realisation of
  the simultaneous update, i.e. the host scheduler of remark~(ii).
  **(b) Limitations refilled.**  The empty TODO is replaced by a nine-item list,
  each item stating what is *not* shown: (1) provenance of the numerical results;
  (2) the transport channel is inert in the reference build (measured: no source
  displacement in 16 light frames at 15x9x9); (3) the directional channel is a
  candidate, not a measurement; (4) **GPU/CPU divergence (G0)** -- the CUDA kernel
  is neither built nor validated against the CPU path; (5) the host scheduler is
  not the idealised rule (not a strictly local, integer-only update) and is outside
  the no-signaling scope; (6) small tubes cannot exercise the polarization sector
  ($R=\mathrm{RMAX}-2\ge2$ needs a short side of at least 9); (7) polarization
  liveness depends on the reconstruction sign; (8) negative results and scale;
  (9) taxonomy, bridges and prospects are conjectural.
  The missing `\label{sec:Introduction}` was added (the new item list is the only
  place that cited the Introduction by number; pdflatex reported it as the sole
  undefined reference).  PDF rebuilt clean through doc/latexpdf.bat (55 pages).
- **2026-09-12 -- WP8 census on the production path: the absorbing plateau
  BREAKS.**  Rebuilt the production census (`experiments/island_census.cpp`) with
  the candidate channel via the new
  `experiments/build_island_census_dir.bat` -> `build/island_census_dir/`, and ran
  8 journeys at L=9/W=243 (53.5 s/frame).  Frames 0--6 are bit-identical to the
  documented reference plateau (K=235, D=8, centres=1, groups=235, max_pop=2,
  span=0, captures=escapes=births=deaths=0), i.e. the channel changes nothing
  before the election stamp is live.  The plateau breaks at **journey 7** --
  exactly when `bstamp` becomes live at the source centre (`bstamp=3514`,
  `pol=(4,0)`, `pB` set under the magnitude convention): K 235 -> 86, D 8 -> 157,
  groups 235 -> 86, unresolved 0 -> 125, max_population 2 -> 3, escapes 157,
  births 149; and at journey 8 `distinct_centers` goes **1 -> 2** (the first
  spatial separation ever recorded on this path) with captures=50, escapes=52,
  births=24, deaths=24.  Final histogram: population 1: 62 groups, 2: 18, 3: 6 --
  **six groups at the target population L/3 = 3**, which the reference never
  reaches in 64 journeys -- and all 86 groups are localised (span <= RMAX).
  Scored as a candidate mechanism (C): the channel supplies the deterministic
  asymmetry the SEED_ASYMMETRY negative said was missing, but the bifurcation is
  global (1 -> 2 centres) rather than per-family, so the 81-distinct-centres
  acceptance signature is not reached; the remaining requirement is
  family-selective directionality (the W-address tie-break branch), not time.
  A 20-journey run was launched to check whether the bifurcation continues; the
  result is appended to `experiments/ISLAND_CENSUS.md`.
- **2026-09-12 -- WP8 census, 20 journeys: the 81-centre signature appears.**
  The 20-journey production census with the candidate channel
  (`build/island_census_dir/run20`, 54.5 s/frame, 1090.5 s total, harness
  invariant check passed) fragments the seed monotonically:
  `distinct_centers` = 1 (f2--6), 2 (f8), 15, 28, 44, 62, 62, 73, 78,
  **81 at journey 16** (exactly 9L), then 94, 87, 89, 86.  `max_span` grows 0 -> 10
  cells (the reference kept 0 for 64 journeys), and `pop==L/3_at_end` is 15 against
  0 in the reference; `localized_at_end` is 40 against 8.  The islands are not a
  stable 81-crystal -- the count fluctuates in the 86--94 band and `max_families`
  reaches 3--4, so families mix at later times.  Graded (C): the candidate channel
  supplies the deterministic spatial asymmetry whose absence the SEED_ASYMMETRY
  negative and the 64-journey reference plateau established, and the predicted
  island count appears as a transient.  Remaining problem: stabilise the island
  count and make the directionality family-selective rather than global.
- **2026-09-12 -- WP8 census, 64 journeys (stabilisation test) -- in flight.**
  Launched `build\island_census_dir\island_census_dir.exe 64 16384
  build\island_census_dir\run64` (same frame budget as the reference `run64`, so
  the two are directly comparable; ~54 s/frame, ~58 min).  Frames 0--8 verified
  bit-identical to the 8- and 20-journey directional runs, i.e. the plateau
  (frames 0--6), the break at journey 7 (K 235 -> 86, D 8 -> 157, groups
  235 -> 86, maxpop 2 -> 3, escapes 157, births 149) and the first centre split at
  journey 8 (distinct_centers 1 -> 2) all reproduce exactly; the run then continues
  to 64 journeys to answer the stabilisation question, i.e. whether the number of
  distinct centres settles at 9L = 81 or keeps drifting through the 86--94 band.
  Audit is automated: `experiments/finish_island_census_dir.ps1` waits for the
  process and writes `build/island_census_dir/run64/analysis.txt` (analyzer output
  + the per-frame trend table + the SUMMARY line) as soon as the run exits.
- **2026-09-12 -- WP8 option (2) patch prepared: FAMILY-SELECTIVE directionality.**
  New candidate macros (both OFF in the reference; reference re-verified
  bit-identical: `6468/0/0`, `alpha_A = 0.003756878`):
  **`FAMILY_SELECTIVE_FSM`** -- producer restricted to INTRA-family pairs in the
  relative-displacement encoding (the cross-family tie-break and the
  absolute-coordinate encodings are excluded, since they are what dragged
  different families together), and consumer co-movement: the copies of a family
  share ONE decision per light frame and step together, so a family keeps a single
  centre; and **`FAMILY_RIGID_FSM`** -- a family whose copies are not co-located
  sends every copy to its family chief, so an island re-coheres instead of
  splitting (self-correcting, not a freeze).
  Design and rationale: `experiments/FAMILY_SELECTIVE_DESIGN.md`; build variants
  `build_rest_shell_probe_fam{rigid}.bat` and `build_island_census_fam{rigid}.bat`.
  **Structural finding (forced by measurement):** the only `homB` write the SLOT II
  homing stage can SEE is the per-CELL one (`draft.homB`, the CUDA's
  `dev_encounter4` pattern) -- a write into a source-centre draft is invisible to
  that cell's neighbours until the frame ends and `homB` is cleared every frame.
  With the pair producers alone the homing block reported `homb_seen = 0` while
  `homb_events` was non-zero (`c_at_center = 0`, the channel silently dead);
  restoring a per-family per-cell winner brought it back to `homb_seen = 184`.
  Fast probe (15x9x9, N=3, 16 frames) now gives `homb_events=7, homb_seen=184,
  c_at_center=393, cB_at_center=40, reloc_moves=9, reloc_cells=31909, consumed=5`
  -- the same downstream chain as the non-family build, so the family restriction
  does not disable the channel.  Census runs of both variants were launched
  (20 journeys each) in parallel with the 64-journey baseline, which was at frame
  15 with `centres=78` (matching the 20-journey run's frame 15 exactly).
- **2026-09-12 -- WP8 option (2), follow-up fix: the winner must accept ANY live
  directional bit.**  The per-family per-cell winner was gated on `curr.sB` alone,
  but the reconstruction can return a component that is EXACTLY zero, so under the
  magnitude convention (`sB = pol_v != 0`) an sB-gated rule dies on the
  trajectories where `pol_v == 0`.  Measured: the rigid variant's first probe
  trajectory landed on `pol = (4,0)` and the whole channel went dark
  (`homb_seen = 0`, `c_at_center = 0`, `reloc_moves = 0`) -- the same class of
  failure as the phase-quadrant lottery that `POLAR_MAGNITUDE_FSM` removed.  The
  guard is now `(curr.pB || curr.sB)`.  With the fix the rigid probe is alive:
  `homb_events=7, homb_seen=184, c_at_center=393, cB_at_center=40,
  reloc_cells=31909`, with the family decision applied (`consumed=1`,
  `reloc_moves=1` -- more restrictive than the base build's 5/9, as the rigid rule
  intends).  Reference re-verified after the fix: `6468/0/0`,
  `alpha_A = 0.003756878`.  Both family censuses were relaunched with the rebuilt
  binaries (20 journeys each, in parallel with the 64-journey baseline), and the
  baseline itself reached **`centres=81` at journey 16**, an independent
  reproduction of the 20-journey signature in a longer run.
- **2026-09-12 -- CENSUS RESULT: the family-rigid variant SETTLES on 9L.**
  Four runs of the canonical seed (L=9/W=243, sieve 16384): a 64-journey baseline
  and three 20-journey runs (baseline channel, `FAMILY_SELECTIVE_FSM`, and the
  latter plus `FAMILY_RIGID_FSM`).  `distinct_centers` over journeys 15--20:
  - baseline (non-family): 78, 81, 94, 87, 89, 86 -- amplitude 16, final 86;
  - plain family: 83, 97, 100, 98, 101, 93 -- amplitude 18, final 93;
  - **family + rigid: 78, 80, 82, 79, 79, 79 -- amplitude 4, final 79.**
  Report cards at journey 20: baseline `chiefs=123, localized=40, pop==L/3=15`;
  family `137 / 33 / 11`; rigid `121 / 35 / 9`; the reference produces **0** at the
  predicted population in 64 journeys.
  **The 64-journey baseline answers the stabilisation question for the unpatched
  channel**: it reaches exactly 81 at journey 16, then drifts in the 83--94 band
  through journey 30 (81, 94, 87, 89, 86, 84, 88, 85, 85, 83, 83, 80, 74, 73, 69)
  and thereafter **re-coalesces** -- it neither holds 81 nor stays fragmented.
  Readings: the plain family restriction shatters for the same reason it fragments
  fast (the copies share a step but nothing binds them together, so
  `distinct_centers` exceeds 9L), and the rigid rule -- a non-co-located family
  homing every copy on its chief -- is the stabiliser at 79 +- 2.  Graded (C):
  candidate macros only, and the +-2 jitter is unexplained; the natural next
  question is a stricter anchor (bind the family to its K chief permanently) and a
  longer run to see whether the jitter vanishes.
  Docs updated: `experiments/ISLAND_CENSUS.md` (three-way table + verdict) and
  `experiments/FAMILY_SELECTIVE_DESIGN.md` (design, both structural findings, the
  result, the open question).  Every run's independent audit is written by
  `experiments/finish_island_census_dir.ps1` into that run's `analysis.txt`
  (analyzer output + trend table + SUMMARY), now parameterised by process name.
- **2026-09-12 -- LOCALITY AUDIT of the rigid rule: the emergent hypothesis is
  FALSIFIED, and the result is host-level.**  `FAMILY_RIGID_FSM` reads
  `lcenters[f]` for the family's other layers and uses the comparison as a predicate
  on the transport -- non-local, same tick, with a READABLE consequence -- so it sits
  outside the non-signaling idealisation, like the host scheduler.  A purely local
  probe was added (each copy decodes its step from its OWN `c[]` and its OWN `x[]`,
  no `lcenters`) and measured at 15x9x9 over 16 light frames:
  **N=3: `c0=393, c1=0, c2=0`, of which 114 decode to a non-zero step; N=6:
  `c0=801, c1=0, c2=0`, 280 decodable; in both, ticks with >= 2 copies fielded = 0,
  and steps agreeing = 0.**
  So every field arrival lands in the family's FIRST copy and the other copies
  receive nothing: there is no shared field, the co-movement cannot be emergent, and
  the table predicate is doing all the work.  Structural reasons: (1) the only homB
  write the SLOT II homing stage can see is the per-cell one, and the per-family
  winner is gated on `x[3] % 3 == 0`, so it writes only the first copy; (2) the
  field's propagation sweeps run WITHIN one layer, so a field written in layer 3k can
  never reach 3k+1 or 3k+2 -- the only cross-layer channel in the architecture is the
  encounter itself.
  Consequences: the 79 +- 2 island result currently rests on a host-level non-local
  coupling and must be reported under that caveat or declared as a postulate (P);
  the reformulation to try is to let the encounter write the displacement into EACH
  copy's own layer (using only the two contacting cells' own fields), so every copy
  carries the field locally and rigidity emerges from per-copy fields with no
  `lcenters` read.  The base consumer is unaffected (it reads only the cell's own
  `c[]`, `x[]`, `t`).  Full write-up:
  `experiments/FAMILY_SELECTIVE_DESIGN.md` ("Locality audit of the rigid rule").
- **2026-09-12 -- Step 1 done: the non-local consumption is REMOVED.**  Both
  non-local parts of the consumer in `applyMomentum` were deleted -- the block that
  gathered a family's copies through `lcenters[]` to form one shared decision, and
  the `FAMILY_RIGID_FSM` re-cohesion -- so the consumer is now purely local (the
  cell's own `c[]`, own `x[]`, own `t`), and `FAMILY_RIGID_FSM` is a no-op.
  What survives of `FAMILY_SELECTIVE_FSM` is its ENCOUNTER half (the producer),
  which pairs two cells already in contact and reads only their own `x[3]`.
  Two measured consequences: (a) the withdrawal changed no behaviour of the `fam`
  build (identical counters), because the family-gathering loop sat behind the
  consumer's outer `c[] != 0` guard and therefore never ran for copies 1 and 2 --
  which never carry a field at all; its apparent co-movement was already inert;
  (b) the **differential test passes**: with the predicate gone the `fam` and
  `famrigid` binaries produce byte-identical output, so the settled 79 +- 2 of the
  20-journey census was produced entirely by the table predicate -- `famrigid` is
  retired as a host-level artefact, not a candidate mechanism.
  Reference re-verified untouched: `6468/0/0`, `alpha_A = 0.003756878`.
  Docs updated (`FAMILY_SELECTIVE_DESIGN.md`: macro marked WITHDRAWN + "Action
  taken"; `ISLAND_CENSUS.md`: explicit caveat on the `family + rigid` row).
  Next: the local reformulation -- the encounter writes the displacement into each
  copy's own layer, so every copy carries the field locally and any co-movement is
  emergent, with no `lcenters` read anywhere.
- **2026-09-12 -- WP8 iteration: producer guards, and the phase-quadrant finding.**
  Instrumented the harness per frame: the end-of-run counters for `c`/`homB` are
  *always* zero because those fields are cleared every light frame
  (`interaction.cpp` ~1485/1526), so the harness can only sample post-clear; the
  reliable observables are the `homb_events` counter and the body velocity.
  Guard refinement: the CUDA's `f == t` phase guard (a legacy per-voxel
  cascade) gave 3 events in 16 frames; switching to the manuscript's own
  `t = RMAX/2` guard, and then dropping the phase guard from the two contact
  producers (the CPU encounter is source-level, so the contact condition at
  `interaction.cpp:603` is the relevant guard), raised it to **7 events in 16
  frames** at `15x9x9` with `N=3`: the ported producers do fire.
  **Finding (blocker class):** live polarization is a *phase lottery*.  The flags
  are defined by sign, `pB = (pol_u > 0)`, `sB = (pol_v > 0)`
  (`simulation.cpp:365-366`), while `reconstructPair` returns
  `(R(R-2j), ±2R·isqrt(j(R-j)))` with `j = (bstamp-1) mod 2R^2`.  In the same
  tube and with the same bootstrap, the `N=3` run landed on `pol = (0,4)`
  (`pB` false, `sB` true -> events) and the `N=6` run on `pol = (-4,0)` (both
  false -> zero events): whether the directional sector is alive is decided by
  which quadrant the stamp falls in, not by the physics.  Model change to
  consider (author decision): treat a direction as live by magnitude
  (`|pol| > 0`) instead of by sign, which would make `pB`/`sB` robust.
  Reference re-verified bit-identical (`6468/0/0`, `alpha_A = 0.003756878`);
  new fingerprint `0248ae343bbf3fcc9dc2bbe605b3485012b3296dbf0f9e59c8f5a9e55668fbbc`.
  No motion yet (`MEAN_V = 0`), so the consumer chain (`SLOT II` -> `RELOC`) is
  the next thing to instrument, with three counters: producer fired / the homing
  block saw a `homB` neighbour / `RELOC` applied a step.
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


