# Claims-vs-artifacts lint (menu item 5)

**Question.** A referee's first pass over a paper like this is mechanical: for every
quantitative claim, where is the artefact that produced it?  This note runs that pass on
`doc/manuscript.tex` and records what it finds, including the checks the tool cannot do.

**Artifacts.** `experiments/check_claims.py` (stdlib only; `numpy`/`pandas` are not installed
here), its output `build/claims_lint.txt` and the generated table
`build/claims_lint_generated.md`.  Run:

```
python experiments/check_claims.py --top 14 --md build/claims_lint_generated.md
```

## 1. Method, and what it can and cannot see

The scanner takes every sentence of the manuscript (bibliography and metadata such as
`\usepackage`/PACS lines are cut), extracts number-like tokens, drops narrative calendar
years and bit-strings shorter than four digits, and tries to link each remaining number to a
file under `experiments/`, `quantization/` or `build/`.  Three buckets:

* **STRONG** -- the number appears in an artefact *and* that artefact shares at least two
  content words with the sentence.  This is evidence of provenance, not proof of correctness.
* **WEAK** -- the number appears somewhere but the context does not match (usually a
  coincidence: a parameter value that also occurs in an unrelated log).
* **NONE** -- the number appears in no artefact at all: either a physical constant, a bit
  pattern, or a claim with no repository support.

Two deliberate exclusions: manuscript backups under `build/` and this tool's own generated
markdown, because both repeat the manuscript's numbers and would make matching circular.

**Limits.** The matcher is lexical, not semantic: bit-string notation (`000000`, `111111`),
LaTeX lengths (`0.62\linewidth`) and table rows (which the sentence joiner can concatenate,
e.g. the `711151921` token at L1073) show up as findings although they are not measurements.
Conversely a number can be "STRONG" and still be *wrong* -- strand 3 below lists the semantic
checks that a human has to make.

## 2. Automated result

| bucket | sentences |
|---|---|
| STRONG (number + shared context) | 53 |
| WEAK (number only) | 2 |
| NONE (prose-only) | 11 |

**Drift of +-1, and why.**  The first measurement of this lint, before this session's own
documentation was committed, gave 52 / 2 / 12.  Re-running after `WORK_PLAN.md` and this note
quote the manuscript's numbers moves one item (the `0.9950` correlation, quoted in the work-plan
entry) from NONE to STRONG: any repository prose that *discusses* a claim links it.  The tool
excludes only its own outputs, so the NONE bucket is a **lower bound** on claims without
artefacts, and the counts should be re-read whenever the campaign notes grow.

Of the 11 prose-only items, four classes matter; the rest are notation (bit patterns in the
algorithm listings and in Table `tab:charge-examples`) and are false positives of the lexical
method:

1. **Wavefront-metric table (`tab:scaling`, manuscript L410)** -- the whole table
   (`12.946, 8.099, 0.131`, ... ) has no artefact: no script, CSV or log in the repository
   reproduces the lattice-size scan.  Either add `experiments/wavefront_metrics.py` or state
   in the caption that the values are not reproducible from the deposit.
2. **`0.9950` (L1167)**, the `sin(r)/r` envelope correlation peak at `L = 11` -- same situation.
3. **Appendix A constants (L1547, L1555, L1592)** -- `m_p c^2 = 1.503277592969e-10 J`,
   `n_p = 1.4179573...`, `n_pT = 5.67183e92`, `O = 8.8e26 m`, `Ed = 10^80`.  Some are cited
   external inputs, but the *derived* ones (`n_p`, `n_pT`) are arithmetic and should be
   recomputed by a short script so the appendix is auditable.
4. **`5.67183`, `8.8`** also fall here for the same reason.

## 3. Manual findings (semantic checks the tool cannot make)

These came from reading the artefacts against the text during the sessions that produced items
(1)-(4), not from the scanner.

1. **`tab:scaling` names a producer that is not in the tree.**  The caption says the metrics
   were "measured by the `lorentz_mm.cpp` probe" (manuscript L1162), and the deposit lists that
   file as "(historical)" (`DATA_DEPOSIT.md`, table -> script map) -- but it exists nowhere in
   the repository.  Either add the source (and a log) or reword the caption.  This is the most
   exposed quantitative claim in the paper, because it is unreproducible *and* its producer is
   absent, while the deposit's own map presents it as covered.
2. **The 93 pair-halves of the reference ledger are no longer an open item.**  The row is reproduced
   exactly by the reference build under its own conditions (log `build/census_ref_el9_s64.txt`:
   `S = 64`, `EL = 9`, frame 2 → `K = 145, D = 5, P = 93`, `145` groups, five population-2 groups),
   so the lint item is closed by measurement; the suspicion recorded in the earlier draft of the
   item-3 note is withdrawn, and its falsifiable statement is now qualified as *reference-sieve*.
3. **The `1187` proton--electron ratio (Appendix B) is not recomputable from the table it sits
   in.**  The appendix already calls it a "scale-of-consistency coincidence"; the lint's
   recommendation is to add the counting recipe (fragment masses) to the same appendix, or to
   drop the number from the abstract-level narrative.
4. **The taxonomy's example words are planted, not produced.**  `0x00/0x1F` and `0x00/0x3F`
   (Table `tab:charge-examples` and the R1/R2 prose) are unreachable from the seed's eight
   words -- stated now in the new Reachability caveat paragraph of Appendix B.  No action
   needed beyond that caveat, but the probes that use them should keep saying they plant them.
5. **The flux-harness figure `N* = 30.38` (CI [29.87, 30.91], surrogate p = 0.005)** lives in
   `quantization/FINDINGS.md` with its script, and the same document reports that four of six
   real datasets are frozen and that no population attractor exists in the repository's data.
   The manuscript must not present that number as model output; check every mention.

## 4. Actions

| # | action | cost |
|---|---|---|
| 1 | add `lorentz_mm.cpp` (or reword the `tab:scaling` caption) | source, if it still exists |
| 2 | ~~archive the `S = 64` ledger log, or re-run and cite~~ **DONE: reproduced exactly and archived at `experiments/census_ref_el9_s64/census.csv`** | none |
| 3 | add `experiments/wavefront_metrics.py` for `tab:scaling` and the `0.9950` correlation | small script |
| 4 | add a short script for Appendix A's derived constants (`n_p`, `n_pT`) | small script |
| 5 | add the fragment-mass recipe behind `1187`, or drop the number | prose |
| 6 | run the lint before every submission and keep `build/claims_lint_generated.md` alongside the deposit | none |

## 5. Reproduce

```bat
python experiments\check_claims.py --top 14 --md build\claims_lint_generated.md
```

Buckets: STRONG 52, WEAK 2, NONE 12 (of 66 sentences carrying a claim-like number), over 815
artefact files scanned.
