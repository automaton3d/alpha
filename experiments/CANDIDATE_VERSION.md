# Candidate version — manuscript change summary (WP6)

**Manuscript:** `doc/manuscript.tex` / `doc/manuscript.pdf` (revision of 11 Sep 2026).
**Build:** `doc\latexpdf.bat` (`pdflatex` + `biber` + `pdflatex` + `pdflatex`);
compiles with **zero warnings** and no undefined citations.
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
- Merged one-line Conjectures subsections (Color + Weak quantization; Spin + the
  Hofer effect) and demoted two short model subsections (The host lattices; What
  the seed does not contain) to paragraphs.

## Claims and evidence

- Abstract and Introduction now state the **pre-registered falsification
  campaign** explicitly.
- Added the **claim-evidence map** (`tab:claim-evidence`) and the
  **postulate-vs-emergent** table (`tab:postulate-emergent`).
- Toned down unsupported claims (Novelty list, Future inclusions, Final thoughts);
  the taxonomy and the QM bridge carry an explicit Status (C).
- Added an explicit **determinism / no-seeds** note to the campaign section.

## Data and reproducibility

- **Re-ran the sieve sweep** at the current kernel and replaced `tab:sieve`
  ($S=16384 \to 6468/0/0$; pairs form for every $S \le 8192$); the dependent text
  was aligned.
- Updated the **Reproducibility** section to cite `MODEL_VERSION` `model-ref-v1`,
  `model_fingerprint.ps1`, `run_all.bat` and `RESULTS_v2.md`.

## Style

- **American English** standardization (no British spellings remain).
- Float specifiers `[h]` -> `[htbp]`: **zero build warnings**.

## Evidence artefacts

`experiments/` : `RESULTS_v2.md`, `ABLATION_81x3.md`, `POSTULATE_VS_EMERGENT.md`,
`PBSB_ISLANDS.md`, `RESPONSE_TO_REFEREES.md`, `WORK_PLAN.md`.
`doc/REFERENCE_CONFIG.md`; `run_all.bat`; `experiments/model_fingerprint.ps1`.

## Open item

**G1** (Route A vs Route B framing) is deferred.  The manuscript is currently in
the **Route A** framing: the falsification campaign is the thesis, and the
equal-charge EM repulsion (`pbsb_two_wide`, `EM_FIRST_FSM`/`EM_NOS2B_FSM`) is a
labelled **candidate** (Status C) in the claim-evidence map.
