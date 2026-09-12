# Simulated referee reports and point-by-point response (WP6)

Status: **draft**.  Produced in WP6 to pressure-test the manuscript before
submission.  The three reports below are written as an IJUC editor would plausibly
receive them — a scope/novelty sceptic, a causality/localism sceptic, and a
numerics sceptic — each followed by the point-by-point response and the resulting
action item(s).

Reference: `doc/manuscript.tex` (revision of 11 Sep 2026).  Evidence artefacts:
`experiments/RESULTS_v2.md`, `POSTULATE_VS_EMERGENT.md`, `ABLATION_81x3.md`,
`PBSB_ISLANDS.md`, `WORK_PLAN.md`; frozen model `doc/REFERENCE_CONFIG.md`;
reproducibility `run_all.bat`, `experiments/model_fingerprint.ps1`.

---

## Referee 1 — scope and novelty

> "This is an extended exercise in digital physics with no contact with
> observation, and I cannot identify a scientific contribution.  The model
> reproduces no measured number — the authors themselves report that all six
> attempts at the fine-structure constant fail.  The particle taxonomy (fermion,
> boson, photon, graviton, Higgs) is analogy dressed as physics.  Claims about
> gravity as a residual electromagnetic effect, black holes, and superdeterminism
> are speculative and unbacked.  Recommendation: reject, or cut to a short note
> reporting the negative result."

**Response.**  The reviewer is right that the paper does not reproduce measured
physics, and the revision states this from the outset rather than defending it.
The contribution is deliberately reframed as **(a) a fully specified discrete toy
universe and (b) a pre-registered falsification campaign whose negative results
are the deliverable**.  Concretely:

- The abstract and Introduction now say the quantitative core is a pre-registered
  campaign in which six candidates for alpha all fail, and that the negatives are
  reported as results (Sect. the campaign, `tab:campaign`).
- Every substantive claim carries a **(P)ostulate / (M)easured / (C)andidate**
  grade, and a claim-evidence map (`tab:claim-evidence`) links each claim to a
  reproducible script.
- The particle taxonomy is now an **appendix** (App. Particles), opened with "the
  assignment is a naming taxonomy; none of these objects has been produced or
  measured by the current implementation".  The quantum-formalism bridge is
  likewise an appendix labelled Status (C).
- The "prescribed vs emergent" question is answered explicitly in
  `tab:postulate-emergent`: only charge conservation and the observer's reading
  of c are emergent; the clock and the polarization channel are postulates.

**Actions.**  (1) Title/abstract foreground the falsification framing — done.
(2) No residual claim of reproducing physics — checked.  (3) Taxonomy and QM
bridge confined to appendices with Status (C) — done.

---

## Referee 2 — causality, non-locality and the no-signalling claim

> "The model is non-local: the 'encounter' sweep and the whole-layer translations
> are global operations.  The no-signalling 'proposition' assumes what it must
> prove — it posits a readability split between readable fields and hidden state
> and then shows the hidden state cannot signal.  The implementation is a
> host-side scheduler, not the idealised local rule the proposition analyses."

**Response.**  This is the correct reading, and the manuscript agrees with it
rather than hiding it.  The proposition is **explicitly conditional** on the
readability split, which is stated as a **postulate of the model, not a theorem
of the implementation** (the three remarks after the proposition).  The paper
distinguishes three layers: the idealised local rule (where the proposition
holds), the coordinated updates (superluminal in coordinates, not steerable), and
the host-side scheduler (whole-layer translation, source snapshots), which is
**not** the idealised transition and is flagged as open work at the point of the
claim and again in Limitations.  The encounter and the whole-layer momentum
application are named as non-local; the text states the implementation "must not
be described as a verified strictly local, integer-only transition".

**Actions.**  (4) Confirm the host-scheduler caveat appears at the point of the
no-signalling claim and in Limitations — done.  (5) No claim of a verified local
realisation remains — checked.

---

## Referee 3 — numbers, statistics and reproducibility

> "The numbers are unstable across versions (6,996 vs 6,468).  The alpha_A
> 'candidate' is L-dependent and is presented without error bars or multiple
> seeds.  The simulations are small (L <= 21) and single-seed, so none of the
> tables is statistically meaningful."

**Response.**  Three separate points:

- **Version drift.**  It is real and now documented and controlled.  The historical
  numbers are retained only as provenance; the current tables are re-run and
  frozen against a **MODEL_VERSION** (`model-ref-v1`) with a source **fingerprint**
  check (`experiments/model_fingerprint.ps1`) and a one-command reproducibility
  script (`run_all.bat`).  `experiments/RESULTS_v2.md` is the canonical table set,
  and the sieve table `tab:sieve` was re-run at the current kernel (the text is
  aligned).
- **"No error bars / seeds".**  The reference dynamics are **deterministic** (no
  RNG on the reference path): repeating a run is byte-identical except for the
  printed wall-clock line, so repeated seeds would give identical numbers and zero
  variance — not a meaningful error bar.  The meaningful dispersion is over the
  **swept parameter S** (the sieve modulus); the S-scan is reported and is a
  non-monotone resonance comb that is not stationary in run length — itself a
  result (no single S is physical).
- **Small L.**  Accepted as a scope limit and stated in Limitations; the
  L-dependence of alpha_A is exactly why the candidate is reported as a
  **failure**, not as a derivation.

**Actions.**  (6) State the determinism / no-seeds point explicitly in the Results
(as a short note) so the referee does not read the absence of error bars as an
oversight.  (7) Keep the per-table pointers to `RESULTS_v2.md`.

---

## Consolidated action list

| # | Action | Status |
|---|---|---|
| 1 | Foreground the falsification framing in title/abstract/Intro | done |
| 2 | Taxonomy + QM bridge to appendices with Status (C) | done |
| 3 | Claim-evidence map and postulate-vs-emergent table | done |
| 4 | Host-scheduler caveat at the no-signalling claim and in Limitations | done |
| 5 | Freeze numbers vs MODEL_VERSION + fingerprint | done |
| 6 | Add an explicit determinism / no-seeds note in the Results | done |
| 7 | Align all tables to the re-run values (e.g. `tab:sieve`) | done |

All seven actions are satisfied (six by the WP5 revision, the seventh by this WP6
pass, implemented in the campaign section and verified with a clean two-pass
`pdflatex`).


