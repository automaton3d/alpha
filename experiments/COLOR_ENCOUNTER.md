# Equal-charge color encounter

The subsequent FSM experiment starting from the unmodified zero-radius seed
is documented in [COLOR_FSM.md](COLOR_FSM.md). This file describes the earlier
prepared-checkpoint test, retained as a regression for the contact rule.

Build: `experiments\build_color_encounter.bat`

Run from the project root: `build\color_encounter\color_encounter.exe`

This isolated experiment uses L=9, W=243 and the ordinary superposed S
seed. It is not wired into simulation(). It tests one proposed encounter
journey followed by diffusion and translation, without other interactions,
chief election, propellers or momentum election.

## Rule

Both participants must be active with positive local radius and identical
full charge words. The relative gap is (w_mirror-w_main) modulo W. Gaps
less than half W use the color signs; gaps greater than half W invert them.
Self encounters are excluded. The code also excludes the antipode for even
W (the tested canonical W is odd). Bit 0/1/2 maps to x/y/z respectively;
zero gives +1, one gives -1. This axis mapping is an explicit convention.

Each shift reads frozen main and mirror states and writes only main's
draft accumulator. Mirror rotates once after the shift. A complete journey
visits all W offsets, including the excluded self comparison. Color and
charge are unchanged, so in this specific additive rule the final sum is
independent of the order of those visits, though partial sums can differ.

## Scope of the transport

The positive-radius checkpoint is prepared: the existing distance update
is run for 12 sweeps, and r=1 cells are marked active. This is not a measured
time of first positive-radius contact or a completed era simulation.

Every contact voxel accumulates its own c, modulo L. Values are copied by
synchronous six-neighbor diffusion using a separate validity marker, not
summed between voxels. Conflicting values cause failure. This diffusion is
an experimental rule valid for the uniform contact field of the superposed
case; it is not a solution for arbitrary collision geometries.

Translation is a synchronous local pull along each axis. L-1 positive
copies represent displacement -1 on the torus. This verifies positions,
not a physical speed or a minimal signed transport latency. All fields
are transported cell by cell; centers are observed afterward from r2=0.
No lcenters override or applyMomentum source translation is used.

## Observed result

The run exited zero: 243 unique layer sources survived, now occupying
19 distinct spatial centers instead of one. The sum of unwrapped signed
encounter contributions was exactly (0,0,0). The CSV records the signed
contributions separately from the wrapped final positions.

Checks cover every ordered pair of W addresses and all eight colors for
reciprocity, zero-radius exclusion, and unequal-charge exclusion. The
full-grid run checks agreement of local accumulators with an independent
pair sum, complete diffusion, final positions and source uniqueness.
The pair-sum observer never writes to the lattice.

Files: build/color_encounter/result.csv and result.log. This demonstrates
spreading for the proposed isolated rule, not 81 spatial islands, stable
aggregation, or functioning inertia. Next, diffusion and translation must
be reconciled with the actual FSM and tested on nonuniform contacts.
