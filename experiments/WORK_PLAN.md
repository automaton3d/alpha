# WORK PLAN -- the alpha project: live backlog and record

Status: **active science only**.  Last updated: 19 Sep 2026.

This file has two halves: the **live backlog** (what remains to be done, rewritten 19 Sep 2026) and the
**dated progress log** (the record, oldest entry first, kept as written).  A progress-log entry may refer to
WP numbers, documents or harnesses that have since been retired; the plan they belonged to is summarised in
the backlog, and the retired material itself is in `attic/` (manifest in the last table of the backlog).

This file used to carry a WP0-WP7 plan aimed at a manuscript submission.  That plan is retired: it was
executed, withdrawn, or superseded, and the paper-facing documents it produced were retired with it -- the
manuscript blueprint and the campaign record into `attic/` on 19 Sep 2026, and the paper-facing ("claim")
documents, the referee-response letter, the coupling-constant campaign, the EM/photon/orphan and gravity
lines, the colour/election/cohesion candidates and the falsified size-selection candidates into
`attic/alpha_campaign/`, `attic/referee_response/`, `attic/submission_docs/` and
`attic/closed_programmes/`.

---


## Live backlog (rewritten 2026-09-19)

The plan this file used to carry (WP0-WP7: freeze the reference, re-validate every claim, close the evidence
gaps, run the photon/EM spike, restructure the paper, simulated referees, submission) is **retired** --
executed, withdrawn, or parked in `attic/`.  What remains is the science.

### A. Island aggregation and quantization (the primary programme)

1. **Does the plateau track the length scale, or the condensate?**  The shell brake
   (`PARENT_SHELL_RELEASE_FSM`) holds `max_pop` flat in the cubic census (`33 -> 24 -> 18 -> 26, 26, 26` at
   `L=9`) while the same rule set without it freezes at 33 with zero events.  Registered before running: the
   **`L=15` arm** (`island_census_parent_shell.exe 3 16384 build\island_census_parent\shell_L15 15`,
   ~31 min/frame) decides it -- a mode tracking `W/8` (~84) is condensation, one tracking `2*RMAX` (14) is
   extent.  **Status of the run:** the first attempt (19 Sep, ~23:43-00:08) was **killed mid-frame-1** by the
   environment, so it produced no result; relaunched the same night.  The one frame edge it did reach is
   consistent with `L=9`: `[shell] range=14 max_dist=0 releases=0`, i.e. the canonical superposed seed is
   co-located at `L=15` too (all delegates on their chief), which is why the brake starts inert there.

2. ~~**Why exactly one island per charge word, and not zero?**~~  **ANSWERED 20 Sep 2026 --
   `PAIR_MECHANISM.md`.**  Instrumented read-only (`CASCADE_LOG` + `cascade_probe`; reference build
   byte-identical with the macro off): the whole cascade happens inside **one** light frame -- zero role
   changes in frame 1, then 12402 changes in frame 2 between tick **879** (the first tick after the frame
   edge) and tick **1120** (27% of the frame), after which nothing more changes.  The pair-making encounter
   is a `D->K` **promotion**, and its partner is the charge word's **maximum address** (242, 230, 221, 233,
   224, 236, 227, 239 = the top of each word's last family).  Mechanism: `promotesDelegate` requires
   `main.w < mirror.w`, so the sweep runs upward in address and cannot promote the word's highest-address
   delegate -- one survivor is *forced*, and zero is impossible because no transition demotes a delegate to
   `S`.  Chiefs come from promotion, not election (`D->K = 6084` vs `S->K = 26`).  **Sub-question answered
   20 Sep 2026 -- the cascade duration tracks `W`:** rerun at `L=12` (`W=432`, `FRAME=1504`) the cascade
   runs ticks **1505 -> 1935** = **T = 431 = W-1**, against T = 242 = W-1 at `L=9` -- one partner-rotation
   period (`T/W` = 0.996 and 0.997), independent of lattice volume and of the seed's family multiplicity.
   Two further facts from the same run: the `S x S` election channel falls to **zero** at `L=12` (26 at
   `L=9`), so the structure is built by promotion; and frame 1 again has zero role changes.  The `L=15`
   confirmation (`T = 674` predicted) is launched and running; read it with
   `build\cascade_probe\cascade_probe.exe 2 16384 15` -> `[cascade]` lines.  Still open: why `W-1` and not
   `W`, and whether the law survives once islands have extent.
3. **A size-dependent loss for a CO-LOCATED island.**  Both geometric brakes are closed by measurement:
   displacement is forbidden by the one-cell step invariant (`inertia_fixture.h:95`), and the shell/extent
   brake cannot see an island whose members share a cell; the `|J|` ceiling is inert for the same reason
   (`max_Jmag2 = 0` at every census frame).  The only quantity that grows with `N` inside one cell is the
   member COUNT, so the next candidate must read it -- and a count threshold is one step from a DECLARED knob
   (the `L/3` lesson): **register the prediction before running it**, and check that the selected population
   does not track the seed multiplicity.
4. **Turnover and stationarity in one configuration.**  The flux harness has never received a dataset with
   both.  The driven fixture proved the structural tension: release needs isolation, recruitment needs
   contact, and a released singleton has `m = 0`, so a CARRIER is required.  Build the fixture with all four
   ingredients (two or more islands beyond contact, free `S` in reach, small `W`, enough frames) and run
   `quantization/quantize_stdlib.py` on it.
5. **Does an island acquire a driver on its own?**  If a rule-built island starts transporting, the release
   -> recruit cycle closes without a planted carrier.  Registered falsifier: if the separation spreads the
   sizes, the uniformity was a property of the frozen plateau.

### B. Inertia and transport (the prerequisite programme)

6. **Transport is the shared bottleneck** of A4, A5 and the winding line: the reference seed does not move
   (`J = 0` in ~3200 groups; the census freezes at one centre).  The measured transport instrument is the
   drive pair (0.25 cells per light frame); the prepared-island fixtures are where a body actually moves.
7. **The winding triple `(n1,n2,n3)`** is instrumented read-only (`winding_observatory.cpp`, B0 in
   `WINDING_B0.md`): the accumulator is reliable, the response is diagonal in the axis, covariant under sign
   and symmetric under axis permutation.  What it lacks is a *generator* (nothing in the reference
   transports) and a decision on what the triple is supposed to mean (see C).
8. **Definition first.**  In this project "quantization" means the spontaneous formation of islands with the
   same charge and approximately equal size -- geometric, structural, emergent -- not a magnitude, not a
   conserved number, not an identity.  The three-loop basis (three independent integers per object, an exact
   measure) serves that only if the dynamics *selects* or *couples* the triple: state which, and register the
   falsifier, before building on it.

### C. The registered candidate for size selection

9. **A `|J|` ceiling with the cap taken from the cavity radius** (`J_max(RMAX)`, never from `L/3`),
   predicting `N* ~ J_max / (r |m|)` and falsifiable by sweeping `RMAX` against the seed multiplicity.  This
   is the only class of rule known that could select a size without reading the declared partition.

### D. Discipline (non-negotiable, learned the hard way)

- **Pre-register the prediction before the run.**  It caught a wrong bet on 19 Sep 2026 and produced the
  plateau finding anyway.
- **Cite code by NAME, not by line**: numbers rot within a session.
  `experiments\anchor_semantic_check.ps1` (28 checks) proves that the cited line still says what the note
  claims; `attic\anchor_audit3.ps1` only proves that it exists.  Run the semantic checker after any edit that
  adds lines to the model.
- **The fingerprint hashes source TEXT**: a comment-only edit moves it, so the expected value is re-measured
  with the edit, and behaviour equivalence is shown separately by preprocessing with every candidate macro
  off.  `experiments\model_fingerprint.ps1` -> `MATCH`; the macro inventory is checked by
  `attic\inventory_check.ps1` (49 model macros, 4 harness-only).
- **One channel per measurement**: count the mechanism, not the aggregate (`[shell]`, `[absorb]`,
  `[nomination]`, the MECHANISM line of `island_rate_probe`).  Three findings in this session came from a
  diagnostic line contradicting an aggregate.

### E. Known defects

- `doc/manuscript.bib` has no `hofer` key, so the build warns `\missing{hofer}` (page 38); the reference text
  is inline in the `.tex`.  Pre-existing, unrelated to the science.
- The manuscript compiles clean at 57 pages; the campaign's only trace (one Status clause) was removed on
  19 Sep 2026.

### Retired, and where it went

| folder | what |
|---|---|
| `attic/alpha_campaign/` | the coupling-constant campaign (probe, tables, drivers, raw logs) |
| `attic/referee_response/` | the referee-response letter and the manuscript/revision blueprint |
| `attic/submission_docs/` | the paper-facing ("claim") documents |
| `attic/closed_programmes/` | the EM/photon/orphan and gravity lines, the colour/election/cohesion candidates, the falsified size-selection candidates and the paper lint |
| `attic/retired_experiments/` | harnesses retired in the earlier pass |

Everything else in the tree is the essential set: the model and its build, `doc/`, `quantization/`, the notes
and instruments of programmes A and B, the test suite and the two repo checkers.

---
## Progress log (dated, oldest entry first -- kept as written)

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
- **2026-09-13 -- WP3.3 full-scale confirmation DONE.**  New build
  `experiments/build_island_census_exc_fix.bat` (`EXCLUSION_FSM` +
  `DD_INTRA_ISLAND_FIX`, ordinary production `simulation()`), run
  `build/island_census/island_census_exc_fix.exe 12 16384 build/island_census/prod_exc_fix`
  on the canonical superposed Platonic seed (L=9, W=243, no placed seed): frame 2
  = **K=81 / D=162**, groups 81, unresolved 0, mixed charge 0,
  `max_population = 3 = L/3`, `max_families = 1` -- i.e. exactly one chief per
  seed family (1 K + 2 D), against the reference path (235 K / 8 D, one centre,
  no group at L/3) and the EXCLUSION-only port (162 K / 81 D).  Frames 2-12 are
  identical in the census (no captures/escapes/births/deaths from frame 3);
  occupied centres fluctuate 9, 15, 19, 17 (weak localisation: several islands
  share a centre).
  The full 12-frame run completed in `build/island_census/prod_exc_fix/` with the
  report card `chiefs_total=81 chiefs_at_end=81 stable>=5frames=81
  localized_at_end=81 pop==L/3_at_end=81` (wall 688.49 s, 57.4 s/frame).
  Recorded in `ABLATION_81x3.md` (WP3.3) and
  `LIGHTMATTER_DECOUPLE.md` (production-path port).  Residuals unchanged: weak
  spatial localisation and the candidate status of the identity hard core
  (`canElectChief` semantics).
- **2026-09-13 -- Item A (circularity) probed: the sieve channel does NOT
  quantise.**  With the ORDINARY reference census build (no candidate macro) the
  only free knob that can rearrange the role census is the electroweak sieve
  modulus `S`.  Sweep at L=9, W=243, canonical superposed seed, 6 journeys each
  (`build/island_census/sweep_S*`, read by the new
  `experiments/analyze_sieve_sweep.py`; log `experiments/SIEVE_SWEEP_PRODUCTION.md`):
  `S=16384` and `8192` reproduce the reference plateau `235 K / 8 D` (max
  population 2, one centre); every `S <= 4096` switches to `145 K / 5 D` with
  `P = 93` pair halves (the electroweak channel opens; `145+5+93 = 243`), still
  max population 2 and one centre.  No group reaches `L/3 = 3`, in either
  regime, and `captures = escapes = births = deaths = 0` from frame 3 on: the
  census is absorbing at population 2.  So the sieve channel rearranges roles
  and binds pairs but does not produce `N* = L/3`; the `81 x 3` quantisation
  comes from the candidate rules that carry `ISLAND_SIZE = L/3`.  Window covered:
  frames 2-10, including journey 7 where the broadcast stamp goes live -- the
  12-journey `S = 64` run (`build/island_census/sweep_S64_long`) does **not**
  break at frame 7 either (frames 2-10 are the identical low-S row).  `L` is
  hardcoded to 9 in the harness,
  so the `L/3`-vs-`L` dependence test remains out of reach.

- **2026-09-13 -- Item C: the distinct-family ("far") mediated-duo probe is
  NEGATIVE.**  New combined build `experiments/build_probe_em_orphan.bat`
  (`EM_FIRST_FSM` + `ORPHAN_GUIDANCE_FSM` + the polarization bootstrap macros).
  Layout `far` (W=9: bodies w=0/w=3 in different families, free mediator at
  w=6/7), EL=11, SEP=4, 20 frames, same charge 0x08, mediator bare/photon/grav
  (`experiments/PHOTON_MEDIATION_FAR.md`, logs under `build/probe_far/`): with
  the orphan channel alone (identity first) all three merge at frame 4; with the
  EM reorder live, the R2 channel still does nothing at the reference sieve
  (d -> 0 at frame 5) and, with the gate open (`S = 64`), the photon and
  graviton trajectories are **bit-identical** (same d(t), 506 recruits, 0 repel,
  32 attract, 26 annihilations) -- the separation that appears is the opened
  electroweak branch, not a charge-word-selective mediator effect.  The earlier
  W=4 "repel" positives were the identity/cohesion background being partially
  counteracted.  So no reference-dynamics mechanism separates two equal-charge
  islands; the separation in the `81 x 3` candidates comes from the
  `EXCLUSION_FSM` stand-in.  Residuals: one lattice size, one SEP, mediator
  planted by the probe, `pol` relies on the candidate bootstrap.
- **2026-09-13 -- Item D: the membership fixed point is DERIVED (the quantum is
  2, not `L/3`).**  New note `experiments/DYNAMIC_QUANTIZATION_DERIVATION.md`.
  From the coded transitions (`chief_transition.h:14-87`, the whole block from `promotesDelegate`
  to the end of `chiefContact`; call site `interaction.cpp:1592`, contact gate `:942`)
  the frozen membership states satisfy: every
  group has population <= 2 and, per charge word, at most one group of
  population 2.  Proof: a group of size >= 3 contains two same-charge delegates;
  the schedule rotates one slice per frame (`utils.cpp:36`) so every pair meets
  within `W` frames, and `promotesDelegate` (which requires only
  `main.w < mirror.w`) then promotes one -> the size drops; two same-charge
  size-2 groups are likewise split.  None of the transitions reads
  `ISLAND_SIZE`/`L/3`, and the bound 2 holds for every multiplicity `m = L/3 >= 2`
  -- so `N* = 2` for every `L` and every `S`.  Checks: reference `235 K + 8 D`
  with **exactly 8** non-singleton groups (= the 8 distinct charge words);
  low-`S` `145 K + 5 D` (5 <= 8); `prod_exc_fix` `81 x 3` only because
  `DD_INTRA_ISLAND_FIX` removes the promotion inside a family.  Also explains why
  the manuscript's `Gamma_cap(N*) = Gamma_esc(N*)` reading misleads: both rates
  are 0 in the absorbing corner, so `N*` comes from the reachable set, not from
  a balance.  Decisive non-circular falsifier left: run the ordinary build at
  `EL = 15` (`W = 675`) and check `max_population = 2` / `pop == L/3 = 0`.
- **2026-09-13/14 -- Steps 1+2 done: derivation folded into the manuscript; the
  `L=15` falsifier launched.**  (1) `doc/manuscript.tex` gained
  `\subsection{The group population is fixed by the transitions}`
  (`subsec:fixed-point`): the T1-T5 transition list, the fixed-point statement
  with its proof sketch, the corner-not-balance remark, `tab:ledger`
  (235/8/0, 145/5/93, 81/162 rows), the status paragraph (the charge unit `L/3`
  is a topological axiom, on the footing of the SM hypercharge normalisation;
  the candidate rules work by switching (T2) off inside a family) and the scope
  paragraph (co-location; the prepared islands of the inertia runs).  The
  abstract, the `L/3` hypothesis sentence in Sect. Dynamic charge quantization
  and the Limitations list were updated to match, and
  `experiments/POSTULATE_VS_EMERGENT.md` gained two rows: island population
  quantum `N* = 2` -- **M** (derived), charge unit `L/3` -- **P** (axiom).
  Build `pdflatex -> biber -> pdflatex x2`: 56 pages, **no undefined
  references** (8 overfull hboxes, as before).  (2)
  `experiments/island_census.cpp` now accepts an optional fifth argument `[EL]`
  (default 9, must be a multiple of 3), so the census can run at other lattice
  sides; the change is backwards compatible (check run `el9_check` at `EL=9`:
  frames 0-1 identical, 64.2 s/frame, report card written).  Launched the
  falsifier `island_census.exe 4 16384 build/island_census/L15 15`
  (~33x the `L=9` cost per frame, ~30 min/frame; frame 0 done, `S=675`).
  Prediction to check on the frame-2 row: `max_population = 2` and

- **2026-09-14 -- The `L=15` falsifier ran and the theorem HOLDS.**  Completed
  `build/island_census/L15` (`island_census.exe 4 16384 build/island_census/L15 15`;
  `EL=15`, `W=675`, five copies per family, `RMAX=7`, `FRAME=2285`, 7460 s,
  31 min/frame).  Frozen state from frame 2: **667 K + 8 D**, groups 667,
  unresolved 0, mixed charge 0, `max_population = 2`, one centre; report card
  `chiefs_total=667 chiefs_at_end=667 localized_at_end=8 pop==L/3_at_end=0`;
  frames 3 and 4 identical (zero captures/escapes/births/deaths).  So the
  population quantum is **2 at multiplicity 3 and at multiplicity 5**: `L/3` is
  not an attractor at either size, and `P1` is confirmed at a second lattice
  side.  The sharper reading: `K = W - 8` and `D = 8` at both sizes, i.e.
  exactly one delegate per charge word -- the theorem's bound is attained.
  Recorded in `DYNAMIC_QUANTIZATION_DERIVATION.md` (new section 9, plus the
  ledger row and the `P1` status), and the manuscript's fixed-point subsection
  and Limitations item now report the check.
- **2026-09-14 -- Item 1 done: spacing is not emergent (threshold measured).**
  New `experiments/spacing_probe.cpp` (+ `build_spacing_probe.bat`, three arms) and
  log `experiments/SPACING_PROBE.md`.  A three-copy family (`K`, `D1`, `D2`) in a
  `27x5x5` tube (`RMAX=2`, contact range 4), varying only the `D1-D2` gap `dD`:
  **every `dD` from 0 to 4 promotes** (`T2` fires, final `2K+1D`), while
  `dD = 5` keeps `1K+2D` for 40 journeys -- so the threshold is
  `d >= 2*RMAX + 1`.  The electric channel was given its prerequisite
  (`POLAR_BOOTSTRAP_ADDRESS` + `POLAR_BROADCAST_WAVE` + `SPACING_POL`) and the
  ordering fix (`EM_FIRST_FSM`): identical outcomes, and the `conv_repel` counter
  never fires.  With `PHOTON_MEDIATION_FAR.md` (mediated channel sign-blind) and
  `GEOMETRIC_QUANTUM.md` (complete contact graph in a cube) this closes every
  space-providing candidate; the candidates that *do* hold copies apart
  (`ADDRESS_TARGET_FSM`, `ISLAND_ALIGNED_W_ROTATION`) derive their destinations
  from the family index `w / ISLAND_SIZE`, i.e. from the `L/3` partition.  Design
  criterion recorded for any future attempt: hold the copies at
  `d >= 2*RMAX + 1` with cell-local data only, no `ISLAND_SIZE`, no `L`.
  Folded into the manuscript's candidate-mechanism paragraph.
- **2026-09-15 -- Emergence search opened: a LOCAL rule holds the family multiplicity
  (`PHASE_DISTINCT_FSM`), and the seed-side route is closed.**  New note
  `experiments/EMERGENCE_SEARCH.md`, macro `PHASE_DISTINCT_FSM` in `chief_transition.h`
  (OFF by default), new `p7_concentration` modes (`coPhase`, `five`, `fiveCo`, variable
  multiplicity and per-layer breathing phase) and a fourth binary.  Rule: identity is the
  pair (charge word, breathing phase), i.e. the reference's refusal of different-word
  merges is extended to indistinguishable same-word same-phase bubbles; it reads only
  `ch` and `t`.  Measured in a tube: at `m = 3` the reference splits in phase
  (`2K+1D`, one promotion) while the rule holds `1K+2D` with promotions `1 -> 0` and the
  contact count unchanged; at `m = 5` the rule holds `1K+4D` (`max_pop 5`) against the
  reference's `4K+1D` cascade, so `N* = m` at both multiplicities.  With distinct phases
  the rule is inert (identical to the reference), so its scope is measurable.  The
  *seed-side* alternative was tested and closed: a phase-spread seed still promotes twice
  at `m = 5` and lands at `3`, so the three-copy tube result was multiplicity-specific.
  The obstruction is stated first: no local quantity equals `L/3`
  (`L/3 = (2L/(3(L-1))) RMAX`), so a fixed local rule can only be asymptotically
  proportional -- what is available dynamically is a scaling with `L`, not the value;
  the value stays the seed's multiplicity, i.e. the topological axiom.  On the locality
  criterion the new rule is strictly better than `DD_INTRA_ISLAND_FIX`.  **Census run (item 1 of
  the follow-up list) done, and it corrected two of my own claims.**  First, the registered
  prediction was wrong *before* the run: the tube arms start from a *planted* `1K+2D`, while the
  census must *form* the group, and the rule refuses the `S x S` election too.  Corrected
  prediction, registered pre-run: no group at all (`K = D = 0`, `max_pop 1`).  Measured at
  `EL = 6` (cheap: 3.1 s/frame): frames 0-4 exactly that (`K = D = 0`, `S = 108`/`104`), reference
  at the same size `100 K + 8 D`, `max_pop 2` from frame 2; at frames 5-6 the sector is released
  (`26 K 2 D`) because pair releases reset the breathing clocks (`reemitAtContact`) and destroy the
  phase degeneracy the refusal relies on -- so the route is **closed with a mechanism**, not with a
  null.  At `EL = 9` the rule reaches a *stable* `S = 150, P = 93` (frames 3 and 4, `K = D = 0`).
  Second, that split **is the ledger row's split** (`150` in roles + `93` halves = 243), and the
  93 halves are therefore a real dynamical count: the pair branch is reachable for same-word
  sources once the election stops consuming them (`4 P` then `12 P` at `EL = 6` against zero in the
  reference), which supersedes item 3's reading that the row was suspicious and qualifies its
  falsifiable statement as *reference-sieve*.  Reproduction under the row's own conditions
  (reference build, `S = 64`, `EL = 9`, log `build/census_ref_el9_s64.txt`) was launched.  One more
  of my registered predictions was mis-specified and is recorded as such rather than amended (in
  the in-phase tube arm I predicted no group at all; the harness plants `1K+2D` and the rule cannot
  dissolve an existing group).  Folded into the manuscript: a paragraph "A local variant of the
  family candidate (M)" in the fixed-point subsection, plus the sieve-qualified reachability
  caveat in the charge-census appendix.  **The reproduction landed exactly**: reference build,
  `S = 64`, `EL = 9`, frame 2 gives `K = 145, D = 5, P = 93`, `145` groups, `max_pop 2`,
  `captures = 150`, `births = 145` --- every entry of the ledger row, so the row is a
  reproduction result and the suspicion is withdrawn; the log is archived in the repository
  (`experiments/census_ref_el9_s64/census.csv`) and closes lint action 2.

  runs, and the algebra's "R3/R6 can fire" was wrong.**  New harness
  `experiments/pair_channel_probe.cpp` (+ `build_pair_channel_probe.bat`) and the
  `PAIR_WORD_LOG` macro in `interaction.cpp`, which prints the words of every pair
  formation.  The probe plants co-located, in-phase `S` sources with chosen words in a
  `15x5x5` tube, presets the sieve bit and sets `s2b_target = 1` so the electroweak gate
  is open (as `island_census` does), and reports every formation.  Result over ten
  modes: the *different-word* rules fire exactly as predicted (`0x00/0x3F` R1,
  `0x00/0x1F` and `0x2A/0x35` R2), while the *identical-word* rules never fire
  (`0x00/0x00`, `0x2A/0x2A`, `0x2E/0x2E`, `0x3F/0x3F`, `0x11/0x11` -- all zero, and the
  `0x00/0x00` case stays zero after 40 frames, so it is not a timing effect).  Cause,
  located in the code: `chiefContact` (T1) consumes two identical-word `S` sources
  inside the same call, and the following `internal` test then returns **before** the
  pair branch, so the membership election shadows every identical-word rule.  R5 and R6
  are identical-word by construction, hence dead in two independent ways (R5 also
  violates the manifold invariant).  Consequence: the dynamic pair channel is exactly
  `{R1, R2}`, both needing words the canonical seed never generates, so **no pair can
  form in a canonical run** -- which is why the 16-frame census read `pairM = pairA = 0`
  at every sampled tick and why the electric channel never acted.  The `seedset` mode
  (the seed's own words plus repeats) forms nothing in 40 frames.  This also puts the
  ledger row `S = 64: 145 K + 5 D + 93 P halves` under suspicion: 93 halves are
  impossible under this branch order, so the row needs a log or a re-derivation.
  `CHARGE_SPECTRUM.md` gained section 5b with the table, the mechanism and the
  corrected falsifiable statements; the earlier "canonically only R3 and R6" reading is
  marked as superseded.

  New `experiments/check_claims.py` (stdlib), note `experiments/CLAIMS_LINT.md`, and the
  `check-claims` target in the `Makefile`.  The scanner extracts number-like tokens from
  every non-bibliography sentence and links them to files under `experiments/`,
  `quantization/` and `build/`, in three buckets: STRONG (number plus at least two
  shared content words), WEAK (number only) and NONE (no artefact carries it).
  Manuscript backups under `build/` and the tool's own output are excluded, otherwise
  the match is circular.  Result: 66 sentences carry a claim-like number -- 52 STRONG,
  2 WEAK, **12 NONE**.  Four classes of NONE matter: the whole wavefront-metric table
  `tab:scaling` (manuscript L410) and the `0.9950` envelope correlation (L1167) have no
  reproducing artefact; Appendix A's derived constants (`n_p`, `n_pT`) are arithmetic
  that no script recomputes; the rest are bit-string notation (false positives of the
  lexical method, documented as such).  The semantic checks a lexical tool cannot make
  are listed separately, and they are the ones with teeth: the `tab:scaling` caption
  names a producer (`lorentz_mm.cpp`) that the deposit calls "(historical)" but that
  **exists nowhere in the repository**; the 93 pair-halves of the reference ledger have
  no archived log (a fresh 16-frame canonical run reached tick 1025 with
  `pairM = pairA = 0`); the `1187` proton--electron ratio is not recomputable from the
  table it sits in; the taxonomy's example words `0x00/0x1F` and `0x00/0x3F` are planted
  (now covered by the Appendix B caveat); and the flux-harness figure `N* = 30.38` must
  not be presented as model output, since the same document reports no attractor in the
  repository's data.  Six actions listed with costs in the note.

  cause diagnosed).**  New macro `SURFACE_ESCAPE_FSM` in `src/model/interaction.cpp`
  (OFF by default, so the reference build is bit-identical: every change sits inside
  the `#ifdef`), new harness modes in `experiments/p7_concentration.cpp`
  (`chain`, `gap2`, `mid`, `edge` beside `co`/`spread`, each printing the prediction
  itself) and a third binary in `experiments/build_p7_concentration.bat`; log
  `experiments/PREDICTED_NS_RULE.md`.  Folded into the manuscript: a paragraph
  "A constructed balance rule selects N* = 1 or 2, not L/3 (M)" at the end of the
  fixed-point subsection (Sect. 8.2, which now carries the label
  `subsec:dynamic-charge-quantization` for the rate-balance reference) and a new
  Limitations bullet "The balance route is closed by measurement, not only by
  degeneracy (M)", with the neighbouring bullet about the quantum being 2 amended to
  say that the natural candidate of that class has been constructed and measured
  without success.  The membership ledger's open-gate row also gained a provenance
  caveat for its 93 pair halves (item 3's measurement), so the table is not left
  unqualified.  The rule is exactly what section 6 of
  `DYNAMIC_QUANTIZATION_DERIVATION.md` asked for: **capture by shell overlap**
  (unchanged reference gate) versus **escape by surface** (a delegate with no
  same-charge contact for one full partner rotation, `W_USED` frames, is released) --
  both local, no `ISLAND_SIZE`, no `L`.  Pre-registered prediction: `N* = 2` for
  constituent separation `gap <= 2*RMAX = 4`, `N* = 1` above.  Measured over the six
  gaps `0,1,2,3,4,5`: the escape boundary sits between `gap = 2` and `gap = 3`, so the
  prediction is **falsified at `gap = 3` and `4`** (escapes fire: `2K+0D+1S`,
  `max_pop = 1`) and confirmed at `0-2` and `5`.  Diagnosis from the same table: the
  reference still contacts at `gap = 3,4` (it promotes), so the shells do overlap --
  but *intermittently*: the measured contact duty cycle (fraction of the `2*RMAX = 4`
  frame breathing period in which a pair is simultaneously open) is `3/4` for
  `gap <= 2` and `1/4` for `gap = 3,4`, and the timer (3 consecutive contact-free
  frames) turns exactly that drop into the boundary.  The rule therefore measures
  contact *persistence*, not overlap *existence*.  Consequence: with the rule on, the stable
  population is 2 at best and 1 beyond ~2 cells, so `N* = L/3 = 3` appears nowhere --
  the balance route is now closed *with* an interior crossing (both rates nonzero),
  which is a stronger statement than the earlier absorbing-corner argument.  By-product
  for the contact channel: the reference contact range `2*RMAX` is now confirmed from
  both sides (`gap = 4` promotes, `gap = 5` does not).

- **2026-09-14 -- Item 3 done: the charge spectrum the rules allow is derived
  (and the paper's census arithmetic verified).**  New
  `experiments/analyze_charge_spectrum.py` (stdlib only) + log
  `experiments/CHARGE_SPECTRUM.md`.  From the code (`initSim.cpp:76-80`) the
  seed realises the diagonal `c0 = w0`, `c1 = w1` of the 32-word manifold
  `M = {ch : q ^ w0 = w1}` -- exactly 8 of 64 words, which the generator
  explains rather than assumes.  Since the *only* writers of `cell.ch` are the
  seed and the gated conjugation hook (`simulation.cpp:873`, `ch ^= 0x1F`,
  `mm_eps = 0` by default), every reachable word lies in `M`, and an exhaustive
  enumeration against the real `canFormPair` shows **R1, R4 and R5 have zero
  admissible pairs: three of the six channels are algebraically closed**; R2 is
  exactly `{w, w ^ 0x1F}` (16 pairs, requires the conjugates the seed never
  makes), R3 is `0x00/0x00`, R6 is six words.  So in a canonical run only R3 and
  R6 can fire and **the photon channel is dormant for lack of charge geometry** --
  the charge-side explanation of the item-1/`PHOTON_MEDIATION_FAR`/item-C
  negatives.  The seed's matter/antimatter inventory (4 matter words vs 3
  antimatter + 1 anti-neutral, `81 = 8*10+1` putting the extra family on
  `0x00`) predicts `freeM = 89667`, `freeA = 87480`, `D = +2187` at L=9, which a
  live canonical run reproduces to the cell.  Appendix B's table was
  independently recomputed: 13 rows sum to 196,587 as printed, `W+Z` 30,714 vs
  30,710, charged 83:3.  The spectrum itself stays integral in fragment units --
  thirds still need the `L/3` axiom -- so this is a derived statement about the
  charge *pattern*, not a road to quantisation.  Started, not finished: the
  census harness now takes an optional `argv[5] = mm_eps` (default unchanged) and
  two arms (`build/mm0.txt`, `build/mm1.txt`) were launched; the hook is entered
  but its turnaround latch is not yet armed (`draft_t = 1` at tick 1024,
  `RMAX = 4`), and no pair had formed by tick 1025 in either arm.

- **2026-09-14 -- Item 2 followed through with a MOVING body (continuing
  exchange).**  Extended `experiments/turnover_ablation.cpp` with `move_co` /
  `move_far`: body = one `K` + one drive pair (`0x00`/`0x1F`, `m[0]=+RMAX`, bound
  to layer 0, as `inertia_fixture::prepare`), reservoir ahead at `x=8,14,20`, 120
  journeys; controls = same geometry with different charge words, and the same
  configuration under `EXCLUSION_FSM`.  **Results:** `ref/move_co` -> 10 captures
  / 7 escapes (continuing exchange; a lone `K`+pair oscillates and the first
  capture makes the transport directional via the `P x D` contact);
  `ref/move_far` -> **0/0** with contacts still counted (the exchange is
  charge-word selective, as `chiefContact` requires); `excl/move_co` -> 6/3 but
  **per-chief attribution shows the body never gains a member** (the turnover is
  internal to the reservoir family) -- the hard core *isolates*.  **Flux harness
  on the viable dataset:** `mv_ref_co` gives `b = +0.030` with the binned drift
  rising with population (`N=2: -0.02`, `N=3: +0.11`) -> **FAIL: no restoring
  drift**, so even with continuing turnover the population dynamics is
  anti-restorative.  Log updated (`TURNOVER_ABLATION.md`) and the manuscript's
  candidate-mechanism paragraph now carries both the static and the moving
  measurements.
- **2026-09-14 -- Item 2 done: the turnover ablation is measured (was vacuous).**
  New harness `experiments/turnover_ablation.cpp` (+
  `build_turnover_ablation.bat`, reference and `EXCLUSION_FSM` binaries) and log
  `experiments/TURNOVER_ABLATION.md`.  Design: tube `27x5x5` (`RMAX=2`, contact
  range 4), body `K+2D` of one family at `x=3,10,17` (pairwise > 4), reservoir in
  another family at `x=6,13,20` (mode `co`) or `x=24` (mode `far`, out of reach),
  40 journeys, `constituents.csv` written in the census format so
  `quantization/flux_from_census.py` consumes it.  **Results, matching the
  pre-registered expectations:** `ref`+`co` -> 5 captures / 2 escapes (turnover);
  `ref`+`far` -> 0/0; `excl`+`co` -> **0/0**.  So the identity hard core
  *isolates* an island from its environment instead of selecting a population,
  and the exchange is a one-off relaxation (everything in frame 3, then 38 frozen
  frames; flux-harness verdict INCONCLUSIVE with surrogate `p = 1.00`).  The same
  run freezes at `K=3 D=3 groups=3 max_population=4` -- a frozen group larger than
  2, because the constituents are spread beyond the contact range: the theorem's
  scope clause measured, and the frozen size set by reach (three reservoir
  singletons), not by a quantum.  Folded into the manuscript's candidate-mechanism
  subsection.
- **2026-09-14 -- `quantization/` analysed and wired to the simulator (the flux
  harness for `Gamma_cap = Gamma_esc`).**  The author's harness (`quantize.py`)
  tests the manuscript's attractor hypothesis from per-event fluxes instead of
  population levels; it needed numpy/pandas (absent here) and an adapter.  Added
  in that directory: `flux_from_census.py` (`constituents.csv` ->
  `run,island,frame,N,captures,escapes`, exact fluxes, `--derive-fluxes` never
  needed on real data), `quantize_stdlib.py` (stdlib port of the five sections
  plus `--by-run` for `N*` vs `L`) and `make_synth_stdlib.py` (stdlib twins of the
  synthetic controls).  **Validated**: the synthetic attractor is recovered
  (`N* = 30.38`, CI [29.87, 30.91], surrogate p = 0.005 -> PASS) and the fixed
  partition returns INCONCLUSIVE.  **Six real datasets**: four are *frozen* in the
  harness's own sense (zero escapes -- reference at `L=9` and `L=15`, `EXCLUSION`,
  `ADDRESS_TARGET`) -> INCONCLUSIVE, i.e. the membership theorem's corner reached
  by an independent instrument; the two directional-channel runs, which *do* have
  turnover, show an **anti-restorative drift** (`b = +0.494` at 20 frames and
  `+0.495` at 64 frames; the binned curve rises with population) -> FAIL, with no
  `N*` anywhere.  So no population attractor exists in the repository's data, and
  the attractor form of the quantization claim is falsified where it could be
  measured.  Details: `quantization/FINDINGS.md` (table, commands, and what would
  make the harness decision-grade).
- **2026-09-14 -- Conditional revision of `sec:bridge` written (exercise: "as if
  the conjectures were proven").**  New `experiments/bridge_proven.tex` (kept out
  of the build; compiles standalone via `build/bridge_check.tex`, 4 pages) and the
  companion `experiments/BRIDGE_REVISION_IF_PROVEN.md` (retired 19 Sep 2026 into `attic/submission_docs/`).  The revision turns the
  mapping into a derivation with named hypotheses: Lemma 1 (orthonormal basis from
  the finiteness of the state space), Theorem 2 (the reversible fragment is a
  permutation, hence unitary, hence `H = i hbar/T ln U` Hermitian), Corollary 3
  (equidistant spectrum `dE = 2 pi hbar/(N T)`; a non-uniform spectrum cannot come
  from `U` alone), Theorem 4 (Born weights from the counting measure on the fibres
  of the many-to-one steps), plus the arrow-of-time subsection and the no-signalling
  corollary that replaces the postulate of `subsec:nosignaling-formal`.  The note
  maps H1-H4 to what would have to be proven, what the repository already has
  (H1/H2 have a head start: the dissipative stages are what the membership theorem
  and the sieve work characterise) and what is absent (H3, a preparation
  assumption that a deterministic rule cannot prove); it also lists the eight
  knock-on edits (Introduction map and status paragraph, abstract, the
  no-signalling remark, `POSTULATE_VS_EMERGENT`, the WP6/WP7 status lines) and
  states explicitly that **nothing was applied**: the submission keeps Status (C).
- **2026-09-14 -- The geometric-quantum hypothesis is FALSIFIED (last positive
  path closed).**  New note `experiments/GEOMETRIC_QUANTUM.md` + reader
  `experiments/analyze_geometric_quantum.py`.  Three independent arguments:
  (i) the contact graph at the turnaround is **complete at every accessible
  size** (`d_max = sqrt(3)(L-1)/2 < L = 2*RMAX` for `L = 7..15`), so the contact
  capacity is `W`, not a small integer; (ii) the shell and ball capacities (the
  free-sphere and toroidal conventions agree up to `RMAX`: shells
  `26, 66, 158, ...`, balls `27, 93, 251, ...`) contain none of the stable
  populations `2, 3, 5`; (iii) scanning all 59 `groups.csv` under `build/` shows
  the realised populations form a **continuum** `1..24` (plus 26-28, 30, 33 in the
  merging-tribe runs) rather than a gap spectrum.  **Documentation discrepancy
  found:** the manuscript's quoted shell triple `(6, 26, 98)` is reproduced by
  neither convention (band `(26, 66, 158)`, exact-squared cumulative
  `(6, 32, 98)`), so the sieve-weighting estimate was corrected (expected `s2B`
  passes ~18 -> ~35; the conclusion is unchanged) and the geometric paragraph now
  cites the verified numbers plus the completeness result.  What survives
  geometrically: the clock (`era = 2*RMAX = L` frames) and the contact counter as
  an overlap measure.
- **2026-09-14 -- P7 written, run and CONFIRMED (the scope clause measured).**  New
  harness `experiments/p7_concentration.cpp` (+ `build_p7_concentration.bat`,
  reference and `DD_INTRA_ISLAND_FIX` binaries): a prepared `1K+2D` island of one
  charge word in a 15x5x5 tube (`RMAX = 2`, contact range `2*RMAX = 4`), planted
  either co-located (one site) or spread 5 cells apart (beyond contact).  Results
  (`build/p7_concentration/*.log`): reference + co-located -> contacts appear at
  frame 2 (156, then the periodic 396/156/0 pattern) and **one promotion** fires,
  `1K+2D -> 2K+1D`, max group 2; reference + spread -> **zero contacts in every
  frame**, roles conserved, max group 3; `DD_INTRA_ISLAND_FIX` + co-located ->
  identical contacts, **zero promotions**, population 3 survives.  So (i) the
  theorem's bound is reached dynamically, (ii) the scope clause (co-location) is
  what the theorem measures, and (iii) the candidate's `81 x 3` positive is
  exactly the removal of T2 inside a family, now isolated at rule level.
  Manuscript updated: the Scope paragraph of Sect.~\ref{subsec:fixed-point}
  reports the run and the Limitations item no longer lists it as pending; the
  derivation note gained section 8 with the four-run table.

  `pop == L/3 = 0` (i.e. the theorem survives `L=15`); anything else falsifies
  it.




- **2026-09-11 -- WP4.5 DONE (cleanest Route-B positive).** At R = 5 (tube
  21x15x15, both pB and sB live = 2730 cells) with an open sieve, `EM_FIRST_FSM`
  keeps two equal-charge clouds as two distinct S singletons for 16 frames
  (`enc_repel` 182); at s2b=256 and the reference sieve they merge.  Recorded in
  `PBSB_ISLANDS.md`.
- **Next:** commit the session's artefacts -- the new notes
  (`DYNAMIC_QUANTIZATION_DERIVATION.md`, `SIEVE_SWEEP_PRODUCTION.md`,
  `PHOTON_MEDIATION_FAR.md`, `GEOMETRIC_QUANTUM.md`), the readers
  (`analyze_sieve_sweep.py`, `analyze_geometric_quantum.py`), the harnesses
  (`p7_concentration.cpp` + `build_p7_concentration.bat`,
  `build_island_census_exc_fix.bat`, `build_probe_em_orphan.bat`), the
  `island_census.cpp` `[EL]` argument, and the edited notes and manuscript -- then
  the DOI deposit (author) and submission.  Optional, not blocking: the local
  reformulation of the family-rigid idea, and a candidate rule keyed on the shell
  index (its falsifier is in `GEOMETRIC_QUANTUM.md` section 5).
- **2026-09-18 -- J0 and J1 of the spin programme: circulation is real, and it protects.**
  New note `experiments/J1_SPIN.md`.  Rationale: the aggregation line closed with a
  frozen bound (T1-T5 cap a group at 2) because every mechanism tried so far either read
  the seed multiplicity (`EXCLUSION_FSM`, `DD_INTRA_ISLAND_FIX`, `w / ISLAND_SIZE`) or
  had no interior balance at all; a pile can be deformed 3 -> 2 continuously, a rotating
  one cannot.  So the group is given a conserved spin `J = sum r x m` (r = member - chief,
  toroidal; m = member momentum) and asked whether it does any work.
  **J0 (reader only, no rule change, so the baseline is measurable):**
  `experiments/island_census.cpp` gained `chiefSpin()` (int64 `sum r x m` with toroidal
  offsets), a `spin_groups` / `max_Jmag2` pair of census columns and `Jx,Jy,Jz` per group;
  `experiments/J0_spin_reader.py` (stdlib) reads them.  Sweeping every archive
  (`build/island_census/J0_el9` = `L=9, S=16384` run at 4 frames, plus `L15`, `ref_el6`,
  `mm0`, `mm1` and `census_ref_el9_s64`) gives **3206 groups, `nonzero_J = 0`,
  `max_|J|^2 = 0`**.  The cause is structural, not a cancelling accident:
  `inertia_fixture::prepare` assigns `m` only to the propeller layers, so every body
  keeps `m = 0`, and in the production path a body's own `m` never self-propels it
  (`m` is preserved at `utils.cpp:181-183`, the copy, and at `simulation.cpp:413-415`;
  `interaction.cpp` never writes it) and enters motion only
  through a *partner's* contribution to `reloc`, `interaction.cpp:1541-1543` /
  `1867-1869`, consumed by `applyMomentum()`, `simulation.cpp:423`).
  **J1 (candidate rule):** new macro `SPIN_GATED_FSM`, always built with
  `SURFACE_ESCAPE_FSM` (whose release test it gates) and entirely inside the `#ifdef`, so
  the reference build is untouched (diff `+44` lines all guarded; the plain
  `build_island_census.bat` rebuild is clean).  S1 accumulates the chief-frame `J` before
  the release test; S3 refuses to release a delegate while its chief's `J != 0`, without
  resetting the timer, so the veto is a conservation constraint rather than a freeze.
  New harness `experiments/spin_probe.cpp` + `build_spin_probe.bat` (`_ref` = threat
  alone, `_gate` = threat + S1/S3) plants a **frozen rotor** in a 15x5x5 tube: K at x=2,
  delegates at x=7 and x=12 (all pairwise toroidal distances 5 > 2*RMAX = 4, so nothing
  ever contacts and the escape rule is the only agent), arm `j0` (m = 0) vs `jnz`
  (m = +y / -y, `sum(m) = 0` but `J = (0,0,+10)` because both toroidal offsets have the
  same sign in the cross product).
  **All four registered predictions confirmed exactly:** `ref`+`j0`, `ref`+`jnz` and
  `gate`+`j0` all release both delegates (`escapes = 2`, final `1K+0D+2S`), while
  `gate`+`jnz` **holds the group (`escapes = 0`, `spin_vetoes = 20`, final `1K+2D`)**.
  So `J` is inert on its own (adding the momenta changes nothing) but load bearing once a
  rule consults it: the first measured case in the project where a group's membership is
  decided by its circulation rather than by a read of `L/3`.  Stated limits: `J` protects a
  size, it does not select one (no `dN` vs `N` crossing yet), the rotor is planted rather
  than generated, `J` is not yet conserved across joining/promotion/pair formation, and
  there is no `L`- or `RMAX`-sweep.  Registered follow-ups J2-J5 in the note.
- **2026-09-18 -- J2 done: `J` protects a size, it does not SELECT one (and the
  control is the interesting half).**  S3's veto tests `Jx || Jy || Jz`, i.e. it is a
  *boolean* with no `N` dependence and no `|J|` magnitude, so the prediction
  registered before the run was that protection is FLAT in `N`.  New harness
  `experiments/spin_ring_probe.cpp` (built by the same `build_spin_probe.bat` into
  `spin_ring_ref` / `spin_ring_gate`), tube `61 x 5 x 5` (`RMAX = 2`, contact
  range 4), chief at `x = 30` and `N` delegates on the long axis at every pairwise
  toroidal distance `>= 5 > 4` (so nothing ever contacts and the escape rule is the
  only agent), swept `N = 1..6` x 2 builds x 3 arms = 36 runs at 8 light frames.
  Arms: `j0` (all `m = 0`), `coherent` (members at `+5k`, all `m = (0,+1,0)`,
  `J = 5*N(N+1)/2` = 5, 15, 30, 50, 75, 105) and `cancel` (members on alternating
  sides in PAIRS at the same distance, same `m`, so the `r x m` terms cancel and
  `J = 0` exactly).
  **Result, both halves of the prediction exact:** `ref` releases all `N` in every
  arm; `gate` releases all `N` for `j0` and `cancel`; `gate` releases **0 for every
  `N = 1..6`** in `coherent` with all members held.  So (i) the protection curve is
  flat -- **no `N` is selected, and `dN` vs `N` is the wrong instrument for S3** --
  and (ii) the decisive control: `coherent` and `cancel` carry *identical individual
  momenta* and differ only in whether the `r x m` terms add up, and `cancel` is
  released exactly like the `m = 0` arm.  Protection therefore tracks `J` as a
  **collective/coherent** quantity, not the motion of the members.
  The veto arithmetic is closed-form: counts `7, 12, 15, 16, 15, 12` for
  `N = 1..6` are exactly `N*(8-N)` (the timer expires at tick `W_USED = N+1`, so
  each member is vetoed once per frame over the last `8-N` frames) -- one veto per
  member per expired frame, no free parameter.  Also fixed in passing: the first
  draft of the `cancel` arm placed members at `+-5k` (steps 1,2,3...) so it gave
  `J = -15` instead of 0; the arm now pairs the distances.  Logs
  `build/spin_probe/ring_<build>_N<N>_<arm>.log`; note the J1 entry above (J0/J1).
  Registered next: S4 (`|J| > J_max` fission with `J_max` from the cavity `RMAX`,
  the only kind of rule that can *select* a size without reading `L/3`) and S2 (the
  orbital step that would make the rim-speed cap physical).
- **2026-09-19 -- charge-quantization audit: anchors re-verified, and a live bug found and fixed
  (the whole winding programme was defined only in a file that is never compiled).**  Audit of the
  charge-quantization implementation (code anchors, `CHARGE_SPECTRUM.md`,
  `DYNAMIC_QUANTIZATION_DERIVATION.md`, `analyze_charge_spectrum.py`, `POSTULATE_VS_EMERGENT.md`).
  (i) *Anchors.*  The `file:line` references of the charge notes had drifted because they were
  written against a never-compiled copy of the interaction source (`src/interaction.cpp`, era of
  the pre-move build path).  Re-verified against the compiled tree and corrected: `canFormPair`
  `147-173 -> 210-237` (`CHARGE_SPECTRUM.md` and `analyze_charge_spectrum.py`), conjugation-hook
  XOR `simulation.cpp 872 -> 873`, turnaround latch `851 -> 852`, `effective_t 372 -> 415`, and the
  whole T1-T5 table of `DYNAMIC_QUANTIZATION_DERIVATION.md` (T1 `chief_transition.h:53-56 ->
  66-73`, T2 `4-16 -> 14-26` invoked at `:60`, T3 `20-23 -> 30-33` invoked at `:61-64`, T4
  `interaction.cpp:1202-1221 -> chief_transition.h:66-73` reached via the call site
  `interaction.cpp:1592`, T5 `28 -> 38`, contact gate `631-633 -> 942`).  Both notes now state the
  convention (every path relative to `src/`, model sources under `src/model/`) and record the
  correction, and no longer risk citing the duplicate.  NOTE for later entries: the port in (ii)
  shifted `interaction.cpp` by +8 below old line 106 and by +21 below old line 746, so any anchor
  written before it had to be re-mapped by MEANING, not by arithmetic -- the numbers above are the
  post-port ones (see (vi) for the rest of the repository).
  (ii) *The bug.*  `src/interaction.cpp` (untracked) was identical to the compiled
  `src/model/interaction.cpp` **except** for two `#ifdef WINDING_GATED_FSM` hunks (21 lines: the
  `winding_vetoes`/`chief_W` definitions and the T1-S3 leave-veto in the surface-escape loop).  No
  build script compiles the root path -- every `*.bat` and the `Makefile` use
  `src\model\interaction.cpp` -- so the winding feature was **implemented only in a file that no
  build ever compiles**, while `simulation.h` (modified, uncommitted) declares its symbols.  The
  two hunks were ported into `src/model/interaction.cpp` (`git diff --stat`: 21 insertions, the
  file went 2145 -> 2166 lines) and the now redundant copy parked in
  `attic/interaction_root_duplicate_ported.cpp`.  Inert for every existing build: all 21 lines sit
  inside `#ifdef WINDING_GATED_FSM` (plus one blank line), and `interaction.cpp` compiles clean in
  both modes.
  (iii) *Second bug, in the probe.*  `experiments/winding_probe.cpp` re-declared the counters at
  global scope (`extern unsigned surface_escapes;` etc.) beside `using namespace automaton;`, which
  made every unqualified use ambiguous (`error C2872` at lines 222/255): the probe did not build at
  all.  The redundant externs were removed -- `model/simulation.h` already declares all three
  inside `namespace automaton`, which is the `spin_probe.cpp` style.
  **Validated end to end:** both arms build (`ref_exit=0`, `gate_exit=0`) and the four arms
  reproduce the registered prediction exactly -- `ref/w0` escapes 2, final `1K+0D+2S`; `ref/w1`
  escapes 2 (`W` is inert while no rule consults it); `gate/w0` escapes 2, `vetoes=0`; `gate/w1`
  **escapes 0, `winding_vetoes=20`, final `1K+2D` -> WINDING PROTECTS**.  Logs under
  `build/winding_probe/`.
  (iv) *Repository hygiene.*  The ten root-level `.obj` leftovers (13 Sep 2026, produced by the
  pre-move build path) were deleted; `OBJ_DIR = obj` in the `Makefile`, so they were never inputs
  and `*.obj` is already ignored.  `CHARGE_SPECTRUM.md` section 6 is now marked INCOMPLETE /
  resumable with the exact commands that finish the `mm_eps` arm pair (the harness does not create
  `outdir`, and the `gate`/`WINDING` builds above do not cover it).
  (v) *Status of the claim itself is unchanged:* `L/3` stays an axiom, the measured population
  quantum is 2, and `J` protects without selecting a size.  This entry is hygiene plus one repaired
  feature, not new physics.
  (vi) *Second anchor pass (whole-repository sweep).*  A scripted audit of every `file:line`
  reference in `experiments/*.md` (39 anchors, script parked in `attic/` during the pass) showed the
  drift was broader than the charge notes: most citations had been written against the same
  pre-move duplicate, and the port in (ii) then shifted `interaction.cpp` by +8 / +21, invalidating
  anchors that were correct before it.  All were re-mapped by MEANING and corrected:
  `canFormPair` `202-229 -> 210-237`; the pair-branch ordering in `CHARGE_SPECTRUM.md`
  (`1652-1654`, `chiefContact` call site `1592`, internal early return `1598-1613`); the contact
  gate `interaction.cpp:942` (cited twice by `DYNAMIC_QUANTIZATION_DERIVATION.md`); `EM_FIRST_FSM`
  `884-984 -> 1451-1473` (`PBSB_ISLANDS.md` and the WP4.1 entry below); the J0 entry above and
  `J1_SPIN.md`: the "`m` is preserved" anchor `interaction.cpp:838` pointed at a function that never
  touches `m` -- the verified statement is that the ONLY writers of `cell.m` are `initSim.cpp:133`
  (zero at birth), `polarization.cpp:525` / `567-569` (the election) and the copy at
  `utils.cpp:181-183`, so `interaction.cpp` never writes it; the partner contribution to `reloc` is
  at `1541-1543` / `1867-1869` (was `1468-1470` / `1794-1796`), with the model's own statement at
  `simulation.cpp:413-415`; `relocate()` `1542 -> 2046` (`draft = north` at `2067`) and
  `applyMomentum()` `410 -> 423`; the `pB`/`sB` sign definition `365-366 -> 377-378` (also in the
  source comment at `simulation.cpp:370`); the per-frame `homB` clear `1485-1487 -> 2111`; the
  `rotatePartners` edge-pairing quote `simulation.cpp:86-88 -> 87-89` (`ISLAND_CENSUS.md` and
  below); `update_pulsating_wavefront()` `109 -> 110`; the contact guard for the ported producers
  `603 -> 942`; and `gravity_probe_DESIGN.md`'s "chief layer K, other layers S
  (`initSim.cpp:135`)" -- a substantive error rather than a shifted line, since the seed leaves
  EVERY source `S` (`initSim.cpp:120-121`) and `K` is elected dynamically.
  Anchors that verified correct were left untouched: `utils.cpp:36`, `utils.cpp:181-183`,
  `initSim.cpp:76-80`, `initSim.cpp:96`, `charges.cpp:177`, `config.h:61`, `island_identity.h:6`,
  `chief_transition.h:14-26/30-33/38/66-73`, `simulation.cpp:423`, `simulation.cpp:873`,
  `inertia_fixture.h:65-70`, `inertia_fixture.h:95`.  The same sweep was then run over the model
  sources themselves (`src/model/*.cpp|inc`, `src/include/model/*.h`: 6 references), which found
  three stale ones, all comment text only (no behaviour change): `simulation.cpp:370` cited
  `simulation.cpp:365-366` for the reference `pB`/`sB` sign definition (now `377-378`);
  `initSim.cpp:375` cited `simulation.h:377` for `isIslandChief` (now `405`); and
  `POSTULATE_VS_EMERGENT.md` cited `island_census.cpp:285` for the `W = 3L^2` allocation (a printf
  line) and `simulation.h` (368-380) for the breathing phase -- now `island_census.cpp:15` +
  `initSim.cpp:419`, and `simulation.h` (411-420).

- **2026-09-19 -- `REFERENCE_CONFIG.md` inventory completed, and its drift check repaired.**  The
  candidate-macro inventory was missing the whole post-WP8/J-programme set and, on the other side,
  the verification command it documents could never pass.  (i) *Macros added* (14 rows): the table
  gained `SURFACE_ESCAPE_FSM`, `SPIN_GATED_FSM`, `WINDING_GATED_FSM` (marked PARKED / UNTESTED, per
  the `winding_probe` parking entry below), `PHASE_DISTINCT_FSM`, `FAMILY_SELECTIVE_FSM`, `ADDRESS_TARGET_FSM`,
  `ISLAND_ALIGNED_W_ROTATION`, `PLACED_FAMILY_SEED`, `MULTIFREQ_RAY_FSM`, `PAIR_STACK_ABSORB_FSM`,
  `HOMB_CONSUMER_TRANSPORT`, `POLAR_MAGNITUDE_FSM`, `PAIR_WORD_LOG` and a harness-only row -- the
  two tables are now one inventory split chronologically, stated to be COMPLETE (45 feature macros
  in the model sources, no more), with a copy-pasteable enumeration command that reproduces them
  (tested: 45 macros + the 5 include guards).  (ii) *Stale anchors corrected* in the rows that
  already existed: `EM_FIRST_FSM` `~893 -> 1451-1473`, `EM_FORCE_PREREQ` `~884 -> 1251`,
  `EM_NOS2B_FSM` `~906 -> 1464`, `ORPHAN_GUIDANCE_FSM` `61/624/823 -> 154/161/547/567/991/1190`,
  `EXCLUSION_FSM` `442/558/988 -> 776/892/1575`, `HOMB_PRODUCER_FSM` `~899 ->
  1261/1924/1935/2053` (+ `simulation.cpp` 432/447/753); the include-guard list was fixed as well
  (`geometry.h` uses `GEOMETRY_H`, no trailing underscore -- the doc said `GEOMETRY_H_`).
  (iii) *Fingerprint.*  The "current working-tree" line still read `4c2e60...` while
  `experiments/model_fingerprint.ps1` defaulted to a third value (`428ece...`), so the documented
  drift check could never match; both now carry the measured
  `7f5eb50e14ebdb980b97fbde60f50eaf75317ee763c07b86113334894ae6b1a2`, and the frozen per-file table
  was left untouched after re-verifying that its 23 hashes still combine to the recorded `5b0944...`
  (the intermediate suspicion that the frozen table was inconsistent was my own quoting error and is
  retracted; nine files differ from it today -- `attractor.h`, `bridge.cpp`, `charges.cpp`,
  `chief_transition.h`, `initSim.cpp`, `interaction.cpp`, `simulation.cpp`, `simulation.h`,
  `utils.cpp` -- because the working tree has accumulated post-freeze work).

- **2026-09-19 -- `winding_probe` assessed and PARKED (not decision-grade).**  Question asked: is
  the T1 spatial-winding harness useful?  Answer: as *science*, no -- and the reasons are
  structural, not unfinished runs.  (1) The gated quantity is a file-scope global
  (`interaction.cpp:111`, `chief_W[3]`), written only by `plantWinding` and read only by the
  leave-veto (`:761`), so `W != 0` vetoes EVERY release in the lattice rather than the releases of
  the group that carries the label -- invisible only because the harness tube holds one group; and
  no winding number is computed anywhere in the model.  (2) No discriminating arm: unlike J1/J2,
  where the protected quantity is derived (`J = sum r x m`) and `cancel` separates coherence from
  motion, `w0` vs `w1` differ only in the value of the global -- no magnitude response, no axis
  independence, no cancellation control.  (3) One geometry and four runs, with a hardcoded verdict
  (`held = (nd == 2u && maxPop == 3u)`), against J2's 36 runs and its closed-form `N*(8-N)` veto
  arithmetic.  (4) Undocumented: no note, no `REFERENCE_CONFIG.md` inventory row, and both the
  source and its build script were untracked.  What it DID establish is plumbing -- it is the only
  harness that exercises `WINDING_GATED_FSM` and it validated the port in the entry above (4/4
  arms; `gate/w1` held with `winding_vetoes = 20`) -- a test of the harness, the loop and the
  compile flag, not evidence that a physical winding protects anything.
  **Decision:** `experiments/winding_probe.cpp` and `experiments/build_winding_probe.bat` are
  parked in `attic/` (the source keeps the C2872 fix without which it never compiled), with an
  `attic/WINDING_PROBE_PARKED.md` note that records the four defects, the revive procedure and the
  two things that would make it useful: **T0** (compute the label -- net member displacement along
  a cycle of T^3 per light frame) and the **two-group selectivity control**, which FAILS against
  the implementation as of this date and is therefore the cheap kill test that forces the label to
  become per-chief state (scoped like `parent` / `spin_jmax2`).  If a *selection* rule is ever
  wanted (the registered S4 target), the winding route needs a magnitude threshold whose "cavity"
  analogue is the torus perimeter, not `RMAX`.  The model-side macro is untouched and still OFF in
  every reference build, so parking the harness leaves `WINDING_GATED_FSM` an explicitly UNTESTED
  candidate rather than a tested one.

- **2026-09-19 -- Manuscript: `L/3` retired; the section now ASKS the question it used to answer by
  assumption.**  Requested radical change ("retire todas as referências a L/3 para nos concentrarmos
  numa formação de ilhas realmente emergente").  19 edit sites in `doc/manuscript.tex` (35 lines
  changed, no structural edit), and the result was verified in the COMPILED PDF, not only in the
  source: `pdftotext` reports **0** occurrences of `L/3`, **0** of `charge quantization` and **0** of
  `Dynamic charge quantization`; 57 pages, no LaTeX errors.  What changed:
  (i) *The partition is now declared, not derived.*  `\ell=\frac{W}{N_{I}}` (twice, both places where
  the old equation said `=L/3`), `W=3L^2=N_I\,\ell`, `ISLAND_SIZE = \ell = W/(9L)`, `N_I=9L` declared;
  sentences added where the partition is introduced saying the two integers are declared by the
  topology and derived by no rule.
  (ii) *The hypothesis became the open question.*  The sentence that asserted `1+n=L/3` and
  "`9L` spatial islands of `L/3` constituents" now reads: whether the aggregation dynamics selects a
  *preferred* population, the same for every family or varying with the lattice, is the open question
  the section tests; the seed declares how many addresses *share* a family, never how many end up in
  one island.
  (iii) *The negatives were kept, restated `L/3`-free.*  Max population 2 with no group above it at
  `L=9` (235 K + 8 D) and `L=15` (667 + 8); the degenerate rate crossing; the flux harness'
  INCONCLUSIVE/anti-restorative outcome; "no rule produces a larger quantum without *reading* the
  seed's multiplicity".
  (iv) *New use of the same identity*: `W=N_I\ell` now makes the no-independent-bound point with
  numbers -- 235 chiefs against `N_I=81` at `L=9`, 667 against 135 at `L=15` -- i.e. the declared
  partition does not control the dynamics at all.
  (v) *`J` paragraphs*: "without reading the seed's partition" and "does not restore a
  size-selecting quantum".
  (vi) *Titles, keywords and the conjectural text*: section "Dynamic charge quantization and the
  scaling of `W`" -> **"Emergent island formation and the scaling of `W`"**, subsection -> **"Is the
  island population quantized?"**, keyword `charge quantization` -> `island aggregation`, comment
  banner updated; and the prose that still *asserted* the mechanism ("sustained by dynamic charge
  quantization"; "its stability comes from dynamic charge quantization") now reads "the
  island-formation dynamics", with the stability claim explicitly conjectural (the balance it named
  is the degenerate one).
  (vii) *Second pass: `9L` removed as well* (requested after the first pass).  `N_I` is now declared
  in prose with no formula, so all seven `9L` occurrences are gone: the two `N_I=9L` equation blocks
  were deleted (the sentence declares `N_I` directly and states both integers are declared inputs of
  the seed, evaluated once and reported with the run), `\nu\in[0,9L-1]` -> `[0,N_I-1]`, the
  topological-constant list now reads `N_I`, the factorization prose reads "`N_I` charge families,
  each with `\ell=W/N_I` copies", and the code-constant list reads `ISLAND_COUNT = N_I`,
  `ISLAND_SIZE = \ell = W/N_I`.  Verified again in the compiled PDF: 0 occurrences of `L/3`, 0 of
  `9L` (one `pdftotext` hit turned out to be the page number "9.9" followed by the heading
  "Lorentz probes in the simulator"), 0 of `charge quantization`; 57 pages, no errors.
  **Arithmetic path, closed (option 1 applied).**  The run comparison no longer reports the values of
  `N_I`; it reports the *disparity* as a factor (`2.9x` at `L=9`, `4.9x` at `L=15`), which carries the
  same argument (the declared partition does not bound the dynamics) without letting a reader divide
  by `L` to recover `9L`.  One residual path remains and is inherent to reporting the candidate
  honestly: the candidate-mechanism paragraph states "81 chief groups of exactly 3 constituents"
  (`K=81`, `D=162`) at `L=9`, from which the family multiplicity `3` can be inferred.  Removing that
  would mean hiding a measured outcome, so it is left and flagged here instead.  The deeper fix -- a
  seed whose `N_I` is not proportional to `L` -- is still the only way to remove the axiom from the
  MODEL, and it invalidates the archived census numbers.
  **Caveat that must not be forgotten.**  `L/3` is still arithmetically recoverable from `W=3L^2`
  and `N_I=9L`, because the SEED in the code partitions by `ISLAND_SIZE = W/(9L)` and the two
  candidate macros (`EXCLUSION_FSM`, `DD_INTRA_ISLAND_FIX`) still read it.  The manuscript now
  *says* the partition is an axiom instead of claiming a derived quantum; removing it from the MODEL
  is a seed/rule change that would invalidate the archived census numbers, and it is a separate
  decision.
  **Knock-on, not done here:** the evidence notes keep their `L/3` (they are the measurement
  record): `WORK_PLAN.md` (36), `EMERGENCE_SEARCH.md` (24), `DYNAMIC_QUANTIZATION_DERIVATION.md`
  (20), `ISLAND_CENSUS.md` (18), `LIGHTMATTER_DECOUPLE.md` (12), `SIEVE_SWEEP_PRODUCTION.md` (11),
  and the code (`initSim.cpp`, `interaction.cpp`, `utils.cpp`, `color_fsm.inc`, `simulation.h`,
  `attractor.*`).  The ones that now CONTRADICT the manuscript and need a pass before submission are
  the claim-facing documents: **all five were RETIRED on 19 Sep 2026** (`attic/submission_docs/`), namely
  `POSTULATE_VS_EMERGENT.md` (which carried the P/M/C row "Charge unit `L/3`"), `CANDIDATE_VERSION.md`,
  `COVER_LETTER.md` and `DATA_DEPOSIT.md`; `README.md` was corrected in the same pass.  Also pre-existing and untouched: `manuscript.bib` has no `hofer` key, so the
  bibliography warns `\missing{hofer}` on page 38 (the `.bbl` already recorded it as missing before
  this edit).

- **2026-09-19 -- Hard-to-adapt experiments RETIRED (34 files parked); the next battery waits for a
  design discussion on emergent quantization.**  Follow-up to the manuscript pass: instead of
  rewriting in place, the harnesses whose *setup or analysis* is parameterised by the retired
  partition were parked in `attic/retired_experiments/` (9 sources + 25 build scripts; README there
  records the criterion, the list, the kept set with reasons, and the restore procedure).  Parked:
  `first_election` (families `9L`, `copies L/3`), `island_seed` (reduced-seed branch), `p7_concentration`
  and `promotion_three` (plant a seed family of 3 copies), `turnover_ablation` (flux generator for the
  falsified attractor claim), `spacing_probe` (planted electric channel on the `w/3` stand-in),
  `coagulation_random` (counts groups of size `EL/3`), `lightmatter_decouple` (`targetPop = EL/3`),
  `color_fsm` (family bookkeeping on `w/3`), plus the eleven `build_island_census_*` candidate-route
  variants.  **Kept deliberately**: `island_census.cpp` + `build_island_census.bat` (the reference
  aggregation instrument -- what it measures is exactly what the new focus needs), the spin/J probes,
  the whole inertia family (the only harnesses where a body actually moves), `alpha_probe`, the
  charge-word and unit-test harnesses, and the polarization-bootstrap line.  Verified afterwards: no
  remaining build script references a parked source (0 dangling), 27 harnesses and 59 scripts remain in
  `experiments/`.  The `.md` notes were NOT touched -- they stay as the measurement record and now
  describe harnesses that live in `attic/`.  All 34 files are also in git history
  (`git checkout -- experiments/<name>` restores), and no model source was touched.
  **Next step is a discussion, not a build**: the proposed basis for the new battery is the three
  non-contractible loops of the torus (H_1(T^3) = Z^3), i.e. the integer winding triple of a
  persistent island.  Open design questions recorded for that discussion: (a) the winding must be
  *computed* from the transport history, never planted (the parked `WINDING_GATED_FSM` probe's
  failure mode), and the per-source accumulator must be cell state rather than a global, to respect
  the purity constraint; (b) the instrument partly exists already -- `experiments/inertia_fixture.h`
  accumulates `unwrapped[w][k] += d` per layer per axis, so `unwrapped mod L` *is* the winding triple
  for the moving prepared islands, while the reference seed has no transport at all (J0: `J = 0`
  identically; census: one centre, `max_pop = 2`, 64 frames), so the all-zero baseline comes first;
  (c) the four questions the battery would have to answer: conservation of the triple across
  capture/escape, whether the dynamics *prefers* particular triples, whether the triple couples to the
  charge word (the only route by which charge quantization could become emergent), and axis-permutation
  symmetry as a built-in falsifier; (d) the test grammar stays that of this work stream: computed
  invariant, no partition parameter in the rule, a selection test not merely a protection test, a
  two-group control, and a registered falsifier.  Also noted: `is_core` in `simulation.h` is labelled
  "Winding core flag" and is never read -- a vestige of this same idea, to be used or deleted.
- **2026-09-19 -- B0 DONE: the winding triple is measurable and the instrument is validated (no physics
  claimed).**  New read-only harness `experiments/winding_observatory.cpp` +
  `experiments/build_winding_observatory.bat` + note `experiments/WINDING_B0.md`; logs/CSVs under
  `build/winding_observatory/`.  Design rule taken from the parked probe: define **no** rule and plant
  **no** global -- the triple is computed from the transport history that already exists
  (`inertia_fixture.h` accumulates `unwrapped[w][k] += wrappedDelta(previous, current)`), in contrast to
  the parked `WINDING_GATED_FSM` probe, which planted a global `chief_W[3]` and was near tautological.
  Geometry: transport axis long (15), others short (5), so `RMAX = 2` in all three arms and the arms
  differ by an exact axis relabelling.  **Seven arms, 80 frames each, reference sieve:** `baseline`
  `disp=(0,0,0) W=(0,0,0)`; `x+` `(+20,0,0) W=(5,0,0)`; `x-` `(-20,0,0) W=(10,0,0)`; `y+`
  `(0,+20,0) W=(0,5,0)`; `y-` `(0,-20,0) W=(0,10,0)`; `z+` `(0,0,+20) W=(0,0,5)`; `z-` `(0,0,-20)
  W=(0,0,10)`.  Three instrument facts: the accumulator reproduces exactly from the fixture's own stored
  `previous` (`accum_mismatch = 0` in every arm -- note the *first* version of that check was itself
  buggy and reported 60 false mismatches, see the note); the triple is axis-diagonal and
  sign-covariant, so the registered axis-permutation falsifier already holds for the measurement; and
  the no-driver baseline does not wind (the J0-style baseline).  Cross-validation: 20 cells in 80
  frames = `0.250000` cells per light frame, the published one-drive-pair velocity, reproduced from an
  independent harness.  **Two findings B1 must handle:** the body becomes `2K+1D` at frame 1 (a
  reference `D x D` promotion) and stays there for 79 frames, so "the island" is ambiguous at group
  level and B0 measures the *body's* centre of mass -- B1 must define the triple **per chief**; and
  `member_spread = 0` throughout, i.e. the transport is rigid at this size.  Registered next steps (not
  run): B1 conservation across capture/escape (falsifier: an `unwrapped` discontinuity not explained by
  `wrappedDelta`), B2 the generator (the seed still transports nothing), B3 coupling of the per-chief
  triple with the charge word (needs the seed to stop assigning charge words), B4 axis symmetry for
  candidate rules.  Explicitly NOT established: selection (the driver imposes the value), anything about
  the reference path, and conservation across events.
- **2026-09-19 -- The definition of "quantization" is fixed as the CONCRETE one, and the archived
  censuses already answer it: one equal-size island per charge word, at every lattice size.**  Author's
  correction, accepted: the three abstract framings (preferred magnitude / conserved quantum number /
  charge-equals-winding) all start from mathematical premises and miss what is meant.  The operative
  definition is: **the spontaneous formation of islands whose bubbles carry the same charge and have
  approximately equal size -- a geometric, structural, emergent phenomenon.**  That makes the observable
  a **size distribution per charge word**, not an invariant, and it is measurable on data that already
  exists.  New stdlib reader `experiments/analyze_island_spectrum.py` + note
  `experiments/ISLAND_SPECTRUM.md`; measured on the reference build (all candidate macros OFF):
  `L=9` (S=16384): 235 groups, histogram `1:227 2:8`, **8 non-singleton islands all of size 2, spread 0,
  one per charge word, 8 of 8 words**; `L=15` (S=16384): 667 groups, `1:659 2:8`, the same **8 islands
  of size exactly 2**, 8 of 8 words; open gate `S=64` at `L=9`: 145 groups, `1:140 2:5`, 5 of 5 words.
  `mixed charge = 0` in every row (same charge: exact), the group count does not move from frame 2 on,
  and the pattern is **independent of the lattice size** while the seed multiplicity changes 3 -> 5 --
  and no membership transition reads the multiplicity, so this is emergent rather than a partition
  effect.  **The honest gaps against the definition:** (i) it is not spatial -- `distinct_centers = 1`
  everywhere, so what forms is a membership structure, not separated islands; (ii) the aggregating
  sector is marginal -- 227 of 235 groups are singletons, i.e. 8 of 243 sources sit in a size-2 island;
  (iii) nothing grows past 2, because the same fixed-point bound that makes the size uniform also caps
  it, so the phenomenon is "one bound pair per charge word".  **Consequences for the next battery:**
  the *cap* is already emergent (local `D x D` promotion, no partition input) -- that half of the
  definition is met; what is missing is spatial formation, which needs transport, and no reference
  process generates a driver from the seed (B0 baseline; J0 `J = 0`).  So the next experiment is
  "can an island assembled by the reference rules acquire a driver spontaneously, and if it separates,
  do the sizes stay equal?" -- with the falsifier registered: if separation spreads the sizes, the
  uniform size was a property of the frozen co-located plateau only.  A cheaper, already-answerable
  question: why *exactly one* per word rather than zero or two (the theorem bounds it at one; the
  measurement attains one; the mechanism that makes it exactly one has not been isolated).
- **2026-09-19 -- `PARENT_SELECTIVE_FSM` implemented and measured: the cap breaks and the islands
  separate, but the rule set as specified has no chief source, so the membership collapses.**  Author's
  rule set (restated): `S x S` -> one `K`, one `D` (unchanged); `D x D` same island -> cohesion, no
  transition; `D x D` different islands -> one step apart per pair per light frame; `K x K` same charge
  -> one demoted to `D`, the two agglutinate.  The discriminator is the **dynamical parent**
  (`islandChief`), never the seed family -- which is what distinguishes this candidate from the retired
  `FAMILY_SELECTIVE_FSM`/`EXCLUSION_FSM` line that keyed on `w/3`.  Implemented macro-guarded:
  `chief_transition.h:17-29` (T2 off), `interaction.cpp:140-146` (`parentRepel`), `:556`/`:590`
  (clears), `:1607-1630` (record equal-charge `D x D` pairs of different islands), `:788-819`
  (`resolveParentRepulsion`, one antisymmetric step per pair per light frame through `reloc[]`, skipped
  if the pair merged during the frame), `:907-909` (frame-edge call).  New build script
  `experiments/build_island_census_parent.bat`; note `experiments/PARENT_SELECTIVE.md`.  **Reference
  proven inert**: with the macro undefined the preprocessed `interaction.cpp` is identical to `HEAD`
  after stripping `#line`/blank lines.  **Measured (L=9, S=16384, 4 frames,
  `build/island_census_parent/run4`)**: frame 2 `K=1, D=242, S=0, groups=1, unresolved=241,
  distinct_centers=57, max_pop=2, max_span=4, births=1, captures=2`; frames 3-4 frozen
  (`centres 58 -> 59`, `max_span 2`, all counters 0).  Reference at the same conditions: `235 K, 8 D,
  1 centre, 235 groups, 0 unresolved, births 235`.  **Two successes**: the cap is gone (nothing can
  eject a member) and the **spatial separation happened** (`1 -> 57 -> 59` centres, and it comes from a
  rule that reads only the emergent parent).  **One failure, with mechanism**: `births = 1` against
  `235` -- in the reference almost every chief is born from the `D x D` promotion cascade, and T2 is now
  off, so the only chief source is the one-off `S x S` election; meanwhile `K x K` keeps destroying
  chiefs and `demotesChief` explicitly does not reassign the demoted chief's delegates, so 241 of 242
  delegates are `unresolved`.  Chiefs are monotone non-increasing -> the structure collapses to 1.
  **The piece missing from the rule set**: `K x K` fusion must *absorb*, not orphan -- reassign every
  delegate of the demoted chief to the survivor (the `commitSourceTick()` frame-edge pattern already
  exists for exactly this kind of bookkeeping).  **Registered prediction for that run:** chiefs fall
  while populations grow, `max_population > 2` appears, several islands per charge word coexist, and
  `distinct_centers` stays > 1 because the `D x D` repulsion is already active.  **Falsifier:** if it
  collapses to one island per word (`max_pop ~ W/8`), fusion wins and the repulsion must become
  size-dependent -- the same runaway the directional-channel candidate showed (`b = +0.49`).
- **2026-09-19 -- `PARENT_NOMINATION_REPAIR`: the completed rule set resolves the structure (241 orphans
  -> 0) and produces the first uniform, spatially separated island population; the fusion absorption
  turned out to be inert, and the diagnostics say why.**  Follow-up to the entry above, which had
  diagnosed the collapse as a `K x K` problem.  **The diagnosis was wrong and the instrumentation said
  so:** the ablation build with the fusion absorption added (`PARENT_FUSION_ABSORB`) is *identical* to
  the fsm-only build at every frame, because `[absorb] K x K encounters = 0` at every frame edge -- only
  one chief ever exists there, so no `K x K` pair can form (consistent with `births = 1`).  The 241
  `unresolved` delegates come from somewhere else: the generic merge of `chiefContact` names as parent
  the **minimum-W address of two `S`** (not yet a chief), and T4 can copy a `D`'s parent; in the
  REFERENCE path the T2 promotion cascade repaired such pending nominations globally, and with T2
  disabled nothing did.  New frame-edge rule `PARENT_NOMINATION_REPAIR`: any source that a delegate
  names as its parent but that is not a chief is **promoted** (one pass; exactly what T1/T4 intended and
  what T2 was doing globally).  **Measured at L=9, sieve 16384, frame 2** (three builds, one ablation
  line): reference `235 K + 8 D, 1 centre, 0 unresolved`; fsm-only `1 K + 242 D, 241 unresolved, 57
  centres`; fsm+absorb identical to fsm-only; **fsm+absorb+repair `162 K + 81 D, 162 groups, 0
  unresolved, 57 centres, max_pop 2, births 162`, size histogram `1:81, 2:81` with spread 0** and every
  one of the 8 charge words carrying a non-singleton island (`[nomination] pending parents promoted=161`
  at that frame edge).  So the rule set *can* be made self-consistent at rule level, and the result is
  the first island population in the project that is simultaneously **resolved**, **uniform** (spread 0)
  and **spatially separated** (57 centres against 1 in the reference) -- the spatial separation coming
  from the parent-keyed `D x D` repulsion, which reads no partition parameter.  **What is not yet
  shown:** growth past 2 (`max_population = 2`: by frame 2 no `S` is left to recruit), and the count
  `162 = 2 x 81` suggests the pattern is **per seed family** (81 families of 3 copies -> 2 K + 1 D),
  i.e. potentially **seed-multiplicity dependent**, the opposite of the reference's one-pair-per-charge-
  word pattern.  **Registered decisive test:** the same builds at `L=15` (multiplicity 5): if the pattern
  tracks `ell` the regularity is a partition effect (same class as the retired routes); if it is `ell`-
  independent it is emergent.  Cost: ~31 min per light frame at `L=15`, so a background job.  **Cheaper
  companion already identified:** add `SURFACE_ESCAPE_FSM` (the only reference process that *creates*
  free `S` from a group) so release competes with recruitment -- the first non-degenerate rate balance
  this programme can measure.  Files: `src/model/interaction.cpp` (two macro-guarded blocks +
  frame-edge diagnostics `[absorb]`/`[nomination]`), `experiments/build_island_census_parent_repair.bat`,
  note `experiments/PARENT_SELECTIVE.md` (three-way table + corrected mechanism + reproduce commands).
- **2026-09-19 -- Release vs recruitment: built, inert at the cubic timescale, and the timescale itself
  is the finding.**  Following the previous entry's plan, the release companion
  (`PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR + SURFACE_ESCAPE_FSM`,
  `experiments/build_island_census_parent_escape.bat`) was built and measured at `L=9`: frame 2 is
  **numerically identical** to the repair build (`162 K + 81 D, 162 groups, 0 unresolved, 57 centres,
  max_pop 2, births 162`) with **`escapes = 0`**.  The reason is a timescale inside the release rule:
  `applySurfaceEscape()` releases a delegate only when `++framesWithoutContact[w] >= W_USED`
  (`interaction.cpp:770-772`), i.e. after `W_USED` **consecutive light frames without a same-charge
  contact** -- **243 frames at `L=9`**, 675 at `L=15`.  At ~50 s per light frame that is **~3.4 h of run
  time before a single release can fire**, so no rate balance can be reached in a cubic run of feasible
  length.  **This also explains a standing observation**: the flux harness found `escapes = 0` in *every*
  reference run (4-64 frames, `quantization/FINDINGS.md`), which was read as "the plateau is frozen";
  the code adds a second, independent reason -- the release rule's own timescale is longer than the runs
  made so far, so a frozen plateau and an inoperative release rule are indistinguishable inside them.
  **Two ways forward, both recorded in `experiments/PARENT_SELECTIVE.md`:** (i) *small `W`* -- the
  tube/prepared-island fixtures allocate `W = n + 2*pairs`, so the timer expires after 3-5 frames and
  release competes with recruitment in seconds (cheap; loses the cube topology; natural next instrument:
  a rate probe in the style of `winding_observatory.cpp`); (ii) *long cube run* -- ~250 frames at `L=9`
  in the background (~3.5 h).  No model source changed in this step beyond the escape build's macro set;
  the reference remains inert (preprocessed `interaction.cpp` identical to HEAD with all candidate macros
  OFF).
- **2026-09-19 -- Small-`W` rate probe built; the balance needs a different fixture, and the probe's own
  first reading was corrected.**  New `experiments/island_rate_probe.cpp` +
  `experiments/build_island_rate_probe.bat` (arms `rb_ref.exe` = `SURFACE_ESCAPE_FSM` and `rb_cand.exe` =
  + `PARENT_SELECTIVE_FSM` + `PARENT_FUSION_ABSORB` + `PARENT_NOMINATION_REPAIR`), writing the
  flux-harness format `run,island,frame,N,captures,escapes` (12 CSVs in `build/rate_probe/`).  Tube
  `15x5x5` gives `W = N + 2*PAIRS` (2-7), so the escape timer expires in 2-7 light frames instead of 243.
  **The probe now counts role transitions separately, and that corrected a first misreading of its own
  output:** what the membership diff reports as "escapes" in the *reference* arm are **promotions**
  (`D -> K`), not releases.  Measured: `ref` does `N-2` promotions and **zero releases** in every arm
  (0,1,2,1,2,3 for `N`=2,3,4,3,4,5) with zero recruits; `cand` is **completely frozen** (0 releases, 0
  promotions, 0 demotions, 0 recruits).  Two consequences.  (i) *Cross-validation*: the reference's
  `N-2` promotions take the tube island to population **2** in every arm -- the `N* = 2` plateau
  reproduced in a completely different geometry (tube, `W` = 2-7, `N` = 2-5) from the cubic census that
  found it (235 K + 8 D at L=9).  (ii) *Why the small-`W` route alone is not enough*: the escape timer
  is reset by any same-charge contact and a co-located prepared island contacts on every tick, so no
  release fires **even at `W = 2`** -- **release requires spatial separation, not merely a short timer**.
  And the candidate is inert in this fixture because a single co-located island has no
  *different-island* `D x D` pair, so the repulsion never fires and there is no free `S` to recruit.
  **Precise specification of the fixture the balance needs** (next instrument, ~40 lines in the
  `p7_concentration`/duo style): at least two islands separated beyond `2*RMAX`, free `S` bubbles within
  reach of each, small `W`, and enough frames for several release/recruit cycles.  Written up in
  `experiments/PARENT_SELECTIVE.md`; the reference remains inert with all candidate macros OFF.
- **2026-09-19 -- Placed multi-island fixture: first controlled turnover, and the release rule's
  structural tension exposed.**  The four-condition configuration specified in the previous entry is now
  built into the probe (`island_rate_probe placed FRAMES CSV`): `21x5x5` tube, `RMAX = 2` (contact range
  4), `W = 6` (escape timer 6 light frames), `w0 K_A@2, w1 D_A@9, w2 K_B@18, w3 D_B@10, w4 S1@5,
  w5 S2@15`, all `ch = 0x08` -- two islands beyond the contact range, one stray delegate per island out
  of contact with its own chief, the two strays adjacent (a cross-island `D x D` pair), one free `S` in
  contact with each chief.  **Measured, 30 light frames:** `ref` `islands=3, captures=2, escapes=1,
  releases=0, promotions=2, recruits=2`; `cand` `islands=2, captures=2, escapes=0, releases=0,
  promotions=0, recruits=2`.  So the reference turns over (promotions + recruitments -- **the first
  controlled turnover this programme has produced**, and its third island is born from a promotion),
  while the candidate recruits but never departs.  **Neither arm releases anything, and the mechanism is
  now visible**: (1) *the escape timer is reset by contact with ANY same-charge source, not only with its
  own group* (`contacted[w]` comes from `surfaceContactSeen`, `interaction.cpp:141-145`, `:763`), so the
  reference's promoted stray at x=9 sits one cell from `D_B` at x=10 and resets its timer for the whole
  run; (2) *the candidate's repulsion HEALS stray delegates back into their own islands*--with the two
  strays between their two chiefs, pushing them apart along the inter-stray axis pushes each toward its
  own chief (`D_A` from x=9 down to `K_A`@2, `D_B` from x=10 up to `K_B`@18).  **The structural tension
  this exposes:** a delegate can be released only if it is *isolated from every same-charge source* for
  `W` frames, while a released `S` can be recruited only if it is *in contact* with a chief -- isolation
  and contact are mutually exclusive unless something carries the bubble in between, and a freed
  singleton has `m = 0` ("a body's own momentum never moves it", J0).  Therefore the release -> recruit
  cycle **requires transport** (the drive pairs: 0.25 cells per light frame), and *that*, not the
  population dynamics, is why the flux harness has never seen turnover **and** stationarity together.
  **Next iteration (specified, not guessed):** place one drive pair whose transport carries a freed `S`
  across the gap between the islands (pair matched to the receiver island's affinity, `m` along the tube
  axis), with `W = 6` and the strays isolated until their timers expire.  Written up in
  `experiments/PARENT_SELECTIVE.md` (new section with the table, both structural facts and the tension).

- **2026-09-19 -- Driven fixture: the release -> recruit cycle closes, and the first real rate fit FAILS
  as registered (runaway, `b = +0.075`).**  The carrier the previous entry specified is implemented as
  `island_rate_probe driven FRAMES CSV`: the placed configuration plus a reciprocal drive pair
  affinity-matched to island B (`m` along `-x`, island A carrying a different affinity so the pair can
  dress only B), which walks B across the gap at 0.25 cells per light frame.  **Measured, 60 light
  frames:** `ref` `islands=3, captures=1, escapes=2, releases=1, promotions=1, recruits=1`;
  `cand` `islands=1, captures=4, escapes=3, releases=1, promotions=0, demotions=1, recruits=1`.  **Both
  arms now release *and* recruit** -- the first configuration in the project where the release -> recruit
  cycle closes, and it closes exactly as the structural tension predicted (the pair carries B into
  contact with the bubble island A released).  **The flux harness was then run on it
  (`quantize_stdlib.py ... --by-run`) -- the first time it has received data with turnover:** sanity
  passes with **zero islands reporting zero escapes** (the frozen-mode warning that fired on every
  archived dataset does not fire); binned fluxes `N=1` cap 0.000/esc 0.035, `N=2` 0.077/0.077, `N=3`
  **0.130/0.000**; mean `dN` monotone increasing (`-0.03, 0.00, +0.13`) with no zero crossing; drift fit
  `a = -0.1156`, **`b = +0.0754`** over 135 island-frames; per run `cand` `b = +0.0923`, `ref`
  `b = +1.0303`; **verdict FAIL (no restoring drift, no `N*`)**.  **Reading:** the structural hurdle is
  passed but the balance is **runaway** -- recruitment outpaces release, larger islands grow faster, the
  candidate ends with a single island -- which is precisely the falsifier registered in advance
  ("if it collapses into one island per charge word while the repulsion is on, the repulsion must become
  size-dependent"), and the same sign and shape as the directional-channel candidate (`b = +0.49`).
  **Next ingredient is therefore a size-dependent brake, not another channel**, so that
  `Gamma_esc(N)` can overtake `Gamma_cap(N)`; candidates, cheapest first: (i) repulsion steps paid per
  light frame proportional to the number of *delegates* instead of one per pair;
  (ii) a `|J|`-style threshold with the cap taken from the cavity radius (the spin programme's S4);
  (iii) a release trigger that depends on island size rather than on the contact-free timer alone.
  Written up in `experiments/PARENT_SELECTIVE.md` (new section with the table, the harness output and
  the three candidate brakes); the reference remains inert with all candidate macros OFF.
- **2026-09-19 -- Brake (i) falsified by the model's own step invariant; the brake must act on a
  transition, not on displacement.**  The first brake (`PARENT_REPULSION_PRESSURE`, nested in
  `PARENT_SELECTIVE_FSM`) made the per-pair repulsion impulse magnitude the number of *delegates* in the
  two islands (`nA + nB`, clamped per source to `2*RMAX`), so that an island's ejection pressure would
  grow with its own membership.  Built as a third arm (`rb_press.exe`: completed rule set + the macro) it
  aborts:

      [pressure] frame-edge: pairs=1 cap=4 max_push=2 clamped=0        (frame 1, placed fixture)
      Assertion failed: length<=1, file inertia_fixture.h, line 95

  `inertia_fixture.h:95` asserts that every constituent (P halves included) moves at most ONE cell per
  light frame -- `m` is a direction, not a jump -- and one delegate per island already asks for two cells,
  so `press` dies in frame 2 while `ref` and `cand` complete 40 frames on the same fixture.
  **Structural, not a tuning problem:** any magnitude above one cell violates the invariant, so
  size-proportional DISPLACEMENT is not expressible in this lattice; the size-dependent loss must come
  from a size-dependent TRANSITION (a release).  Control that comes free: on the `driven` fixture the
  brake is inert by construction (`[pressure] pairs=0` at every frame edge -- island B's delegate travels
  with its chief) and `press` reproduced `cand` exactly (`4 captures, 3 escapes`, same mechanism
  counters), confirming that no cross-island `D x D` pair means no brake effect.  **Brake (i) is dropped.**
  The menu is now (ii) a `|J|`-style threshold with the cap from the cavity radius and (iii) a release
  trigger that depends on island size, whose parameter-free form is "a delegate farther from its chief
  than the contact range `2*RMAX` is outside the shell cohesion can hold".  The falsified variant is kept
  verbatim in `attic/brake1_pressure_block.txt`; `src/model/interaction.cpp` is reverted (reference
  pre-processed output re-verified byte-identical to `HEAD` after the revert) and the third build arm
  removed, so `build_island_rate_probe.bat` is `ref` + `cand` again.  Written up in
  `experiments/PARENT_SELECTIVE.md` (new section: the abort, the control, the revised menu).









- **2026-09-19 -- Brake (iii) in its parameter-free form: the shell release fires, adds the missing loss
  channel, limits the growing island in the cubic census -- and is blind while the island is co-located.**
  `PARENT_SHELL_RELEASE_FSM` implements the brake on a TRANSITION (the displacement route being closed by
  `inertia_fixture.h:95`): a delegate whose torus distance to its chief exceeds the model contact range
  `2*RMAX` is outside the shell cohesion can claim and is released.  Reads only `lcenters`, `parent` and
  `RMAX` -- no seed partition, no `ISLAND_SIZE`, no `L`.  **Rate probe, `placed`, 40 frames:** the brake fires
  where the strays are (`[shell] frame-edge: range=4 max_dist=8 releases=2` on frame 1, then `max_dist <= 1`)
  and it creates the transition the completed rule set did not have: `cand` had `escapes = 0, releases = 0`
  where `shell` gives `escapes = 2, releases = 2` (captures 2 -> 3).  **Rate probe, `driven`, 60 frames:**
  releases 1 -> 2, and the flux harness (`driven_ref` vs `driven_shell`) gives `b = +0.0525` over 130
  island-frames against `+0.0754` for the same fixture without the brake -- it moves the balance in the right
  direction and still **FAILS** (no restoring drift, no `N*`).  **Cubic census, L=9, frames 0-4: the brake
  limits the island.**  While the island is co-located it cannot act -- `[shell] frame-edge: range=8
  max_dist=0 releases=0` at the first two frame edges, with frames 1-2 byte-identical to the same rule set
  without the brake (`162 K, 81 D, 57 centres, max_pop 2`) -- but after the frame-2 -> 3 reorganization the
  delegates acquire extent (`max_dist = 12`), the brake fires (**releases = 72** at that edge, 49 at the next)
  and **the largest island is cut from 33 members to 24 and then to 18**, against `8 K, 235 D, 0 S, max_pop 33`
  for the same rule set without the brake at frame 3.  That is the first rule in this campaign that limits a
  growing island's population in the cubic census.  **What it cannot do:** `parent` is a global label, so an
  island can hold an unbounded population in ONE cell, and the runaway the rate probe measured is population
  growth of that kind; the shell brake limits EXTENT, not count, and is blind in the co-located regime the
  reference census lives in for its first two frames.  The `|J|` ceiling (candidate (ii)) is measured inert in
  the same census (`J = sum r x m` vanishes when every `r = 0`; `max_Jmag2 = 0` at every frame, frame 3
  included), so a rule that limits by COUNTING members remains the only route to a size-dependent loss for a
  co-located island -- not exotic (the model already selects populations with a count: `<= 2`, one group of 2
  per charge word) but one step from a DECLARED knob, the `L/3` lesson: any such rule must register its
  prediction before it is run and must be checked against the trap that closed the old route (the selected
  population may not track the seed multiplicity).  Implementation note: the first placement put the function
  INSIDE the `SURFACE_ESCAPE_FSM` guard and the census build caught it as `error C3861: applyShellRelease:
  identifier not found`; it now sits outside that guard, and the reference pre-processed output is
  re-verified byte-identical to `HEAD`.  Third probe arm `rb_shell.exe`, census arm
  `build_island_census_parent_shell.bat`; written up in `experiments/PARENT_SELECTIVE.md` (the "Brake (iii)"
  section, plus the updated diagnostics and reproduce blocks).
- **2026-09-19 -- Pre-registered before the run: the extended `L=9` census (`shell` vs `repair`, 10 frames)
  to test whether the shell brake's size limit is a SELECTED PLATEAU or only a delay.**  Frames 0-4 measured
  `max_pop = 33 -> 24 -> 18` with the brake on (against `33` without it at frame 3), so the open question is
  what the limit converges to.  Three registered outcomes: **P1** size-selected plateau (`shell`'s `max_pop`
  holds a value `> 2` for at least three consecutive frames while `repair` stays far above it -- that would
  be the first size-selected plateau of the campaign, and the value becomes a candidate `N*` whose
  `L`-dependence can then be tested); **P2** no brake (`shell` and `repair` converge, or fall together to
  `max_pop = 2` or 1); **P3** collapse by shedding (`shell` falls to 2 while `repair` stays large -- the
  brake is then a hard cap, not a balance).  **Registered prediction: P2 or P3, not P1** -- the "extent, not
  count" finding says the brake acts on a geometric quantity that the reference geometry does not maintain,
  so a plateau would require the dynamics to *regenerate* extent continuously rather than produce it once in
  the frame-2 -> 3 reorganization; if P1 appears anyway, that regeneration is the discovery, and it is
  falsifiable by the `L=15` arm (the plateau must then track `L` through `2*RMAX`).  Commands:
  `build\island_census_parent\island_census_parent_shell.exe 10 16384 build\island_census_parent\shell_run10`
  and the same for `..._repair.exe` into `repair_run10`; the CSVs land in
  `build\island_census_parent\*_run10\census.csv`.  Written up in `experiments/PARENT_SELECTIVE.md` (the
  "Brake (iii)" section, pre-registration block).

- **2026-09-19 -- Ten-frame `L=9` census: the pre-registered prediction (P2/P3) is falsified in letter --
  P1 appeared -- but the spectrum shows the plateau is CONDENSATION, not a selected quantum; the brake buys
  DYNAMICS, not quantisation.**  Runs: `island_census_parent_shell.exe 10 16384 ...shell_run10` and
  `..._repair.exe 10 16384 ...repair_run10`, both reproducing frames 0-4 of the shorter runs exactly.
  **Measured `max_pop`:** `shell` 24, 18, 23, 26, 24, **26, 26, 26** for frames 3-10; `repair` 33 at every
  frame from 3 on and **frozen** from frame 4 -- seven consecutive frames with exactly zero captures,
  escapes, births and deaths -- while `shell` keeps turning over (releases 3-72 per frame edge, `max_dist`
  10-12 at every edge).  The registered P1 condition ("`max_pop` holds a value > 2 for at least three
  consecutive frames while `repair` stays far above it") is therefore MET, so my prediction of P2/P3 is
  wrong, and **the brake keeps the cubic census out of the frozen state, which no other candidate has
  done.**  **But the size spectrum rules out the reading that this is a selected quantum:** at frame 10
  `shell` is `1:1 2:2 3:7 6:1 22:1 26:7` -- seven islands of exactly 26 plus debris -- with
  `spread(max-min) = 24`, whereas `repair` is `30:7 33:1` with `spread = 3`.  **By the operational reading
  of the phenomenon (same charge, approximately equal sizes, one island per charge word) the FROZEN build
  is more quantised than the brake build**; what it lacks is movement, its equality being static.  Both
  builds condense to one island per charge word (`8 of 8 reachable words`, `mixed_charge = 0`) at sizes of
  order `243/8 ~ 30`, and the brake converts a static condensate into a dynamic one plus debris
  (`7 x 26 + 22 + 9 small = 243`), so 26 is the condensation level after shedding, not a preferred quantum.
  **Verdict for brake (iii): it buys dynamics, not quantisation.**  **Next, registered before running:**
  the `L=15` arm (675 sources, the same 8 charge words, `2*RMAX` 8 -> 14) decides whether the mode tracks
  `W/8` (condensation, and the campaign's size limit is geometric bookkeeping) or `2*RMAX` (the brake
  selects EXTENT, and the plateau is a real if not yet charged quantum).  Written up in
  `experiments/PARENT_SELECTIVE.md` (new section with the table, the spectrum, and the `L=15` registration).

- **2026-09-19 -- Cheap discriminator EXECUTED: the shell brake reads EXTENT (`2*RMAX`), not the member
  COUNT.**  New probe mode `island_rate_probe range LY DIST FRAMES CSV` in a fourth build arm
  (`rb_shellonly.exe`: completed rule set + the brake, WITHOUT `SURFACE_ESCAPE_FSM`, so the shell release is
  the only release channel).  Fixture: two tubes whose cross-section doubles the contact range -- `5x5`
  (`RMAX = 2`, range 4) and `9x9` (`RMAX = 4`, range 8) -- holding the SAME two sources at the same absolute
  torus separation (`K @1`, `D @1 + DIST`).  **Measured:** `5x5`/`DIST=6` -> `[shell] max_dist=6 releases=1`;
  `5x5`/`DIST=2` -> `releases=0`; **`9x9`/`DIST=6` -> `max_dist=6 releases=0`**; `9x9`/`DIST=12` ->
  `max_dist=12 releases=1`; with `promotions = demotions = recruits = 0` in all four cases and
  `releases(D->S)` equal to the `[shell]` count, so no other channel contributed.  **Same fixture, same member
  count and same `DIST` (rows 1 and 3) give opposite outcomes**, and the two internal controls close the
  loopholes (inside the shell at the small scale -> no release; outside it at the large scale -> release).
  **The brake therefore measures the length scale, not the population**, as the cubic `max_dist = 0` analysis
  predicted -- which is what makes the running `L=15` census a fair test: a plateau tracking the length scale
  is geometric, one tracking `W/8` is condensation.  Two fixture defects were found, fixed and recorded:
  (1) the first 4-source version was CONTAMINATED by the torus wrap -- its two "far apart" islands sat 3 cells
  apart through the wrap, fused (`[absorb] K x K encounters=16 pairs=8`) and produced a `D -> S` that looked
  like a brake release while `[shell] releases=0` said otherwise (caught by the `[absorb]` line, not the shell
  one); (2) with all three edges equal (`PX=5`, `5x5`) the tube allocator leaves `EL=0, RMAX=0` and the run
  dies in an access violation (`0xC0000005`, exit `-1073741819`), so the axis is now
  `max(2*DIST+1, 2*LY+1)`, strictly longer than the cross-section.  Written up in
  `experiments/PARENT_SELECTIVE.md` (new "cheap discriminator" section plus the reproduce block).

- **2026-09-19 -- Correction pass (author request): the `D x D` "cohesion" claim was false, four campaign
  macros were missing from the inventory, and eight anchors had DRIFTED past the construct they cite.**
  Three separate defects, all found by reading the code instead of trusting the notes.
  **(1) `D x D` same-island is a NO-OP, not cohesion.**  `encounter()` returns early for ANY equal-charge
  `D x D` pair (`if(currSrc.kind==SourceKind::D && partnerSrc.kind==SourceKind::D &&
  currSrc.ch==partnerSrc.ch) return false;`) and that guard runs BEFORE the `internal` recording that feeds
  `resolveInternalContacts()`.  No `D-D` pair therefore ever reaches `internalContacts`, so the cohesion loop
  -- whose comment read "K-D and D-D cohesion" -- only ever acts on `K-D` pairs: a delegate is held by its
  contacts with its CHIEF, not by cohesion between delegates.  Corrected in `interaction.cpp` (cohesion-loop
  comment and the `parentRepel` declaration comment), in `chief_transition.h` (the candidate block) and in
  `PARENT_SELECTIVE.md` (rule-set row now "no-op" + a correction paragraph).
  **(2) The inventory lacked the four macros this campaign created** -- `PARENT_SELECTIVE_FSM`,
  `PARENT_FUSION_ABSORB`, `PARENT_NOMINATION_REPAIR`, `PARENT_SHELL_RELEASE_FSM` -- because it was last
  audited before they existed.  Added to `doc/REFERENCE_CONFIG.md` with effect, anchors and reference
  behaviour; the inventory check now reports **49 model macros, 49 in the table, none missing either way,
  4 harness-only cited**.  (The `harness-only` row was dropped by the same edit and restored -- the check
  caught it.)
  **(3) Eight anchors had drifted**, in `CHARGE_SPECTRUM.md`, `DYNAMIC_QUANTIZATION_DERIVATION.md`,
  `PARENT_SELECTIVE.md` and `WORK_PLAN.md`.  The existence-only audit cannot see this: the line still EXISTS,
  it just says something else.  Re-mapped by meaning: `canFormPair` `210-237 -> 226-253`; pair branch
  `1652-1654 -> 1884-1886`; `chiefContact` call `1592 -> 1790`; internal early return `1598-1613 ->
  1830-1845`; escape timer `762-764 -> 770-772`; `homB = false` `2111 -> 2343`; `relocate()` `2046 -> 2278`
  and the `draft = north` slide `2067 -> 2299-2300`; the `chief_transition.h` block `14-74 -> 14-87` with T2
  `14-26 -> 14-39` (invoked `60 -> 73`), T3 `30-33 -> 43-46` (invoked `61-64 -> 74-77`), the S x S minimum
  `66-73 -> 65-71` (minimum line `70 -> 68`), the T5 return `38 -> 37-38` and the `DD_INTRA_ISLAND_FIX`
  override `52-58 -> 59-72`.  One of them was not merely displaced but WRONG in substance: no transition
  requires the same CENTRE cell -- two approaching sources share the VOXEL, and that is where `encounter()`
  is called from (`simulation.cpp:1000-1001`); the note now says that.
  **Durable fix: new versioned tool `experiments/anchor_semantic_check.ps1`** -- a table of 28
  (construct, file, line, expected text) rows that fails when a cited line no longer says what the note
  claims.  It caught four stale rows in my own first draft of the table, which is the point.
  `powershell -NoProfile -ExecutionPolicy Bypass -File experiments\anchor_semantic_check.ps1`.
  Both audits green at the end of this pass (semantic 28/0, existence 106/0); the reference pre-processed
  output is unchanged (comments only) and both build modes compile.

- **2026-09-19 -- The coupling-constant (alpha) campaign is RETIRED (author: "premature; we return to it
  once quantization and inertia are mature").**  Parked, not deleted: **125 files** moved to
  `attic/alpha_campaign/` with a `README.md` carrying the manifest, the reason and the restore recipe --
  the probe `alpha_probe.cpp` (`alpha_A..F`) with `alphaC_limit.cpp`/`alphaE_limit.cpp`, their tables, the
  notes `RESULTS.md` ("alpha probe - results") and `RESULTS_v2.md` (WP2 canonical tables), the 22
  `build_probe*.bat` builders plus `build_alphaE.bat`, the drivers `run_wp2_sweep.ps1` /
  `plot_wp2_scan.ps1` / `analyze_wp2_scan.ps1`, 23 binaries, `obj/alpha_probe.obj` and the whole
  `build/wp2` output root (the `alpha_A` L-sweep, the `S*`/`L*`/`pol*` sweep logs, the S-scan CSVs/SVGs).
  **The invariant check moved to the fingerprint.**  `run_all.bat` step 2 built `alpha_probe` and asserted
  an `alpha_A` line; it now runs `experiments\model_fingerprint.ps1` and asserts `MATCH` (sub-second,
  independent of any harness).  `doc/REFERENCE_CONFIG.md` lost the campaign recipe and the `alpha_A` quote,
  and `experiments/DATA_DEPOSIT.md` lost the campaign rows, the `RESULTS_v2` paragraph and the two
  `alpha_probe` frozen-value rows (replaced by a fingerprint row).
  **Correction to the scope I first reported:** I said the 68 `alpha_probe` references meant the other
  harnesses check the reference through it -- they do not; the count was dominated by matches of the repo
  PATH `E:\alpha`, and the only real script dependency was `run_all.bat`.  Likewise the manuscript is NOT
  part of the campaign: the paper never uses the word `alpha`, never states a coupling-constant value and
  has no `tab:campaign`; its `subsec:s2b-sweep` is the electroweak-gate demonstration (the gate opening as
  `S` falls), cited by five other passages, and it stands on the `s2B` mechanism rather than on the
  constant.  **One real cost, recorded:** the paper names `scatter_main.cpp` as the harness for
  `tab:sieve` and that file is **not in this tree**; the reproducible measurement of those rows used the
  probe that is now parked, so the table is reproducible today only from
  `attic/alpha_campaign/alpha_probe.cpp`.  That is stated in `DATA_DEPOSIT.md` rather than hidden.
  **Fingerprint note (maintenance):** the fingerprint hashes source TEXT, so the comment-only edits of the
  `D x D` correction pass moved it (`7f5eb50e...` -> `6f906df0...`); the expected value in
  `model_fingerprint.ps1` and in `REFERENCE_CONFIG.md` was updated, and behaviour equivalence is proven
  separately by preprocessing `interaction.cpp` with all candidate macros OFF and diffing with `HEAD`.
  **Documentation pass (same day, completed).**  Every active file that quoted the campaign's reference
  check now quotes the fingerprint instead (`PBSB_ISLANDS.md` x3, `FAMILY_SELECTIVE_DESIGN.md`,
  `DRIVE_PAIR_SHELL.md`, `gravity_probe_DESIGN.md`); the files that measure *other* programmes through the
  parked harness carry a "Harness note (19 Sep 2026)" at the top saying the probe is retired and the recipes
  below are the historical record (`PHOTON_MEDIATION_DESIGN.md`, `PHOTON_MEDIATION_FAR.md`);
  `stack_absorb_probe.cpp`'s four comments and `gravity_probe.cpp`'s link-stub comment were re-pointed; and
  the paper-facing maps lost the campaign (`DATA_DEPOSIT.md`: campaign rows, the `RESULTS_v2` paragraph and
  the two `alpha_probe` frozen-value rows, replaced by a fingerprint row; `CANDIDATE_VERSION.md` artifact
  list; `POSTULATE_VS_EMERGENT.md` pointers; `REVISION_PLAN.md`, where the coupling-constant deliverable is
  struck as WITHDRAWN; `RESPONSE_TO_REFEREES.md` pointers).  **The manuscript's only footprint was one
  clause** in a Status paragraph ("...and the six pre-registered coupling candidates all remain rejected");
  it was removed -- a withdrawn internal campaign does not belong in the paper -- and the PDF recompiles
  clean at 57 pages.  The campaign's own history stays in this log, deliberately: it is the record of what
  was tried, what was falsified and why it was set aside.

- **2026-09-19 -- The referee-response letter is RETIRED too (author's request).**
  `experiments/RESPONSE_TO_REFEREES.md` (14.4 kB: three simulated referee reports with point-by-point
  responses and an action list, written in WP6 on 2026-09-11) now lives in `attic/referee_response/`, with a
  README recording why and how to restore it (`git checkout HEAD -- experiments\RESPONSE_TO_REFEREES.md`).
  **It had also become inconsistent with the manuscript:** the letter argues from the coupling-constant
  campaign -- it quotes `RESULTS_v2.md`, the "six candidates for the fine-structure constant", `tab:campaign`
  and a referee's question about the `alpha_A` table -- and that campaign was withdrawn earlier the same day,
  with neither that section nor that table present in the paper.  **What changed in the active tree:**
  `README.md` (the document-inventory row is gone); `experiments/CANDIDATE_VERSION.md` (the pointer to the
  letter, **plus two claims that the revision had ADDED a `sec:campaign` / "six candidates" section and had
  stated the campaign in the abstract and Introduction -- neither of which is in the manuscript**; both are
  now marked Corrected); `experiments/BRIDGE_REVISION_IF_PROVEN.md` (item 7 had listed the letter among the
  files needing a status update; it now points at the parked copy); and this log's L/3 knock-on list no
  longer names it.  The WP6 history of writing the letter stays here deliberately.  The manuscript was not
  touched: it never cited the letter.

- **2026-09-19 -- The revision blueprint is RETIRED as well (author's request), and the log's PLAN half was
  corrected.**  `experiments/REVISION_PLAN.md` (6.5 kB, the editor-response blueprint for restructuring
  `manuscript.tex`) now lives in `attic/referee_response/` next to the retired response letter, and that
  folder's README covers both (`git checkout HEAD -- experiments\REVISION_PLAN.md` restores it).
  **Pointers fixed:** `README.md` (its inventory row is gone) and this file's own PLAN sections -- the
  header's "complements ... `REVISION_PLAN.md` / `RESULTS.md`" now names the parked copies, WP3's and WP5's
  blueprint references point at `attic/referee_response/REVISION_PLAN.md`, and the WP5 reorder pipeline no
  longer carries a "Falsification campaign" stage (nothing in the manuscript presented one).
  **WP2 is marked WITHDRAWN** with a banner -- that WP *was* the coupling-constant campaign -- and the
  diagnosis table's `alpha_A..F` row is struck, its evidence pointer now naming
  `attic/alpha_campaign/RESULTS.md`.  **Two false claims in the WP5 status paragraph were corrected in
  place:** it asserted that the abstract gained the falsification-campaign outcome and that a dedicated
  section (`sec:campaign`, `tab:campaign`) was added before Results presenting the six pre-registered
  candidates.  **Neither is in `manuscript.tex`** -- there is no `sec:campaign`, no `tab:campaign`, no
  campaign claim in the abstract and no campaign section anywhere (the campaign's only trace in the paper
  was one clause in a Status paragraph, removed earlier today).  Those two claims describe a revision state
  this tree does not contain, which is why the correction names them explicitly rather than deleting them
  quietly.

- **2026-09-19 -- The paper-facing ("claim") documents are RETIRED: five files, and the acceptance table was
  preserved first.**  `COVER_LETTER.md`, `CANDIDATE_VERSION.md`, `POSTULATE_VS_EMERGENT.md`,
  `DATA_DEPOSIT.md` and `BRIDGE_REVISION_IF_PROVEN.md` (28.5 kB together) now live in
  `attic/submission_docs/` with a README (manifest, why, restore recipe, and the warning that they cite code
  lines and manuscript labels written against older revisions -- re-anchor with
  `experiments\anchor_semantic_check.ps1` before trusting them).  **Before parking, the frozen
  expected-values table** -- the acceptance list a reproducer runs, and the one part of the deposit plan the
  active project still needs -- **moved to `doc/REFERENCE_CONFIG.md`**, section "Frozen expected values",
  with a note recording where it came from; `run_all.bat`'s step-3 comment now points there.  **References
  fixed:** `README.md` (five inventory rows), `experiments/CLAIMS_LINT.md` (its pointer to the deposit
  plan), `experiments/bridge_proven.tex` (the header comment that named the conditional-revision plan -- a
  `.tex` file, easy to miss) and this log's PLAN half (WP7's deliverables, the WP3.4 status line, the
  bridge-revision section, and the L/3 knock-on list, which now records that all five are gone).
  **A finding that justifies the removal:** the retired documents -- and, before them, the WP5 status
  paragraph -- assert five manuscript labels that **do not exist in `doc/manuscript.tex`**:
  `sec:campaign`, `tab:campaign`, `tab:postulate-emergent`, `tab:claim-evidence` and `subsec:fixed-point`.
  The tex grades claims inline (line 71) and its only tables are `tab:Cell-structure`, `tab:polar-fidelity`,
  `tab:color-combination`, `tab:scenarios`, `tab:sieve`, `tab:scaling` and `tab:charge-examples`; the
  sections that DO exist include `sec:Particles`, `sec:bridge` and `sec:dynamic-charge-quantization`.  The
  submission package therefore described a manuscript revision this tree is not, which is why it could not
  stay.  The dated entries that record *writing* these five documents are kept as history.

- **2026-09-19 -- "Keep only the essential": 89 files of closed lineages retired into
  `attic/closed_programmes/` (+ its README).**  The rule applied: the active tree keeps the model and its
  build, `doc/` (manuscript, reference config, bibliography), the repo `README.md`, this log, `quantization/`,
  and in `experiments/` the notes and instruments of the **two programmes that must mature** (island
  aggregation/quantization, inertia), the J/invariant chain, the transport (winding) instrument, the test
  suite and the two repo tools.  **Parked:** 30 notes (EM/photon/orphan: `PHOTON_MEDIATION_*`, `PBSB_ISLANDS`,
  `DRIVE_PAIR_SHELL`, `MF_CAMPAIGN_SUMMARY`, `PREDICTED_NS_RULE`, `SIEVE_SWEEP_PRODUCTION`; gravity:
  `gravity_probe_DESIGN`; colour/election/cohesion: `COLOR_*`, `FIRST_ELECTION`, `ISLAND_SEED`,
  `SEED_ASYMMETRY`, `KK_CLASH`, `DD_PROMOTION_AUDIT`, `FAMILY_SELECTIVE_DESIGN`; falsified size selection:
  `EMERGENCE_SEARCH`, `GEOMETRIC_QUANTUM`, `ABLATION_81x3`, `LIGHTMATTER_DECOUPLE`, `SPACING_PROBE`,
  `TURNOVER_ABLATION`, `COAGULATION_RANDOM`, `AGGREGATION_REVIEW`; paper tooling: `CLAIMS_LINT`), 9 harnesses
  (+ their binaries), 12 scripts and 25 build scripts.  **Integrity work, so nothing kept broke:** (i) the
  build scripts that matched the parked names cite **macro** names (`COLOR_*_FSM`, `DD_PROMOTION_FSM`,
  `KK_CLASH_FSM`, `ISLAND_SEED_EXPERIMENT`), not files -- verified, no functional break; (ii) every file
  reference in the kept documents was re-pointed to `attic/closed_programmes/`; (iii) `README.md` lost 14
  inventory rows -- 11 for the closed programmes plus **3 whose target had already been parked in the earlier
  pass**, found by testing each row's cited path for existence; (iv) `Makefile`'s `check-claims` target now
  prints a pointer instead of failing on a missing script; (v) the frozen expected-values table in
  `REFERENCE_CONFIG.md` dropped its two rows for parked analyzers (`analyze_geometric_quantum.py`,
  `analyze_sieve_sweep.py`).  **Active set now:** 12 notes, 17 harnesses, 11 scripts, 19 build scripts; both
  audits and the fingerprint stay green (semantic 28/0, inventory 49/49, `MATCH`).

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
  read zero because `homB` is cleared at the end of every frame
  (`interaction.cpp:2352`, `draft.homB = false;`), so the effect must be sampled *within* the
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
  (A) `relocate()` (interaction.cpp:2287; the `draft = north; draft.c[0]--` slide at `:2308-2309`), which
  slides the whole state pattern through the lattice keeping the address -- this
  is the archived CUDA kernel's actual transport, and `MEAN_V` cannot see it;
  (B) `applyMomentum()` (simulation.cpp:423), which translates a source centre and
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
  sign convention (`simulation.cpp:377-378`) is therefore not a nicety but a
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
- **2026-09-12 -- MILESTONE: the 9L quantised islands exist, stationary, one per
  site.**  Following the directive to park the sophisticated machinery and first
  PROVE the islands, three core-rules builds were run on a prepared seed
  (`PLACED_FAMILY_SEED`, `initSim.cpp::initCenters`): island `i` of the model's own
  partition (`ISLAND_COUNT = 9*EL`, `ISLAND_SIZE = W/(9*EL) = L/3`) is placed at its
  own site on a flat `EL x EL` grid.  At L=9/W=243:
  - placed, core only: frame 2 `K=235 D=8`, centres 81, groups 235, max_pop 2;
  - + `DD_INTRA_ISLAND_FIX`: `K=227 D=16`, centres 81, groups 227, max_pop 3;
  - + `ISLAND_ALIGNED_W_ROTATION`: **`K=81 D=162`, centres 81, groups 81,
    unresolved 0, max_pop 3 = L/3, max_span 0, max_families 1.**
  All three are static (frames 2--6 identical, captures = escapes = 0) and all three
  show the seed itself at frames 0--1 (`S=243`, centres 81).  So the quantised
  island -- `1 K + 2 D`, population `L/3`, one per site, at rest -- is achieved
  exactly, as predicted before the run.
  Diagnosis recorded: the reference schedule rotates the partner lattice by ONE
  slice per frame (`rotatePartners()`, `utils.cpp`; "cross-layer adjacency is owned
  by rotatePartners()'s rotation schedule, not by spatial geometry",
  `simulation.cpp:87-89`), so at `ISLAND_SIZE = 3` two of every three pairings are
  between different islands and the election cascades on the W address; and the
  identity structure does not depend on positions at all (the superposed and the
  placed seed give identical K/D counts).  Rotating cyclically within each
  `ISLAND_SIZE` block fixes it.  New macro `ISLAND_ALIGNED_W_ROTATION` + build
  `experiments/build_island_census_rot.bat`; reference re-verified untouched after
  the hot-path change: `6468/0/0`, `alpha_A = 0.003756878`.
  Also recorded: at L=9 `RMAX = L/2 = CENTER` so at the turnaround any two bubbles
  are mutually in contact -- "separate islands" can only mean distinct CENTRES.
  Graded (C): prepared seed + two candidate rules; the FORMATION question (canonical
  superposed seed) is untouched.  A 20-journey persistence run was launched.
- **2026-09-12 -- PERSISTENCE RESULT: the quantised state is the ABSORBING state.**
  `build/island_census_rot/run20`, 20 journeys = 2.5 complete breathing cycles of
  `2*RMAX = 8` frames.  Frames 2--20 are **bit-identical**: `K=81, D=162,
  distinct_centers=81, groups=81, unresolved=0, max_population=3 = L/3, max_span=0,
  max_families=1`, with `captures=243` only at frame 2 (the election) and
  `captures=escapes=births=deaths=0` thereafter.  Report card:
  `SUMMARY frames=20 chiefs_total=81 chiefs_at_end=81 stable>=5frames=81
  localized_at_end=81 pop==L/3_at_end=81` -- **every metric is 81**, against the
  reference's 64-journey outcome (`chiefs_at_end=235`, `pop==L/3=0`,
  `localized=8`).  So the model's own prediction -- `9L` islands of `1K + nD` with
  `n = L/3 - 1`, localized and persistent -- is now satisfied exactly in this
  construction: 81 islands, each `1K + 2D`, each at its own site, at rest, absorbing.
  No drive pairs, no inertia, no transport and none of the advanced-channel macros
  are involved.  What it took: (i) a PREPARED seed (one site per island),
  (ii) `DD_INTRA_ISLAND_FIX` (the model's own `isIslandChief` convention in the
  election) and (iii) `ISLAND_ALIGNED_W_ROTATION` (the cross-layer schedule aligned
  with the island partition -- the decisive one).  Graded (C).  Independent audit:
  `build/island_census_rot/run20/analysis.txt` (helper + `analyze_island_census.py`).
  **Still open: FORMATION from the canonical superposed seed** -- the islands now
  demonstrably exist and persist; making the single-point seed reach them is the
  remaining question, and it is now a well-posed one because the target state is
  fully characterised.
- **2026-09-12 -- THE FORMATION RULE IS FOUND: "the address supplies the place".**
  `ADDRESS_TARGET_FSM` (+ `DD_INTRA_ISLAND_FIX` + `ISLAND_ALIGNED_W_ROTATION`) run on
  the ORDINARY canonical superposed seed (all 243 layers born at one point; no placed
  seed, no drive pairs, no inertia, no host-table read).  The rule: island
  `i = w / ISLAND_SIZE` walks its centre to the site `(i % EL, (i / EL) % EL, CENTER)`,
  one lattice cell per light frame, reading only `lcenters[w]` and its own `x[3]` --
  every layer of an island computes the same target from the same address, so the
  copies walk in lockstep and arrive whole; the walk stops at the target, so the
  absorbing quantised state is its fixed point.  Measured (`build/island_census_addr`,
  20 journeys): journey 1 `centres=5`; journey 2 the election gives the quantisation
  (`K=81, D=162, groups=81, unresolved=0, max_pop=3 = L/3, span=0`) with
  `centres=13`; then the centres fan out monotonically `25, 41, 57, 69, 77` and reach
  **`centres = 81` at journey 8** (one breathing cycle), staying there (frames 8--10
  identical, zero events).  The control -- the same build WITHOUT the walk -- gives the
  same quantisation (`K=81, D=162, groups=81, unresolved=0, pop=3, span=0`) but
  `distinct_centers = 1` in every frame, which isolates the two effects: the aligned
  election supplies the QUANTISATION (already on the canonical seed), the new rule
  supplies the SEPARATION, and it does not disturb the quantisation.
  New macro `ADDRESS_TARGET_FSM` + builds `experiments/build_island_census_addr.bat`
  and `..._align.bat` (control); docs in `experiments/ISLAND_CENSUS.md`.  Graded (C):
  the W index is read as a position label -- exactly the deterministic asymmetry the
  SEED_ASYMMETRY note found missing.  Reference untouched (macros OFF).
- **2026-09-12 -- ADDRESS_TARGET_FSM made strictly cell-local, verified bit-identical.**
  The rule now reads only the cell's own fields -- its address `old.x[3]` and its
  coordinates `old.x[0..2]` -- instead of the surrounding loop's `cx,cy,cz` (which are
  the `lcenters[w]` entry for this same layer).  Reason (raised in review): `lcenters`
  is the HOST table, and although this particular entry is not a cross-layer global
  (that was the withdrawn `FAMILY_RIGID_FSM`, which read `lcenters[f]` for the family's
  other layers), the rule should not touch the table at all.  `trackCenter()` mirrors
  the centre cell's coordinates into `lcenters`, so the two forms are equivalent in
  principle; they were run side by side on the canonical seed and the entire census
  series is **identical frame by frame** (`1, 5, 13, 25, 41, 57, 69, 77, 81, 81, ...`)
  -- which proves the table was only a mirror.  Residual caveat: the transport stage
  `applyMomentum` is host-side, exactly as in the reference (Limitations item 5).
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
  `interaction.cpp:942` is the relevant guard), raised it to **7 events in 16
  frames** at `15x9x9` with `N=3`: the ported producers do fire.
  **Finding (blocker class):** live polarization is a *phase lottery*.  The flags
  are defined by sign, `pB = (pol_u > 0)`, `sB = (pol_v > 0)`
  (`simulation.cpp:377-378`), while `reconstructPair` returns
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
  implemented as `EM_FIRST_FSM` (`interaction.cpp:1451-1473`).  A/B test of
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



- **2026-09-20 -- The log was trimmed to the live backlog plus the dated record.**  The WP0-WP7 plan half
  (executive summary, the diagnosis table, WP0-WP7, the WP4 detail, the decision gates, the manuscript edit
  plan, risks and the recommended sequence) described work that had been executed, withdrawn or superseded,
  and it named documents and harnesses now in `attic/`.  It was replaced by a **Live backlog** of nine
  numbered items in four groups -- A island aggregation/quantization, B inertia/transport, C the registered
  `|J|`-ceiling candidate for size selection, D discipline, E known defects -- followed by the manifest of
  what was retired and where it went; the **100 dated entries are untouched**, and both the title and the
  header now say that a WP number or a retired filename met below is history.  File: 2322 -> 2082 lines.
  Two facts recorded in the backlog while trimming: the first `L=15` arm was **killed mid-frame-1** by the
  environment (it produced no result; relaunched the same night), and the one frame edge it did reach
  (`[shell] range=14 max_dist=0 releases=0`) matches `L=9` -- the canonical superposed seed is co-located at
  `L=15` too, which is why the shell brake starts inert there.


- **2026-09-20 -- A2 ANSWERED: the pair-making encounter and the cascade window, measured read-only.**  New
  `CASCADE_LOG` -- instrumentation only, no rule: it records every encounter that changes a source's role
  (tick, both addresses, both kinds, the charge word, the parent before/after) -- plus the probe
  `cascade_probe.cpp` and `build_cascade_probe.bat`.  The ledger sits at the END of `chief_transition.h`, so
  no existing anchor moved there; the single hook in `interaction.cpp` (9 lines, right after the
  `chiefContact` call) moved the six anchors below it by **+9**, which the semantic checker caught and I
  re-mapped together with the citing documents (`CHARGE_SPECTRUM.md` x2, this log x2) -- the discipline
  working as written.  **Measured on the reference path** (canonical seed, `L=9`, 2 light frames): frame 1 =
  **zero** role changes; frame 2 = 12402 of them, all between tick **879** and tick **1120**; and per charge
  word the LAST transition is a `D->K` promotion whose partner is that word's **maximum address**.  Answers:
  the cascade is a single one-frame, ~240-tick event opened by the frame edge itself; the pair is made by
  promotion, not by election (`D->K = 6084` against `S->K = 26`); and the bound is *forced* by
  `main.w < mirror.w`, because the highest-address delegate has no higher partner to meet.  Written up in
  `experiments/PAIR_MECHANISM.md`; backlog item A2 is struck, with the new sub-question recorded (why 241
  ticks; does the stopping rule hold once an island has extent).  `CASCADE_LOG` was also added to the macro
  inventory (**50/50**), the reference preprocessed output re-verified **byte-identical to `HEAD`** with the
  macro off, and both audits are green (semantic 28/0, inventory 50/50).

- **2026-09-20 -- A2 sub-question answered: the cascade duration tracks W, not the volume.**  Reran the
  same read-only probe at `L=12` (`W=432`, `RMAX=6`, `FRAME=1504`, `ISLAND_SIZE=4`, `ISLAND_COUNT=108`) with
  no code change: frame 1 again has **zero** role changes, and the cascade runs ticks **1505 -> 1935**, i.e.
  **T = 431 = W-1**, against **T = 242 = W-1** at `L=9` (`T/W` = 0.996 and 0.997).  The cascade is therefore
  one **partner-rotation period** (one `rotatePartners()` slice per light frame, `W` slices per rotation):
  its length is set by the number of layers, not by the number of sources, the lattice volume or the seed's
  family multiplicity -- which is also why it starts on the first tick of a frame (the edge advances a
  slice).  Two further facts: the `S x S` election channel drops to **zero** now (26 at `L=9`), so chiefly
  structure is built by the `D->K` promotion cascade (11024 notes, fed by 11232 `S->D` adoptions); and
  `K->D = 0` again.  Recorded in `experiments/PAIR_MECHANISM.md` (new "Scaling of the cascade duration T"
  section) and in backlog item A2.  Still open: why `W-1` rather than `W`, and whether the law survives once
  islands have extent.

- **2026-09-20 -- L=15 confirmation launched; census run stopped to free the machine (relaunch needed).**
  The `L=15` probe (`T = 674` predicted, cascade ticks 2286 -> 2959) is running as
  `build\cascade_probe\cascade_probe.exe 2 16384 15`, plus a sieve sensitivity control
  (`... 2 4096 15`).  Note for the environment: background processes advance only ~30 s of CPU per command
  window, so the `L=15` run needs ~30 more windows to reach its second frame edge; the prediction is
  registered above so the run is a confirmation, not an exploration.  The `shell_L15` census
  (`build\island_census_parent\shell_L15\`) was **stopped** at frame 2 to give the probe the machine; it had
  committed frames 0-1 (`frame 1: 0 K, 0 D, 675 S, 1 centre` -- no structure yet at that size, matching
  `L=9`) and **must be relaunched** for backlog item A1.


