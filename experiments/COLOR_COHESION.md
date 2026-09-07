# Provisional contact cohesion

The subsequent [reciprocal matching candidate](COLOR_MATCHED.md) bounds
movement and separates attraction from color kicks. Its test returned to
complete superposition, which is admissible in the reduced resting system.
The specific inertial P x K and P x D tests are documented in INERTIA_MATCHED.md.

Build: `experiments\build_color_cohesion.bat`.
Run from the repository root: `build\color_cohesion\color_fsm.exe 4`.
Output: `build/color_cohesion/`. The chief-only baseline remains separate.

This variant adds an experimental mechanical contribution to c for existing
K/D constituents with the same chief. Eligibility uses the roles at the start
of the encounter journey, so newly elected constituents first participate in
the following journey. Both local participants must be active, have positive
radius, and have equal full charge. Affinity does not gate cohesion.

For each contacting partner and each spatial axis, a constituent records one
step toward that partner when their shortest toroidal separation on that axis
exceeds one cell. Exact half-period ties are ignored. Axes are treated equally;
no clock, scan order, or preferred axis breaks ties. The reciprocal encounter
must independently record the opposite step. Coincident or adjacent positions
receive no attraction on the relevant axis. This is a proposed multi-axis
rule, not the older resolver's matched, single-face-step rule.

Six per-voxel contact sets encode positive and negative contributions on each
axis. They propagate by synchronous neighbor union, and each partner counts
once per axis per journey. Each voxel adds its received mechanical contributions
to the persistent color impulse in c. No global matching, movement override,
normalization, or switch disables color spreading. Summing many contacts can
overshoot: reduction of pairwise separation in isolation does not guarantee
contraction of a many-body group under synchronous motion.

The existing transport consumes c through neighbor pulls. The harness checks
reciprocal mechanical contributions and reconstructs total c independently from
the received contact sets. contacts.csv now contains the combined signed
contribution; trajectories.csv likewise contains total impulses. groups.csv
retains population, mixed-family, and spatial extent diagnostics.

## Scope and limitations

The direction calculation reads the reference source-center cache during
encounter. It does not write the partner's draft, but this remote geometry
read is not a local CA realization. A local transport of geometric information
remains necessary, as do the inherited phase/reissue improvements. Added state
is six W-bit sets per voxel plus double buffering; this is an experimental
state budget, not an established axiom. No manuscript rule is promoted from
this test alone, and no maximum physical velocity follows from modular c.

The chief election, seed, and permanent color rule remain unchanged. This
experiment tests attraction among already associated constituents; it does
not establish capture/escape balance, inertial motion, or a target island size.

## Four-frame result

The run completed 3512 ticks with exit code zero. Independent trajectory
analysis also passed (`python experiments/analyze_color_fsm.py --directory
build/color_cohesion`). All recorded contacts had reciprocals; mechanical
contributions were checked individually against their reverse contacts, and
the combined signed impulse summed to zero on every axis in every journey.

| Frame | Centers | K | D | Moving sources | Directed contacts |
| --- | --- | --- | --- | --- | --- |
| 1 | 1 | 0 | 0 | 0 | 0 |
| 2 | 19 | 8 | 235 | 220 | 7146 |
| 3 | 33 | 8 | 235 | 176 | 4296 |
| 4 | 33 | 8 | 235 | 192 | 3892 |

Cohesion contributed nonzero impulses to 2210 directed contacts in journey 3
and 1634 in journey 4. It therefore changes motion: the chief-only baseline
had 192 and 212 moving sources in those journeys, respectively. Both runs
nevertheless retained 33 occupied centers. At frame 4, all eight groups had
maximum Manhattan distance 12 from their chiefs, versus 9 for one group and
12 for the other seven in the baseline. The groups still occupied seven or
nine sites each; no parent was unresolved.

The test establishes functioning c-mediated reciprocal attraction, but shows
no improvement in these localization measures. It does not establish cohesion
or long-term instability either. In particular, an axis contribution that
attracts an isolated pair can overshoot when summed over many simultaneous
contacts. This additive rule should remain experimental. A subsequent change
needs to control conflicting simultaneous movements while preserving reciprocal
impulses, without imposing a chief population or using global arbitration.
