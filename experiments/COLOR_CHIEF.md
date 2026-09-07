# Color spreading and chief election

The subsequent [cohesion experiment](COLOR_COHESION.md) adds provisional
reciprocal attraction through c and compares its spatial measurements with
this baseline.

Build from the repository root: `experiments\build_color_chief.bat`.
Run: `build\color_chief\color_fsm.exe 4`.
Analyze: `python experiments\analyze_color_fsm.py --directory build/color_chief`.
Outputs and objects are isolated in `build/color_chief`; the color-only
experiment remains available through its original build script.

This variant adds chief election to COLOR_ENCOUNTER_FSM. It uses the ordinary
superposed L=9, W=243 seed, without prepared radii, chiefs, or displacements.
The same active, positive-radius, equal-charge encounter records both the
color contact and the partner's chief candidate. Affinity and seed-family
membership do not gate either operation.

An S proposes its own immutable W address, ignoring its seeded leader field.
A K proposes itself; a D proposes its parent. Main updates only its own
candidate; mirror remains a frozen encounter snapshot. A minimum candidate
propagates synchronously through the six spatial neighbors alongside the
contact-set union. At the end of diffusion, voxels that received a contact
adopt the candidate: K if it equals their own W address, D otherwise. Affinity
is preserved. Without a contact, the existing role remains unchanged.

This retains the reference election's minimum-address preference, but does
not read or modify the partner's draft. It adds one candidate register per
voxel (with double buffering). No global election counter, affinity filter,
family quota, or required chief count is used. Parents can temporarily name
a former chief during a merger; the observer reports unresolved parents
rather than silently following or repairing chains.

Color impulses and their c-driven translation remain unchanged and run in
the same journey as election. There is no stage that turns color spreading
off after election. Kernel checks require uniform role and translation fields
after diffusion and one source per W throughout translation. The harness
checks preserved affinity, equal charge with the recorded chief, absence of
P/m, and no election or displacement in the first zero-radius journey.

`groups.csv` reports each chief's population, contributing seed families,
distinct member centers, largest toroidal Manhattan distance to the chief,
and whether the named chief is currently K. These are measurements, not
acceptance thresholds. Trajectories and contacts retain the color experiment's
formats, allowing the independent displacement and reciprocity analysis.

This is an integration of spreading and election, not yet of mechanical
cohesion. K x D and D x D attraction from the older source-level resolver is
not enabled: its coordinated source writes must still be expressed through
unilateral encounters and c. Consequently, agreement on a chief does not
demonstrate a localized or stable island. The inherited geometry, phase,
and reissue locality limitations in COLOR_FSM.md still apply. Four frame
journeys do not constitute evidence of long-term stability over eras.

## Four-frame observation

The run completed 3512 ticks with exit code zero. At frame boundaries:

| Frame | K | D | S | Distinct source centers |
| --- | --- | --- | --- | --- |
| 0 | 0 | 0 | 243 | 1 |
| 1 | 0 | 0 | 243 | 1 |
| 2 | 8 | 235 | 0 | 19 |
| 3 | 8 | 235 | 0 | 33 |
| 4 | 8 | 235 | 0 | 33 |

Every recorded parent named a current K. The eight groups mixed seed families:
one had 33 constituents from 11 families, seven had 30 from 10 families each.
At frame 4, these groups occupied respectively seven and nine distinct sites;
maximum Manhattan distances to their chiefs were nine and twelve cells.
Thus chief agreement persists while constituents spread, without demonstrated
spatial cohesion. The canonical seed has eight distinct full-charge words.

All observed directed contacts had reciprocals. Independent analysis verified
displacement from c, zero net signed color impulse, and the body-diagonal
restriction. The four-frame trajectories reproduce the color-only baseline,
as expected: in this variant, chief identity does not yet feed back into motion.
This identifies the next missing part precisely: mechanical encounters must
affect c if these associations are to become spatially cohesive islands.
