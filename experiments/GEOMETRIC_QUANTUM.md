# The geometric-quantum hypothesis: tested and falsified

Last open positive path of the aggregation line.  The membership theorem
(`DYNAMIC_QUANTIZATION_DERIVATION.md`) shows that *co-located* frozen states have
population at most 2, and that the value `L/3` enters only through the seed's
address map.  If the model has a quantum that is not the seed's, it must be
**geometric**: a shell capacity, a ball capacity, or a contact capacity, i.e. a
number fixed by the lattice rather than by the rule or the seed.  This note tests
all three variants exactly and reports the verdict.

Reader: `experiments/analyze_geometric_quantum.py` (no dependencies; runs in
seconds).  Convention throughout is the one stated in the manuscript: a cell
belongs to shell `r` when `r^2 <= d^2 < (r+1)^2`, with `d^2` the toroidal
(wrapped) squared distance.

## 1. Contact capacity: there is none (the graph is complete)

Two sources can interact only where their shells overlap, i.e. at centre distance
`d <= 2*RMAX = L`.  The largest toroidal distance between two cells of an odd
cube of side `L` is

    d_max = sqrt(3) * (L-1)/2 ,

which is below `L` for every `L`:

| L | RMAX | `d_max` | `2*RMAX` | complete? |
|---|---|---|---|---|
| 7 | 3 | 5.20 | 6 | yes |
| 9 | 4 | 6.93 | 8 | yes |
| 11 | 5 | 8.66 | 10 | yes |
| 13 | 6 | 10.39 | 12 | yes |
| 15 | 7 | 12.12 | 14 | yes |

So at the turnaround **every pair of sources is in contact**, at every accessible
size: the contact capacity is `W`, not a small integer.  A "capacity quantum" of
this type therefore does not exist -- the earlier statement that `RMAX = CENTER`
makes all bubbles mutually in contact at `L = 9` is the general case, not a
peculiarity of that size.  The measured contact counter confirms the continuum
picture: the P7 tube (`RMAX = 2`, capacity 26 cells on shell 1 and 66 on shell 2
per coincident pair) records **156** contacts at frame 2, i.e. ~17 cells per
pair-tick -- a *fraction* of the geometric capacity that varies with the phase,
not a quantised count.

## 2. Shell and ball capacities do not contain the observed populations

| L | RMAX | `N_shell(r)`, `r = 0..RMAX` | `|B(r)|`, `r = 0..RMAX` |
|---|---|---|
| 7 | 3 | 1, 26, 66, 158 | 1, 27, 93, 251 |
| 9 | 4 | 1, 26, 66, 158, 234 | 1, 27, 93, 251, 485 |
| 11 | 5 | 1, 26, 66, 158, 234, 410 | 1, 27, 93, 251, 485, 895 |
| 13 | 6 | 1, 26, 66, 158, 234, 410, 470 | ..., 1365 |
| 15 | 7 | 1, 26, 66, 158, 234, 410, 470, 738 | ..., 2103 |

Two conventions were computed and agree exactly for `r <= RMAX`: the wrapped
toroidal count over the whole cube and the free-sphere count of the model's own
instrument (a local box, `wavefront.cpp` `shellPoints`).  They cannot differ
there, because a ball of radius `L/2` about a site does not wrap the torus.  The
capacities are therefore unambiguous: shells `26, 66, 158, ...`, balls
`27, 93, 251, ...`.

**Documentation discrepancy found.**  The manuscript quotes the theoretical shell
populations as `(6, 26, 98)` at `r = 1, 2, 3` (in the sieve-weighting paragraph of
Results and in the fixed-point subsection).  Neither convention reproduces that
triple: the band convention gives `(26, 66, 158)` and the exact-squared cumulative
gives `(6, 32, 98)`, so the triple appears to come from the pre-revision wavefront
probe.  The effect on the argument is small and conservative: the weighting
estimate for the `s2B` passes scales by `(26+66+158)/(6+26+98) = 1.92`, i.e. the
expected count rises from ~18 to ~35 and stays far below one pair after the
`samePos ^ sameT` coincidence -- but the quoted numbers should be corrected in the
paper.

The quantised populations the model actually produces are **2** (theorem bound,
reference build), **3** (candidate rule that switches T2 off inside a family) and
**5** (hand-planted inertia island).  None of them is a shell or ball capacity at
any `L <= 15`: the smallest non-trivial capacities are 26/27, two orders of
magnitude above the values observed.  A geometric quantum of this type would have
to produce populations in `{26, 66, 158, ...}` (shells) or `{27, 93, 251, ...}`
(balls); nothing of the kind appears in any *stable* state.

## 3. What the dynamics actually realises: a continuum, not a spectrum

Scanning every `groups.csv` under `build/` (59 files, all campaigns in the
repository) gives the population histogram

| population | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | ... | 24 | 26 | 27 | 28 | 30 | 33 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| group-frames | 45 203 | 5 623 | 12 553 | 208 | 58 | 104 | 21 | 15 | ... | 2 | 1 | 16 | 1 | 581 | 83 |

Every integer from 1 to 24 is realised (25, 29, 31 and 32 are absent only because
the runs that would produce them are small), plus 26-28, 30 and 33 in the
merging-tribe runs (the eight charge-word tribes of ~30 members and the
spread81 clusters).  A quantised observable must produce **gaps**; the realised
set is a near-continuum, because the only mechanism that grows a group in this
rule set is unprotected co-location, which has no size limit: `T2` splits groups,
`T3`/`T4` merge them, and which pairs meet is set by the rotation schedule.

The stable (frozen) populations, by contrast, are only three: **2** in the
reference build, **3** in the candidate build that removes `T2` inside a family,
and **5** in a prepared island.  Their origins are combinatorial and rule-based,
not geometric.

## 4. What does survive as a geometric law

Two geometric statements about the model are true and testable, and neither is a
population quantum:

- **The clock.**  `RMAX = L/2`, so one breathing era is `2*RMAX = L` light
  frames: 8 frames at `L = 9` (the window used by every census and candidate
  campaign) and 14 at `L = 15`.  This is what fixes the journeys at which
  turnarounds and candidate channels come alive.
- **The contact counter is an overlap measure.**  It scales with the
  intersecting shell area (26, 66, 158, ... cells per coincident pair, times the
  fraction of the phase in which both shells are active) rather than with any
  discrete index, as the P7 numbers show (156 contacts at frame 2, then the
  periodic 396/156/0 pattern).

## 5. What a genuine geometric quantum would need

A rule whose capture/escape balance is keyed on the shell index, for instance "a
source stops being captured when the island's shell index reaches the capacity of
the current shell".  Such a rule would quantise the population at the shell or
ball capacities, and therefore makes a sharp prediction the present data already
constrains: stable populations in `{26, 66, 158, ...}` (shells) or
`{27, 93, 251, ...}` (balls), and a *different* set at larger `L` (the shell set
is `L`-independent up to `RMAX`; the ball set grows with `RMAX` before saturating
at the cavity volume) -- i.e. the quantum would scale with the cavity, never with
`L/3`.  No such attractor appears in any run in the repository, so the hypothesis
is falsified at the accessible sizes.

## 6. Reproduce

```bat
python experiments\analyze_geometric_quantum.py build
rem sections 1-2 are the capacity tables; section 3 is the histogram over every
rem groups.csv found under build/
```

The verdict in one line: **the model has no geometric population quantum at
`L <= 15`** -- the contact capacity is `W`, the shell and ball capacities are two
orders of magnitude above every stable population, and the realised populations
form a continuum rather than a gap spectrum.  Together with the membership
theorem this closes the last positive path of the aggregation line: the model's
only quantised populations are the rule-induced 2, the candidate 3 and the
planted 5.

