# Coagulation with temporary random reemission

The author confirmed equal charge as the only pair-specific condition for
D x D promotion. The restored variant is built with build_coagulation_dd.bat
and writes to build/coagulation_dd/seed_1. See DD_PROMOTION_AUDIT.md.
The original build below remains a min-only baseline; its eight-chief result
diagnoses an omission, not a limitation of the intended model.

Build: `experiments\build_coagulation_random.bat`.
Run from the repository root: `build\coagulation_random\coagulation_random.exe 1`.
The argument is the random seed. Outputs are isolated in
`build/coagulation_random/seed_1/`; other seeds use separate directories.
Rerunning a seed overwrites its diagnostic files.

After a completed run, `python experiments\analyze_coagulation_random.py 1`
independently reconstructs the overlap selections and random sequence,
verifies actual destinations and preserved identities, and writes analysis.json.

The experiment uses the superposed canonical L=9, W=243 seed and the
COLOR_MATCHED election/cohesion FSM. P and m are absent throughout; inertial
and electromagnetic light-matter interactions are not enabled.

## Explicit temporary intervention

Four complete light-frame journeys run first. At each frame boundary, the
observer identifies the lowest-W coincident partner of each source. A source
qualifies as persistently superposed after sharing its center with that same
partner at three consecutive boundary samples, even if they moved together.
This is a sampled persistence criterion, not proof of uninterrupted overlap
between samples.

During journeys 5 through 8, each qualifying source receives an independently
drawn nonzero displacement modulo the torus. Each axis is uniform in 0..L-1;
the all-zero vector is rejected. SplitMix64 with fixed-width arithmetic and
rejection sampling makes the sequence reproducible for the given seed.
No target site, chief count, family partition, or population is prescribed.
Collisions between randomly chosen destinations remain possible.

The observer replaces c for the selected layer at the end of diffusion,
before translation. It does not assign center positions or reset roles,
parents, charge, affinity, m, or wave fields. The normal transport consumes c
and triggers reissue for layers that moved. Natural c and forced c are logged
separately. An imposed displacement can break mechanical reciprocity: this
is intentional external forcing, not a claimed conservation law.

Journeys 9 through 12 run without interventions to observe subsequent
coagulation. The scheduling counters, coincidence lookup, random generator,
and per-layer assignment are an expressly authorized test apparatus, not
candidate local physical rules. Signed offsets use the existing modular
transport; neither path length nor physical speed is inferred from them.

## Measurements and structural limitation

trace.csv reports K/D/S, distinct source and chief centers, groups with L/3
constituents, unresolved parents, and intervention counts. groups.csv reports
population, contributing seed families, occupied sites, and maximum distance
to the chief. members.csv records each immutable W identity and its actual
chief association at every frame boundary. interventions.csv records the
overlap witness, persistence, natural c, and forced c.

The harness verifies preserved identities and absence of P/m; it checks the
actual final displacement against the c consumed by transport. Source
uniqueness and uniform translation fields are also checked by the kernel.

The existing election has an important limitation that random displacement
does not remove: once all S have elected chiefs, K proposes its own W and D
proposes its current parent. Subsequent minimum propagation can only select
an already represented identity. Without the missing D x D promotion, it cannot create a new chief for a fragment
that separates spatially. With the observed eight chiefs, changing c alone
cannot generate 81 chiefs. This is a diagnosis of the incomplete current code,
not a desired axiom or a test acceptance condition.

This experiment therefore separates two questions: whether persistent
superposition can be disrupted through reemission, and whether the current
election/cohesion rules can build the intended island population afterward.
Separated centers or fragments are not counted as additional islands without
their own K. A future chief-birth or orphan-handling rule requires explicit
design; this intervention does not silently reset D to S to obtain the target.
