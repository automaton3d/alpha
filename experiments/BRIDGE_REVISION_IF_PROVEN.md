# Appendix `sec:bridge` revised, under the supposition that the conjectures hold

**Status: conditional design document, not part of the submission.**  The
manuscript keeps the Status~(C) version of Sect.~\ref{sec:bridge}.  The revised
text is `experiments/bridge_proven.tex`, kept out of the build; it compiles
standalone through the scratch wrapper `build/bridge_check.tex`
(`pdflatex bridge_check.tex`: 4 pages, no errors).

The supposition of this exercise is that the model's conjectures are **proven**.
The revision therefore does not add confidence language to the existing analogy:
it *derives* the quantum formalism from the rule set, and it pays for each step by
naming the hypothesis that carries it (H1-H4), so that a referee can attack a
statement instead of a mood.

## What the revision changes

| element | Status (C) version (submission) | revised version (`bridge_proven.tex`) |
|---|---|---|
| Hilbert space | "an orthonormal set of ontological states can serve as a basis" | **Lemma 1**, with proof from the finiteness of the state space |
| step operator | "find a permutation operator" (recipe) | **Theorem 2**: the reversible fragment *is* a permutation, hence unitary, hence $H=\tfrac{i\hbar}{T}\ln U$ Hermitian |
| spectrum | not stated | **Corollary 3**: equally spaced levels, $\Delta E=2\pi\hbar/(NT)$ -- a non-uniform spectrum cannot come from $U$ alone |
| Born rule | "follows once superposition states are accepted" | **Theorem 4**: $p(C)=\sum_{A\in\pi^{-1}(C)}|\lambda_A|^2$ from the counting measure on the fibres |
| irreversibility | "the sieve, collapse and overwrite are excluded ... not reconciled" | **`bridge:arrow`**: those steps *are* the measurement channel, and their many-to-one character is the arrow of time |
| no signalling | the readability split is a postulate | **`bridge:nosignal`**: a corollary of Lemma 1 and Theorems 2 and 4 |

## The obligations this supposition buys

| # | what must be proven | evidence already in the repository | how it would be checked (cost) |
|---|---|---|---|
| **H1** | the fragment boundary: which stages are bijections (`phase_step` wave update, polarization reconstruction) and which are many-to-one (sieve, collapse-and-reissue, overwrite) | the stages are coded and named; the dissipative events already appear in the logs (`captures/escapes/births/deaths`, `annihilations`, the `s2B` gate) | enumerate the stage map on a small tube (`L=5`) and count preimages per state (hours) |
| **H2** | the fibres of the many-to-one stages are exactly the classes of information the readable fields do not expose | the readability split is written out in `subsec:nosignaling-formal` (its Setup enumerates $\Phi$ and $\Psi$) | same enumeration, grouped by $\Phi$: check $\Phi$ is constant on each fibre (hours) |
| **H3** | preparations assign equal weight inside a fibre | **nothing yet** -- a statement about preparation, not about the rule | statistical test over *distinct* initial templates (the reference dynamics are deterministic, so one template gives no statistics) |
| **H4** | the stage-wise bijection composes to the whole FSM at finite $L$ | the manuscript already restricts the unitary step to the reversible fragment and flags the rest | formal composition, then the same enumeration (hours) |
| **Cor. 3** | follows from H4 -- no extra assumption | the taxonomy's even-integer ladder ($2n$ photon modes) is already in the text | compare the enumerated spectrum of $U$ with the $2n$ ladder |
| **nosignal** | follows from H2 plus Lemmas 1-2 of `subsec:nosignaling-formal` | the proposition and its lemmas exist; they only *assume* the split | replace the postulate by H2 and delete remark (ii) |

Two honest warnings, both of which a referee would raise first:

1. **H3 cannot be proven from a deterministic rule alone.**  It is an assumption
   about preparation, so Theorem 4 is a *conditional* derivation of the Born rule
   -- strictly stronger than "the Born rule follows once superposition is
   accepted", but still not a theorem of the rule set.  The revised text says so
   in the Theorem's own paragraph.
2. **A permutation of a finite state space has a rational, equally spaced
   spectrum.**  It cannot produce the measured non-uniform spectra of real atoms
   on its own; Corollary 3 makes that explicit and assigns the non-uniformity to
   the dissipative (interacting) part.  This is a feature of the revision -- it
   turns a vague "quantum-like" claim into a sharp structural statement -- but it
   also bounds what the appendix can ever explain.

## Knock-on edits if the swap is ever made

1. `doc/manuscript.tex` `sec:bridge`: replace the Status~(C) paragraph and the
   closing caveat paragraph by `experiments/bridge_proven.tex` (the 't Hooft
   recipe itemize is superseded by Theorem 2; the references remain).
2. Introduction, section map (line 77): "The speculative bridge to the
   quantum-mechanics formalism is likewise confined to Appendix~\ref{sec:bridge}."
   becomes "The derivation of the quantum-mechanical formalism from the reversible
   fragment is given in Appendix~\ref{sec:bridge}."
3. Introduction, claims-and-status paragraph (line 71): the quantum-formalism
   mapping leaves the "(C) ... explicitly labeled conjectural" list.
4. Abstract (line 69): "The model is not offered as an interpretation of quantum
   mechanics, but as a primitive descriptive layer." would need rewording, since
   the appendix would then *derive* rather than interpret.
5. `subsec:nosignaling-formal` remark (ii): the split is no longer a postulate but
   H2; the remark is rewritten and the proposition restated as a corollary.
6. `experiments/POSTULATE_VS_EMERGENT.md`: add rows -- ontological basis and
   permutation step (M, derived); Born weights (M, derived, conditional on H3);
   equidistant spectrum (M, derived).
7. `experiments/CANDIDATE_VERSION.md`, `COVER_LETTER.md`,
   `RESPONSE_TO_REFEREES.md`: the status lines that call the bridge an appendix
   analogy, and referee 1's "the model explains nothing" answer, both change.
8. Rebuild `doc/manuscript.pdf` and check the `tab:claim-evidence` row for the
   bridge.

Nothing above has been applied: the repository remains consistent with the
Status~(C) submission, and this note exists so that the *target* is explicit if the
conjectures are ever discharged.  The obligations map also shows where the results
of the current session already bite: the dissipative stages are precisely the ones
the membership theorem and the sieve work characterise quantitatively (`T1`-`T5`,
the gate probability `P(u,S)`, the frozen corner), so **H1/H2 have a head start**,
while **H3 and the preparation statistics have no evidence at all**.  A referee
reading the revised appendix should therefore be told, in the same paragraph,
which of the four hypotheses are supported by enumerated data and which are
preparation assumptions -- exactly the split this note and the revised text make.

