# Family-selective directionality (WP8 option 2)

## Problem it addresses

The non-family directional channel fragments the canonical seed monotonically
(`1 -> 2 -> 15 -> 28 -> 44 -> 62 -> ... -> 81` distinct centres at journey 16) but
then drifts through **86--94** centres with `max_families` 3--4: the island count is
not stable and families mix.  The census's predicted island is `1K + nD` with
`n = L/3 - 1`, i.e. exactly the `L/3` W copies of ONE family (`family = w / 3`), so
the family has to act as a single unit: the direction must be decided *inside* the
family and applied to all its copies together.

## The two macros (both OFF in the reference)

The reference build is unaffected: `alpha_probe` still reports
`active-passes=6468, s2B-passes=0, pairs-formed=0` and
`alpha_A = 0.003756878`.

* **`FAMILY_SELECTIVE_FSM`**
  - *Producer* (`src/model/interaction.cpp`, `encounter`): only **intra-family**
    pairs (`!diffFamily`, i.e. same `w/3`) write the directional field, and they
    write the **relative-displacement** encoding
    `c = L + (own - anchor) % L` -- the only directional one.  The cross-family
    producers are excluded: producer (2)'s W-address tie-break and the
    absolute-coordinate encodings of (1) are what dragged constituents of
    *different* families together.
  - *Consumer* (`src/model/simulation.cpp`, `applyMomentum`): the copies of a
    family share **one decision per light frame**, read from the family's own
    copies (largest decodable distance, ties by ascending `w`) and applied to
    every copy, so a family keeps a single centre.
* **`FAMILY_RIGID_FSM`** — **WITHDRAWN (2026-09-12); it is now a no-op.**
  It used to read `lcenters[]` for the family's other layers, in the same tick, and
  use the comparison as a predicate on the transport: a non-local read with a
  readable consequence, outside the non-signaling idealisation.  The purely local
  probe below falsified the alternative (that the rigidity is emergent from the
  shared field), and the differential test confirmed the audit: with the predicate
  removed, the `fam` and `famrigid` binaries produce **identical** output, so the
  table predicate was doing 100% of the difference.  The rule may only return as a
  local (encounter-level) reformulation; see "Locality audit" below.
  Its former intent was: a family whose copies are not co-located sends every copy
  to its family chief (the `K` copy, else the first), so an island re-coheres
  instead of splitting -- self-correcting rather than a freeze.
  *(Historical result, host-level, not a claim about the model's local rule: with
  it enabled the island count settled at 79 +- 2 over journeys 15--20.)*

## Structural finding (forced by measurement)

The only `homB` write the SLOT II homing stage can actually **see** is the
per-**cell** one (`draft.homB = 1`, the archived CUDA's `dev_encounter4` pattern).
A write into a source-centre *draft* is not visible to that cell's neighbours until
the frame ends, and `homB` is cleared at the end of every frame.  Measured: with
the pair producers alone (which write drafts) the homing block reported
`homb_seen = 0` although `homb_events` was non-zero, and `c_at_center` stayed 0 --
the channel was silently dead.  Restoring a **per-family, per-cell winner** (one
winner per family per turnaround, on the family's anchor layer) brought it back.

## Validation (fast probe, 15x9x9, N=3, 16 light frames)

```
homb_events=7  homb_seen=184  c_at_center=393  cB_at_center=40
reloc_moves=9  reloc_cells=31909  consumed=5
```

Identical to the non-family build's downstream chain, so the family restriction
does not disable the channel; the difference has to show up in the census
structure (81 families), which is what the census runs test.

## Build and run

| script | binary | purpose |
|---|---|---|
| `build_rest_shell_probe_fam.bat` | `rest_shell_probe_fam.exe` | fast probe, family channel |
| `build_rest_shell_probe_famrigid.bat` | `rest_shell_probe_famrigid.exe` | fast probe, rigid variant |
| `build_island_census_fam.bat` | `island_census_fam.exe` | production census |
| `build_island_census_famrigid.bat` | `island_census_famr.exe` | production census, rigid |

Run as `island_census_fam.exe <frames> <sieve> build\island_census_fam\run<N>`;
the acceptance signature is 81 distinct source centres with the three copies of
each family co-located.

## Result: 20 journeys, three-way comparison

All three runs are 20 journeys at L=9/W=243, sieve 16384 (the baseline also has a
64-journey run, `build/island_census_dir/run64`).  `distinct_centers` per frame:

| frame | 2--6 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 |
|---|---|---|---|---|---|---|---|---|---|
| **baseline** (non-family) | 1 | 62 | 73 | 78 | **81** | 94 | 87 | 89 | 86 |
| **family** | 1 | 82 | 83 | (83) | 97 | 100 | 98 | 101 | 93 |
| **family + rigid** | 1 | 63 | 69 | 78 | **80** | 82 | 79 | 79 | **79** |

Stability over the last six journeys (f15--f20):

| variant | f15 ... f20 | amplitude | final | localized | pop == L/3 |
|---|---|---|---|---|---|
| baseline | 78, 81, 94, 87, 89, 86 | **16** | 86 | 40 | 15 |
| family | 83, 97, 100, 98, 101, 93 | **18** | 93 | 33 | 11 |
| **family + rigid** | 78, 80, 82, 79, 79, **79** | **4** | **79** | 35 | 9 |

Readings:

* The family restriction alone makes the seed shatter **faster** (82 centres by
  journey 13 against 62 for the baseline) but then **overshoots badly** (97--101):
  the three copies share a step, yet nothing holds them together, so the family's
  own copies drift apart and `distinct_centers` exceeds 9L = 81.
* The **rigid** rule (a non-co-located family homes every copy on its chief) is the
  stabiliser: it tracks the 81 signature from journey 15 onwards and holds
  **79 +- 2**, i.e. it removes both the baseline's overshoot (78 -> 94) and the
  plain family variant's shattering (83 -> 101).
* All variants keep `P = 0`, `stable>=5frames = 235`, and every variant produces
  groups at the predicted population `L/3` (baseline 15, family 11, rigid 9) --
  whereas the reference produces **none** in 64 journeys.

Verdict (graded (C), candidate macros only): `FAMILY_RIGID_FSM` is the first
configuration in which the island count of this model **settles on the predicted
value** instead of drifting.  It is not bit-stable (the count still moves between
78 and 82) and the underlying chiefs still reorganise freely, so the next question
is what sets the +-2 jitter and whether a longer run or a stricter anchor (e.g.
binding the family to its K chief permanently) removes it.

## Locality audit of the rigid rule (measured)

`FAMILY_RIGID_FSM` reads `lcenters[f]` for the family's *other* layers and uses the
comparison as a predicate on the transport.  That is a non-local read of the
host's per-layer table, in the same tick, with a **readable** consequence (the
centre position), i.e. it falls outside the non-signaling idealisation (same
category as the host scheduler of Sect.~`nosignaling-formal`).  Before any claim is
made, the obvious question is whether the rigidity is *emergent* from the shared
field -- in which case the predicate is redundant and can be dropped, making the
island formation a local result.

Measured with a purely local probe (each copy decodes its step from its OWN `c[]`
and its OWN `x[]`; no `lcenters`), 15x9x9, 16 light frames:

| run | field arrivals at the three copies | copies with a deCODABLE step | ticks with >= 2 copies fielded | steps agreeing |
|---|---|---|---|---|
| N=3 (1 family) | `c0 = 393`, **`c1 = 0`, `c2 = 0`** | 114 | **0** | 0 |
| N=6 (2 families) | `c0 = 801`, **`c1 = 0`, `c2 = 0`** | 280 | **0** | 0 |

**The emergent-co-movement hypothesis is falsified**: every field arrival lands in
the family's *first* copy; the second and third copies receive nothing, so there is
no shared field for anything to emerge from, and the co-movement is produced
entirely by the table predicate.

Structural reason (this is not a tuning artefact):

1. the only `homB` write the SLOT II homing stage can see is the per-cell one, and
   the per-family winner is gated on `x[3] % 3 == 0`, i.e. it writes **only the
   family's first copy**;
2. the field's propagation sweeps operate **within one layer** (a cell's six
   neighbours are spatial neighbours in its own `w`), so a field written in layer
   `3k` can never reach layers `3k+1`, `3k+2`.  The only cross-layer channel in the
   architecture is the **encounter** itself.

Consequences:

* The demonstrated island separation (79 +- 2 centres) currently rests on a
  **host-level, non-local coupling**.  It must either be reported as such -- under
  the same caveat as the host scheduler -- or declared as a postulate (P) of the
  model.
* A legitimate cross-layer coupling has to live where the model *already* declares
  one: in the encounter (a rule stage that pairs cells in contact; superluminal in
  coordinates but non-signaling by the paper's own argument).  The reformulation to
  try is therefore to let the **encounter write the displacement into each copy's
  own layer** (using only the two contacting cells' own fields), so that every copy
  carries the field *in its own layer* and the local consumer makes them step
  together -- rigidity emergent from per-copy local fields, with no `lcenters`
  read anywhere.
* The base consumer (`HOMB_CONSUMER_TRANSPORT`) is unaffected by this criticism: it
  reads only the cell's own `c[]`, own `x[]` and own `t`.

### Action taken: the non-local consumption is removed

Both non-local parts of the consumer have been deleted: the block that gathered the
family's copies through `lcenters[]` to form one decision, and the `FAMILY_RIGID_FSM`
re-cohesion.  The consumer is now purely local (own `c[]`, own `x[]`, own `t`), and
`FAMILY_RIGID_FSM` is a no-op.  What survives of `FAMILY_SELECTIVE_FSM` is its
**encounter** half (the producer), which pairs two cells already in contact and reads
only their own `x[3]`.

Two consequences measured immediately:

* the withdrawal changed **no** behaviour of the `fam` build (identical counters,
  `homb_events=7, homb_seen=184, c_at_center=393, reloc_moves=9, reloc_cells=31909,
  consumed=5`), because the family-gathering loop sat behind the consumer's outer
  `c[] != 0` guard and therefore never ran for copies 1 and 2 -- which never carry a
  field at all.  Its apparent "co-movement" was already inert;
* the differential test passes: with the predicate gone, the `fam` and `famrigid`
  binaries produce **identical** output, so the settled 79 +- 2 of the 20-journey
  census was produced entirely by the table predicate, and `famrigid` is retired as a
  host-level artefact rather than a candidate mechanism.

Next: the local reformulation, i.e. let the **encounter** write the displacement into
each copy's own layer, so every copy carries the field locally and any co-movement is
emergent from per-copy fields with no `lcenters` read anywhere.



