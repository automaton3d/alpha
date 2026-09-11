# Drive-pair inertia: current-rule revalidation

This campaign rebuilds the reference tests and records complete diagnostic
trajectories under the current D x D promotion and K x K clash rules. All
outputs are isolated in `build/inertia_revalidation`; historical artifacts
are preserved. The ordinary production dynamics and the matched experimental
FSM remain separate executables and are not interchangeable measurements.

## Reproduce

```
experiments\build_inertia_revalidation.bat
python experiments/run_inertia_revalidation.py production
python experiments/run_inertia_revalidation.py matched
python experiments/analyze_inertia_revalidation.py
```

The runners continue after a failed case and retain every exit code in
`execution.json`. They return nonzero if any executable fails. The analyzer
returns nonzero when physical acceptance fails; a completed diagnostic with
exit zero is not a claim of successful island transport. `audit.json` lists
the failures case by case. Original assertions are retained for comparison.

The matched build explicitly enables DD_PROMOTION_FSM and KK_CLASH_FSM in
addition to the color, chief, cohesion, matching and inertial macros. The
historical matched build omitted both role-transition macros. Production
uses its ordinary encounter path, with no EXCLUSION or orphan-mediation macro.

`build_inertia_matched.bat` now also enables both role rules. Its original
fixed-role harness remains a regression comparison that fails on the changed
island identities; use the complete runner above to collect every case.

## Measurements and acceptance

The diagnostic follows the same prepared source identities throughout each
run. It accumulates center displacement each tick, exports unwrapped xyz
trajectories, checks source uniqueness against the grid, and records charge,
affinity, role, parent, reciprocal pair identity and m. Its center-of-mass
velocity is that of the prepared constituent set; it must not be called an
island velocity when the set has split into several chief identities.

Role changes are allowed as outcomes. After the measurement burn-in, the
prepared body must still have one live chief identity and no unresolved
members to pass as one island. Body span and drive-pair distance use unwrapped
Manhattan distances, so winding around the torus cannot hide dispersion.
The operational localization thresholds are body span <= 3 and pair distance
<= 2 RMAX; these are acceptance criteria, not inferred equilibrium radii.

Production promises one face-step per journey. The matched variant permits
one step per axis; the diagnostic records the total number of axis steps,
with a bound of three, while the original matched harness checks individual
axes until its first role-identity failure. Neither check proves a local
causal implementation or a universal physical speed limit.

Controls cover no drive pair, opposite directions, foreign affinity, broken
pair association and zero m. Other cases cover increasing pair population,
axis change, oblique m, preparation from S, and affiliation lifecycle. The
lifecycle intentionally changes affiliation after frames 24 and 72; it is an
intervention test, not unforced formation or unforced capture. The production
fixture also checks a displaced constituent and larger N=5 islands over 160
frames. Matched cases use 96 frames; ordinary production cases use 48, with
96 for lifecycle and 160 for the saturation case. The first quarter of each
run is omitted only from mean-velocity/identity acceptance, never from dynamics
or the maximum separation measurements.

Population comparisons change W and hence the frame duration. They are
conditional transport checks, not a fixed-universe mass or speed law. A light
frame is distinct from an era and from a housekeeping tick. The reported
one-cell/frame result is the configured journey-step bound, not a measured
one-cell/k universal speed.

## What the failures mean

The original production suite aborts when the first prepared delegate becomes
a chief, violating its old fixed-role assertion. In the rest diagnostic,
W=1 becomes K in frame 2 while W=0 remains K and W=2 remains its delegate.
The endpoint census therefore contains two chiefs, even when their centers
coincide. Sustained motion of this constituent set does not establish transport
of a single 1K+2D island.

In the matched rest trace, D promotion is followed by K-clash demotion and
color-driven separation: the centers alternate between coincidence and
opposite xyz offsets. Both role transitions are active. Restoring the role
rules invalidates the historical fixed-role matched results; disabling those
rules again would not validate the intended dynamics.

The K-only case avoids D promotion and remains a successful minimum transport
test in both directions. Extending it to one persistent, multi-constituent
island remains open, along with response to an external impulse, a mass law,
arbitrary-phase robustness and strict locality. No new physical rule is
introduced in this revalidation to force those conclusions.

## Completed results

All 32 diagnostic runs completed with exit zero and zero recorded integrity
errors. The CSV auditor independently verified velocities, unwrapped spans,
pair distances, role-change counts and live-chief membership. Physical
acceptance passed only the four K-only directional cases. The original
production suite exited 3 at its fixed-role assertion. Of the nine original
matched cases, right and left exited zero; the other seven exited 2 at the
source-identity assertion. Runner/auditor nonzero exits preserve these failures.

Representative results (mean displacement per light frame after burn-in):

| Variant | Prepared body | Drive pairs | Mean vx | Max unwrapped body span | Max pair gap | Interpretation |
|---|---|---|---:|---:|---:|---|
| Production | K | 1 right/left | +0.75 / -0.75 | 0 | 0 | Minimum transport passes |
| Matched | K | 1 right/left | +1 / -1 | 0 | 0 | Minimum transport passes |
| Production | K+2D | 1 right | +0.25 | 1 | 0 | Moves, but has multiple chiefs |
| Production | K+2D | 3 right | +0.75 | 0 | 0 | Moves, but has multiple chiefs |
| Production | K+4D | 1 right | +0.175 | 1 | 0 | Moves, but has multiple chiefs |
| Production | K+4D | 4 right | +0.70 | 1 | 0 | Moves, but has multiple chiefs |
| Matched | K+2D | none | 0 | 6 | -- | Bounded oscillation with changing chief identities |
| Matched | K+2D | 1 right | +0.138889 | 81 | 32 | Loss of cohesion and pair proximity |
| Matched | K+2D | 1 left | -0.157407 | 143 | 22 | Loss of cohesion; reflection asymmetry |
| Matched | K+2D | 3 right | 0 | 18 | 6 | No sustained axial transport |

Production retains the historical directional, oblique (2:1), affinity and
population-speed responses for the prepared constituent set, but not its
single-chief identity. In matched, balanced pairs give a small nonzero drift
and the oblique case gives a 1:1 rather than 2:1 direction. Zero, foreign and
broken-pair controls have zero mean drift, but the body still alternates
roles and positions like the no-pair case. A span above the acceptance
threshold does not by itself prove unbounded dispersion; the no-pair trace
is explicitly a bounded oscillation.

The next unresolved mechanism is maintenance of a single island identity
under D promotion, K clash and cohesion. Force response and an emergent mass
law have not been added or claimed by this campaign.
