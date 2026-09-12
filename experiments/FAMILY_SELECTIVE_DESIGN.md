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
* **`FAMILY_RIGID_FSM`** (implies the above)
  - additionally, a family whose copies are **not co-located** sends every copy to
    its family chief (the `K` copy, else the first), so an island **re-coheres**
    instead of splitting.  This is self-correcting, not a freeze: once the copies
    are co-located the shared family decision takes over again.

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
