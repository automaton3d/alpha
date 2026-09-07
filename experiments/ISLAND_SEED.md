# Superposed seed: reduced island experiment

After restoring D x D promotion, the runner observes two frames and requires
the first zero-radius frame to remain S. It measures mixing rather than
requiring it for this particular encounter ordering. The historical one-frame
counts below predate that correction and are retained as history only.

Build: `experiments\build_island_seed.bat`

Run: `build\island_seed\island_seed.exe`

The experiment uses the ordinary Platonic initialization on a periodic
9x9x9 grid with W=243: 81 families of three sources, all initially S and
spatially superposed. No chief, displacement, pair, or momentum is assigned
by the harness. Static family identity is inherited from initGeneral.

ISLAND_SEED_EXPERIMENT disables the encounter branches for inter-family
scattering and pair formation. Contact-driven chief election and internal
cohesion remain enabled. Geometry, wave fields, polarization and the other
FSM stages are retained. Objects are built separately in build/island_seed.
This is a reduced reference algorithm, not a certified local implementation.

The harness measures chief groups at the end of the encounter window and at
the end of the first frame, checking preserved affinity and no P. Equal-charge
sources from different seed families and affinities can elect the same chief.
It reports each chief's population, number of contributing seed families,
unresolved parents, and distinct source centers. No island count or population
is imposed. This superposed fixture must exercise cross-family election. Election does
not establish spatial separation, aggregation from dispersed sources, or
long-term stability. In particular, initially coincident constituents need
not move under internal cohesion alone.

Historical result before removing the affinity gate: at tick 243 (end of encounter) and tick 878
(end of frame), K=81, D=162, S=0, with exactly one distinct spatial center.
The family-election checks passed; all families remained superposed.
The run log is build/island_seed/result.log. No spatial symmetry breaking
or stability over multiple eras has been demonstrated by this run.

After removing the affinity gate, the one-frame run completed with exit code
zero (build/island_seed/cross_family.log). At tick 878 it measured 8 K, 235 D,
and no S. Chief 0 had 33 constituents from 11 seed families; chiefs 3, 6, 9,
12, 15, 18, and 21 each had 30 constituents from 10 families. All eight groups
mixed seed families, with no unresolved parent references. There was still
only one spatial center. These are chief associations, not eight demonstrated
spatial islands. The canonical seed contains eight distinct charge words;
the observed count must not be generalized into a required island count.

Initialization correction: the first harness version used the cubic
allocator without calculateParameters, leaving lcenters empty before
reading lcenters[0]. The current harness explicitly allocates storage,
calculates parameters, validates dimensions and center addresses, initializes
the seed, and replicates the buffers in that order. A printed PASS alone
does not validate a run: the executable must also exit with code zero.
