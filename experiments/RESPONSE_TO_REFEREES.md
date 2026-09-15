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

---

# Round 2 — after the new material (ledger, theorem, P7, `L=15`)

Reference: `doc/manuscript.tex` with the new subsection
`subsec:fixed-point` and Table `tab:ledger`; evidence artefacts
`experiments/DYNAMIC_QUANTIZATION_DERIVATION.md` (theorem, ledger, P7, `L=15`),
`SIEVE_SWEEP_PRODUCTION.md`, `PHOTON_MEDIATION_FAR.md`, `ABLATION_81x3.md`.

## Referee 4 — "quantisation to 2 is trivial, and the model explains nothing"

> "Any rule that promotes one of two equal delegates caps groups at two.  Calling
> that a theorem dresses up a tautology, and it deletes the paper's only crisp
> claim (the `9L` islands of `L/3`)."

**Response.**  Three answers.  (a) The theorem is not offered as a discovery about
the world; it is the **explanation of the paper's own negatives**.  It converts
"we searched for `L/3` and did not find it" into "the reference rules cannot
produce it, and here is the exact transition that blocks it" — which is the
standard of evidence the paper sets for itself.  (b) As stated it is not a
tautology: it needs the co-location condition (measured separately by the P7
control), the exact-order clause (`main.w < mirror.w`) and the per-charge-word
bound, and it has a non-obvious consequence: the number of non-singleton groups
equals the number of charge words, i.e. `D = 8` and `K = W - 8`.  That law was
*derived and written down before* the `L=15` run and confirmed by it
(`667 + 8 = 675`), which a tautology would not survive.  (c) On "explains
nothing": the manuscript now claims exactly what is supported — the charge
*spectrum* (the unit in thirds, with the sector and colour structure), the
conservation law and the pair algebra.  The unit itself is declared an axiom, the
same position as hypercharge normalisation in the Standard Model.  A referee who
judges an axiom-only statement insufficient will find the paper's own Limitations
list in agreement.

**Actions.**  (8) State in the subsection that the theorem is the explanation of
the negatives — done.  (9) Report the `K = W - 8`, `D = 8` law and note it was
predicted before the `L=15` run — done (subsection text and section 9 of the
derivation note).

## Referee 5 — "4 frames at `L=15`, 12 at `L=9`, is not a stability measurement"

> "The quantised state is sampled for a handful of journeys.  The paper's own
> earlier work showed a plateau breaking at journey 7, so a 4-frame run cannot
> exclude a later change."

**Response.**  The argument is in the ledger: from the second journey the counts
of captures, escapes, births and deaths are **identically zero** in the reference
build, so the census observables are frozen *unless a new channel becomes live*.
The one channel known to switch on later is the broadcast election stamp at
journey 7; the low-`S` run at `L=9` covers 12 journeys, crosses journey 7, and
shows no change (`145 K + 5 D`, maximum population 2, zero events).  The
reference build at `L=9` was carried to **64 journeys** by the earlier campaign
with the same conclusion.  Additional frames at `L=15` would cost ~31 min each
and cannot alter the reported quantities; the run is scripted (2 h for four
frames) if a referee wants the transient at that size as well.

**Actions.**  (10) Make the "zero events ⇒ frozen observables" argument explicit
wherever frame counts are quoted — done (subsection and ledger caption).

## Referee 6 — "you retracted a census result before; why trust these numbers?"

> "The same log contains a withdrawn result (`79 +- 2` islands) attributed to a
> host-level artifact.  That is a red flag about the apparatus, not a badge."

**Response.**  The retraction is part of the record and is precisely why the new
numbers carry weight: (i) the artifact was diagnosed (a rule reading the host's
per-layer table across a family in one tick), the rule was withdrawn, and a purely
cell-local re-formulation was shown to be bit-identical — the diagnosis is in the
same note, not hidden; (ii) a second defect (`unresolved` delegates not counted)
is documented with its fix; (iii) the census now cross-checks an **independent**
instrumentation every frame and prints `PASS census recorded; independent chief
counts agree`; (iv) group histograms come from a separate CSV and were verified
twice independently (`659` singleton + `8` double groups at `L=15`; `227 + 8` at
`L=9`); (v) the `L=15` state was **predicted** from the theorem before the run.
An apparatus that retracts its own artefacts and then predicts a new number is
stronger evidence than one that has never been checked.

**Actions.**  (11) Cite the two audits and the invariant check wherever the ledger
is quoted — done (derivation note sections 2-3, manuscript Limitations).

## Referee 7 — "P7 plants what it wants to see"

> "The probe plants a `1K+2D` island and then reports that the island changed.
> An author who chooses the initial condition can choose the outcome."

**Response.**  P7 does not assume the transition; it plants the *same roles twice*
and varies one geometric quantity with the rule set held fixed.  The observable
difference is in the **contact counter** (zero in every frame beyond the contact
range, versus the periodic 156/396 sequence inside it), which is a property of the
wavefront geometry and of the per-cell contact gate, not of the roles.  The
outcome difference (one promotion versus none) follows from the contacts.  The
rule-level isolation is the second control: with `DD_INTRA_ISLAND_FIX` the
contacts are **identical** and the promotion is **absent**, so the change lives in
the rule, not in the measurement.  A third element closes the loop with the
canonical (unprepared) seed: the production census at `L=9` and `L=15` reaches the
same bound from a fully superposed initial condition.

**Actions.**  (12) Present P7 as a geometry control plus a rule control, and cite
the canonical-seed agreement — done (subsection Scope paragraph, derivation note
section 8).

## Consolidated action list — round 2

| # | Action | Status |
|---|---|---|
| 8 | State that the theorem explains the paper's negatives | done |
| 9 | Report the `K = W - 8`, `D = 8` law, predicted before `L=15` | done |
| 10 | Make the "zero events ⇒ frozen observables" argument explicit | done |
| 11 | Cite the two measurement audits and the invariant check | done |
| 12 | Present P7 as geometry control + rule control + canonical-seed agreement | done |
| 13 | Declare the axiom status of the charge unit in the abstract and Limitations | done |

Neither round found a claim unsupported by the artefacts.  The net effect of
rounds 1-2 is a **scope reduction** rather than a defence of the original claim:
the quantitative core of the paper is now (i) a fully specified rule set, (ii) a
pre-registered campaign that fails in six ways, (iii) a proved statement about
what the rules *do* fix (population 2, one delegate per charge word), and (iv) an
explicit axiom for the charge unit.  Revision decisions that remain for the author
are editorial, not evidential.




