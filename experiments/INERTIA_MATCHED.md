# Prepared inertial contact experiment

Current status: see [INERTIA_REVALIDATION.md](INERTIA_REVALIDATION.md).

Historical scope: the observations below predate D x D promotion and
K x K clash activation in this build. The completed current revalidation is
reported in INERTIA_REVALIDATION.md; old PASS results do not validate the
restored role dynamics.

This adds the specific inertial P x K and P x D contacts to the matched
cohesion variant. Electromagnetic light-matter interactions are absent.
Superposition at rest is admissible; no finite resting radius is imposed.

Build: `experiments\build_inertia_matched.bat`.
Run the 96-frame suite: `experiments\test_inertia_matched.bat`.
Single case: `build\inertia_matched\inertia_matched.exe island 96`.
Audit: `python experiments\analyze_inertia_matched.py`.
Outputs, logs, and objects reside in `build/inertia_matched/`.

## Initial conditions and rules

The harness prepares either K alone or K+2D, with zero or more reciprocal P
pairs. All begin at the same center, with source radius zero and unreached
geometry elsewhere. Roles, affinities, reciprocal pair identities, and axial
m are prepared throughout each layer; chiefs and drive pairs are not formed
dynamically by this test. The lattice is a periodic 9x9x9 spatial grid with
W equal to the number of participating sources. Nothing edits simulation
state between ticks. The reduced FSM is called through simulation().

An active, positive-radius P/body encounter with matching affinity records
the drive pair's signed direction and pair identity. P/P contacts, pair
formation, and electromagnetic branches are absent. S cannot receive an
inertial kick. A pair with zero m or an invalid reciprocal association cannot
authorize transport. The two P halves are treated as one kick opportunity.

Contact payloads diffuse synchronously through six spatial neighbors. Movement
proposals then circulate through adjacent W positions. Each source accepts
at most one partner; a P/body impulse additionally requires agreement from
both reciprocal halves and the body constituent, with matching directions.
An incomplete agreement gives no kick. Neither remote draft writes nor a
global counter arbitrate this agreement. The explicit W-based proposal
priority remains a possible physical bias and has not been derived axiomatically.

An agreed K/D recipient takes a step in the sign direction of m through c.
Both P halves reemit one step toward that recipient's destination. This
recycles the pair near the aggregate; m itself is preserved. If no agreement
occurs, m alone gives no motion. Cohesion and propulsion compete through
the same proposal mechanism, rather than adding unlimited impulses.

In this variant, a c residue representing -1 is consumed with one negative
neighbor pull. It is not executed as eight positive steps. The harness counts
actual per-tick center movements and checks at most one step per axis per
journey, including torus crossings. All current probes use axial m; a physical
maximum velocity for general directions is not established.

## Verified 96-frame observations

All nine cases exited zero. The independent CSV audit verified signed
transport, negative controls, intact P pairs, and exact spatial reflection
between rightward and leftward runs. K/D identities, parent, charge, affinity,
and m were checked during the run. The first zero-radius journey gave no kick.

| Prepared case | Mean body displacement/frame | Largest unwrapped body diameter | Largest unwrapped P distance to nearest body |
| --- | --- | --- | --- |
| K+2D, no P | 0 | 0 | -- |
| K, one rightward P | +0.989583 | 0 | 0 |
| K, one leftward P | -0.989583 | 0 | 0 |
| K+2D, one rightward P | +0.256944 | 3 | 1 |
| K+2D, one leftward P | -0.256944 | 3 | 1 |
| K+2D, foreign-affinity P | 0 | 0 | 0 |
| K+2D, broken reciprocal pair | 0 | 0 | 0 |
| K+2D, P with zero m | 0 | 0 | 0 |
| K+2D, three aligned P | +0.388889 | 12 | 27 |

For K alone, the 95 positive-radius journeys each moved K and both P halves
one cell. In the one-P K+2D run, 29 grants reached K and 45 reached D. The
body remained localized in these 96 frames and the P remained nearby.

The three-P case is not a successful cohesive-transport result: unwrapped
separations expose body dispersion and a departing drive pair, despite an
apparently larger mean velocity. Torus distances alone had hidden this.
Its W and frame duration also differ from the one-P case, so these values
do not establish a quantitative population-speed law for a fixed universe.

## Remaining scope

The initial trial moved each P forward whenever it kicked a body element.
That trial let a single P separate by 45 unwrapped cells over 96 frames.
The reemission-to-recipient rule corrects that observed one-P failure, but
has not solved the multiple-P case. It should not be interpreted as evidence
that every drive pair remains bound indefinitely.

Attraction and P reemission directions still read the reference center cache.
The inherited phase and reissue paths also retain nonlocal dependencies.
Contact sets, per-partner metadata, and proposal buffers are experimental
added state. No strict local-CA realization, emergent mass law, long-term
stability, or electromagnetic behavior is claimed. Frames are FSM journeys,
not eras; velocities here are cells per frame, not a derived physical speed.
The manuscript and ordinary simulation build do not adopt this candidate.
