# Cover letter (submission draft)

To the Editor-in-Chief
*International Journal of Unconventional Computing*

Dear Editor,

Please consider the manuscript **"It from bit: a concrete attempt"**
(Alexandre Furtado Neto) for publication in the *International Journal of
Unconventional Computing*.

## What the paper does

The manuscript specifies a deterministic cellular automaton that extends a
three-torus with a non-spatial internal dimension, and then subjects it to a
**pre-registered falsification campaign**.  Rather than claiming that the model
reproduces physics, the paper (i) defines the model completely, (ii) fixes six
candidate derivations of the fine-structure constant *before* the runs and reports
that **all six fail**, (iii) reports the controlled positives that do hold
(prepared-island transport; a family-preserving `81 x 3` exclusion control), and
(iv) confines every speculative mapping to clearly labelled appendices.

This fits the journal's scope because the contribution is about the
**computational character of a discrete physical model** — the rule set, its
information clock, a lattice causality argument, and an explicit account of what
the automaton does and does not derive — rather than about a new particle-physics
prediction.

## Revision summary

This is a substantially revised version.  Key changes (full list in
`experiments/CANDIDATE_VERSION.md`):

- Added a dedicated **falsification-campaign section** (six candidates, moved
  before Results).
- Every substantive claim is graded **(P)ostulate / (M)easured / (C)andidate**,
  with a claim-evidence map.
- Added a **postulate-vs-emergent** table that answers which ingredients are
  prescribed rather than derived.
- The particle taxonomy and the quantum-formalism bridge were moved to **labelled
  appendices**.
- All quantitative tables were **re-run** at the current kernel and frozen against
  a `MODEL_VERSION` with a source fingerprint; historical numbers are retained
  only as provenance.
- American-English standardization; the manuscript builds with **zero warnings**.

## Reproducibility and materials

The reference configuration is frozen (tag `model-ref-v1`) and verifiable with
`experiments/model_fingerprint.ps1`; `run_all.bat` reproduces the reference build
and probe.  All harness sources and result tables are in the repository; the
deposit plan is in `experiments/DATA_DEPOSIT.md`.  AI use is disclosed in the
manuscript.

The author has no conflicts of interest.

Sincerely,
Alexandre Furtado Neto
