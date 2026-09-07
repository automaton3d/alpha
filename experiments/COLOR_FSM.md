# Color contact integrated into the reference FSM (experimental variant)

Current work returns to island formation with inertia disabled. The
[temporary random reemission experiment](COAGULATION_RANDOM.md) perturbs
persistent overlaps through c, then observes unforced coagulation. The
intervention is test apparatus, not a proposed physical rule.
The [D x D promotion restoration](DD_PROMOTION_AUDIT.md) adds the missing
equal-charge promotion without a parent, affinity, spin, or separation gate.
The complementary [K x K clash experiment](KK_CLASH.md) demotes the larger-W
chief to a delegate of the smaller-W chief. Both transitions are enabled;
the resulting population is measured rather than fixed at 9L.

The separate [color-and-chief variant](COLOR_CHIEF.md) adds contact-driven
election while preserving this color-only baseline. It does not yet add
mechanical cohesion.

## Trajectory and contact diagnostics

The runner now also writes trajectories.csv (each W source at the sampled
stage boundaries) and contacts.csv (one row per directed source contact,
sampled after diffusion). The contact tick is the observation tick, not
the tick of its first voxel encounter. The row includes the reverse-contact
presence and the signed unit contribution. Trajectories record source
phase, modular c, signed accumulated contributions and reissue flags.
Diagnostics only read kernel state. A missing reciprocal contact is
reported, not silently manufactured or repaired.

`python experiments\analyze_color_fsm.py` reads the files and writes
build/color_fsm/analysis.json. It verifies final positions against signed
contributions modulo 9, counts coincident source pairs, measures the RMS
pairwise toroidal separation, and detects repeated per-W positions and
phases. Such repetition is only a projected-state recurrence; u/v and the
other fields are not included. Signed contributions are not the actual
path length of the positive-step modular translation algorithm.

Build: `experiments\build_color_fsm.bat`

Run from E:\alpha: `build\color_fsm\color_fsm.exe 4`

Output: build/color_fsm/trace.csv. Build objects are isolated. The
COLOR_ENCOUNTER_FSM macro selects this reduced path inside simulation();
ordinary builds keep their previous behavior. No manuscript claim changes.

## Initial conditions and persistent rule

The harness uses initGeneral and replicate on a cubic L=9, W=243 seed.
All sources are S, spatially coincident, with t=0 and zero m/polarization.
The harness does not construct or activate a positive-radius shell, set
directions, move centers, or edit state between simulation calls.
The existing distance and wave updates create the first positive front.

At every encounter window, main and a frozen mirror compare at each voxel.
Mirror rotates one W position per shift. Only active, positive-radius,
equal-full-charge participants record a contact. Relative W order gives
opposite signs for reciprocal contacts; color bit 0/1/2 maps to x/y/z.
No affinity test is used. The rule is not disabled after the first journey.
P formation, K election, m election, and the other interactions are absent.

## Nonuniform contacts: explicit added state and rule

Each voxel has a set of encountered W identities. A bitset is its software
representation; one bit per W identity is experimental added state, not a
previously established fixed-size cell axiom. Multiple voxels or repeated
observations never multiply one partner's contribution.

During diffusion, sets propagate by union from the six spatial neighbors,
reading only the previous tick. A number of sweeps equal to the toroidal
Manhattan diameter suffices to cover a layer. Each voxel independently
converts its received set to modular c. Conflicting partial contact sets
are merged rather than rejected. Before translation a read-only check
requires that c agree throughout the layer. No global contact arbitration
chooses which event wins.

Translation uses synchronous neighbor pulls in the existing x/y/z slots.
c is consumed. Negative offsets are represented modulo the axis length;
this reproduces positions but does not establish minimal signed-transport
latency or a physical velocity. Source uniqueness is checked after each
translation tick. The reference center cache is refreshed from actual
transported r2=0 cells, never assigned a predicted destination.

At reissue, layers that actually translated reset their source phase and
wave field: the source has radius zero and amplitude 2048; other cells
return to unreached geometry. The next wavefront grows by the same distance
and phase routines. A frame edge advances t in the existing clock convention.

## Remaining limitations

This integrates a proposed contact/diffusion/translation path, not a proof
of strict AC locality. The inherited geometry reads source coordinates;
phase_step reads the source clock. The reissue notification is still a
per-layer flag, not a locally propagated message. These dependencies are
explicit and remain work to do. Contact-set diffusion and modular transport
also need validation against the intended state budget and physical timing.

The observer samples stage boundaries and reports distinct centers and
positive source phases. Its acceptance checks require structural integrity,
not a prescribed amount of spreading. Several journeys do not establish
islands, long-term stability or inertia.

## Structural restriction of the permanent fixed-color rule

For a fixed charge, every signed contribution is plus or minus the same
vector s=(1-2b0,1-2b1,1-2b2). Therefore every completed journey adds an
integer multiple of s. Starting from the common center, each source's
boundary position belongs to a single toroidal body diagonal. Opposite
colors share the same line, giving four lines in total. For odd L these
lines intersect only at the initial center, so their union contains
4L-3 sites: exactly 33 for L=9. Intermediate axis-by-axis transport is not
subject to this positional restriction.

The analyzer checks this invariant per source at every frame boundary.
Saturation at 33 distinct centers is geometric confinement, not equilibrium.
This fixed-color rule alone cannot give 81 distinct source centers at L=9.
No direction-changing dynamics or charge changes are present in this run.

## Verified four-frame run

The subsequent instrumented run completed 12 frames (10536 ticks), exited
zero, and reproduced the first four counts. All recorded directed contacts
had reciprocals; signed contributions summed to zero per frame in every
axis. From frames 3 through 12 the distinct-center count stayed at 33,
while 150 to 230 sources changed boundary position per journey. No complete
per-W position-and-phase configuration repeated at sampled frame boundaries.
The diagonal invariant held throughout. This is persistent motion within a
restricted geometry, not demonstrated equilibrium or island formation.
Full diagnostics are in trajectories.csv, contacts.csv and analysis.json
under build/color_fsm. The earlier four-frame account below remains valid.

The executable completed 3512 ticks and exited zero. Distinct centers at
frame boundaries were 1 initially, then 1, 19, 33 and 33. The first spatial
separation completed at tick 1731, in the second frame's translation stage.
No displacement occurred in the first, zero-radius encounter journey.
The fourth frame preserving a count of 33 does not establish unchanged
positions or stability: this observer reports counts, not full trajectories.

Source uniqueness and absence of K/P/m were checked at stage boundaries;
translation source uniqueness and diffusion consistency were checked by
the kernel. The earlier prepared-checkpoint color_encounter executable was
rebuilt without this macro and passed its reciprocity and transport checks.
