# Island inertia: current CPU reference

The object under test is a prepared island of N equal-charge body elements,
one K and N-1 D, with a common affinity. Reciprocal photon-type pairs become
its propellers by sharing that affinity and carrying nonzero m. K-D and D-D
contacts maintain cohesion; P-K and P-D contacts supply internal propulsion.
Receiving a displacement never changes a body element into a propeller.

This replaces the previous inertia probe, which planted a mixed composition
of singleton sources and free pairs. Its old CSVs are historical and do not
validate this kernel. Neither harness demonstrates spontaneous electron
formation from the Platonic seed.

## Operational rules

- Source changes are staged separately from voxel updates and committed
  after the sweep. A later voxel cannot erase a source encounter's writes.
- W rotation selects the other source. Wave flags come from the current
  physical instant, eliminating the previous one-frame partner-shell lag.
- Same-affinity mechanical contacts precede the electroweak sieve. They
  require overlapping active fronts; a zero-radius propeller cannot kick.
- Contacts are deduplicated across voxels and mirrored encounters. Each
  reciprocal P pair supplies at most one kick per light frame, not two.
- K-D/D-D cohesion supplies opposite unit face-steps for separated body
  elements, conserving their unwrapped position sum. It has priority over
  propulsion, and each body element moves at most once per frame.
- A propeller selects a contacted body element at the rear along m, using
  a rotating address tie-break. Integer DDA distributes unit face-steps in
  proportion to the signed components of m. The magnitude of m is not a
  multi-cell displacement. Increasing pair population increases available
  kick opportunities; collisions and lack of contacts can leave them unused.
- Both halves of a bound pair recycle together toward the contacted body
  element, by at most one face-step. Bound pairs retain their m through
  breathing turnarounds, translation and kicks.
- A layer translation carries its wave state, charge, affinity and source
  identity together, preserving the physical clock. It cannot leave an old
  zero-radius centre behind or lose affinity at the destination.
- Same-family equal-charge singletons elect a chief through contact; the
  standard seed does not assign K. The family affinity is distinct from the
  elected chief's intrinsic W address.

These arbitration, step-budget and recycling choices are explicit reference
rules. They use host-side source arrays, contact lists and integer DDA state;
they are not yet a strictly local cellular realization. The ordinary app
refuses CUDA activation because the legacy GPU kernels lack these rules.

## Build and run

From `E:\alpha` in a Windows command prompt with the configured MSVC installation:

```bat
experiments\build_inertia.bat
experiments\seed_test.exe
experiments\inertia_test.exe
experiments\inertia_probe.exe 9 5 5 3 1 48 16384 right experiments\inertia_one.csv 12
experiments\inertia_probe.exe 15 9 9 5 1 160 16384 right experiments\inertia_n5_p1.csv 40
experiments\inertia_probe.exe 15 9 9 5 4 160 16384 right experiments\inertia_n5_p4.csv 40
experiments\inertia_probe.exe 9 5 5 3 8 160 16384 right experiments\inertia_n3_p8.csv 40
```

CLI: `LX LY LZ N PAIRS FRAMES [SIEVE] [MODE] [CSV] [BURN]`.
Modes: `right`, `left`, `balanced` (alternating directions), `foreign`
(pairs belong to another affinity), `up` (+y). All body elements have
charge 0x08; the two halves of each seeded photon-type pair are 0x00/0x1f.
N is the number of body elements, excluding the pair dressing. A canonical
interpretation uses N=L/3; tube experiments permit explicit reduced N/W.

The CSV measures unwrapped body centre of mass, displacement per light
frame, maximum observed pairwise body span (toroidal Manhattan distance),
and maximum distance of a pair centre to its nearest body element. It does
not infer a dressed-particle mass. FRAMES includes BURN; no dynamics is
disabled during BURN. The printed first/second window means help expose
transients or phase sampling. Window lengths should cover whole breathing
periods before interpreting small differences as secular drift.

## Checked results

The regression executes the production `simulation()` path. Baseline runs
use 48 frames, discarding 12, on a 9x5x5 tube with N=3:

| Condition | Mean axial body velocity (cells/light frame) |
|---|---:|
| No pairs | 0 |
| One rightward pair | +0.250000 |
| Three rightward pairs | +0.750000 |
| One leftward pair | -0.250000 |
| Two opposed pairs | 0 |
| One foreign-affinity pair | 0 |
| One +y pair | +0.250000 on y |

The zero-propeller tests also cover a displaced constituent (reciprocal
cohesion conserves centre of mass) and contact-driven election from S.
Assertions check unit-step bounds, charge, affinity, chief/delegate roles,
pair reciprocity, coincident pair halves and retention of m. The runs cross
the torus seam repeatedly; velocities use unwrapped displacements.

A lifecycle test leaves a foreign pair unattached for 24 frames, affiliates
it for 48 frames, then removes its affiliation for 24. Body displacements
are 0, +12 and 0 cells, respectively. An oblique m=(2,1,0) test gives body
displacement (24,12,0) over 48 measured frames, retaining the 2:1 direction.

Longer 160-frame checks (40 excluded) give:

| Tube | N | Aligned pairs | Mean vx | Max body span |
|---|---:|---:|---:|---:|
| 15x9x9 | 5 | 1 | 0.175000 | 1 |
| 15x9x9 | 5 | 4 | 0.700000 | 1 |
| 9x5x5 | 3 | 8 | 0.750000 | 0 |

The four-pair N=5 run reports first/second 60-frame means 0.693333 and
0.706667; the windows are not multiples of its eight-frame breathing period.
The maximum pair-to-body gap in this run is zero.

## Limits of the conclusion

These checks demonstrate controlled sustained transport, bounded steps and
preservation of the prepared island for the tested regimes. They do not
prove universal equilibration, arbitrary-phase stability, rotational/Lorentz
invariance, a mass law, force response, or stability for arbitrary initial
geometries and numbers of constituents. The speed identity sum(dx)/N is a
measurement, not a velocity injected into the body. A local implementation
of source coordination and GPU parity remain separate work.

The source-transaction, clock and seed corrections invalidate direct reuse
of the old alpha/scattering/attractor numbers as measurements of the current
kernel. The manuscript labels those tables historical pending reruns. No
checkpoint/resume support is claimed for the new contact/DDA runtime state.
