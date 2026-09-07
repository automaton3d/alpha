# Reciprocal movement matching: experimental candidate

Build: `experiments\build_color_matched.bat`.
Run from the repository root: `build\color_matched\color_fsm.exe 6`.
Diagnostics and objects: `build/color_matched/`.

This candidate addresses two defects in additive attraction: many partners
can cause arbitrarily large accumulated impulses, and persistent color kicks
can oppose attraction within an already associated body. It changes movement
selection, so it is a proposed rule revision, not merely an optimization.
The chief-only and additive-cohesion builds remain available independently.

## Transition

The original positive-radius, active, equal-charge encounter and spatial
contact diffusion are retained. The encounter additionally records whether
each partner already shared the current chief. Roles are read from the
journey's input state, not from the election being computed in that journey.

After spatial diffusion, each voxel proposes one moving partner from its
received contacts. For a same-chief K/D contact, the proposed movement is
the existing axiswise attraction (zero on axes separated by at most one cell).
A contact whose attraction is entirely zero makes no movement proposal.
For other contacts, the movement is the reciprocal color step. Color and
attraction are alternatives for an individual contact, not added together.
There is no time-based switch; classification can change as affiliations change.

Eligible edges are ordered by the tuple (shortest W gap, smaller endpoint W,
larger endpoint W). Each endpoint therefore assigns the same priority to an
edge; explicit endpoint addresses resolve ties independently of traversal
order. This is a structural address preference, not proven physically unbiased.
It does not use a global counter or scan-dependent claim on a partner.

Proposals circulate synchronously through adjacent W positions for W ticks.
Each voxel reads the received proposal and accepts only mutual selection.
All proposal shifts read the previous buffer and write the next buffer.
Accepted partners alone contribute to c. The kernel rejects a frame layout
without enough slots for this additional circulation. The present tested
layout is L=9, W=243; compatibility at other sizes remains untested.

This is a matching, not necessarily a maximal matching: rejected sources do
not propose again within the journey. Starvation is possible and must be
investigated. Election still considers all eligible contacts; a movement
matching does not restrict who can elect a common chief.

## Checks and limits

The harness checks at most one moving partner per source, at most one signed
step per axis, reciprocal total impulses, preserved affinity, unique sources,
and consistent chief references. Movement passes through diffusion and c;
neither a remote draft write nor an imposed center position is used.

The signed endpoint displacement bound is not a physical speed result: the
inherited modular transport can realize -1 through L-1 positive pulls. Reference
center reads still determine attraction direction. Phase and reissue retain
the prior locality limitations. Added bound-contact sets and proposal buffers
are experimental state. Thus this candidate is neither a complete local CA
realization nor a demonstration of inertia or stable spatial islands.

Possible return to superposition is measured rather than prevented with an
imposed repulsion, population quota, or additional random seed. Bounded
reciprocal motion alone does not guarantee the desired emergent structure.

## Result: rest-state coalescence without propellers

The six-frame run completed 5268 ticks with exit code zero. The independent
trajectory and matching audits passed:

`python experiments/analyze_color_fsm.py --directory build/color_matched`

`python experiments/analyze_color_matching.py`

| Frame | Source centers | K | D |
| --- | --- | --- | --- |
| 0 | 1 | 0 | 0 |
| 1 | 1 | 0 | 0 |
| 2 | 9 | 8 | 235 |
| 3 | 1 | 8 | 235 |
| 4 | 1 | 8 | 235 |
| 5 | 1 | 8 | 235 |
| 6 | 1 | 8 | 235 |

The candidate prevents accumulated movement overshoot and passes reciprocal
impulse checks. It reunites the associated constituents in superposition.
Under the clarified scope, this is admissible for the reduced resting system;
finite spatial extent is not an acceptance requirement at this stage.

The resulting geometry is absorbing under this reduced rule, not merely a
short observed plateau: all sources are coincident, all equal-charge contacts
share a chief, their attraction is zero, and different-charge contacts are
excluded. Therefore no contact proposes movement, c remains zero, and subsequent
wave phases cannot restart motion. This statement concerns positions and roles;
the complete wave state need not be stationary.

The earlier interpretation rejected this result because it lacked finite spatial
extent. That interpretation does not apply to the present reduced scope.
The specific P x K and P x D propeller interactions model inertia and must
be distinguished from electromagnetic light-matter interactions, which remain
excluded. The next [prepared inertia experiment](INERTIA_MATCHED.md) adds the
former only. The rest-state result neither proves nor disproves inertial
transport, and no artificial equilibrium spacing is imposed.
