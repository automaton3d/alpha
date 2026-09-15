# The turnover ablation (item 2): `EXCLUSION` isolates, it does not select

Why this exists: the planned `EXCLUSION` on/off ablation in
`quantization/README.md` was **vacuous** on the superposed seed -- every build
freezes, `Gamma_esc == 0` by construction, and the flux harness can only return
INCONCLUSIVE.  This note reports the ablation run on a prepared configuration that
**does** have turnover, and what it establishes.

## Design (harness `experiments/turnover_ablation.cpp`)

Tube `27 x 5 x 5`, `W = 6`, `RMAX = min(LY,LZ)/2 = 2`, so two sources can interact
only at centre distance `d <= 2*RMAX = 4`; the distances below respect that
arithmetic exactly.

| role | layers | charge | sites |
|---|---|---|---|
| body (one family, `K + 2 D`) | 0, 1, 2 | `0x08` | `x = 3, 10, 17` (pairwise 7, 7, 9 -- all > 4, so the body does not self-contact) |
| reservoir, mode `co` | 3, 4, 5 | `0x08` | `x = 6, 13, 20` (each 3 from a body member; pairwise 7 apart) |
| reservoir, mode `far` | 3 | `0x08` | `x = 24` (>= 5 from every body member) |
| inert spectators, mode `far` | 4, 5 | `0x09`, `0x0A` | `x = 24` (other charge words: `T1`-`T5` can never involve them) |

Arms: the same harness built with and without `/D EXCLUSION_FSM`
(`experiments/build_turnover_ablation.bat`).  All runs: 40 light frames, and the
harness writes `constituents.csv` in the census format, so
`quantization/flux_from_census.py` consumes it directly.

**Expectations, registered before the runs:**

| arm | expectation |
|---|---|
| `ref` + `co` | captures > 0 (a reservoir `S` attaches through `T4`) and escapes > 0 (the incoming `D` and a body `D` then meet, so `T2` promotes) |
| `ref` + `far` | nothing is in contact -> frozen |
| `excl` + `co` | the hard core blocks cross-family equal-charge contacts, so the reservoir can never feed the body -> frozen |

## Results

Harness level (from the run logs):

| arm | captures | escapes | verdict |
|---|---|---|---|
| `ref`, `co` | **5** | **2** | TURNOVER |
| `ref`, `far` | 0 | 0 | FROZEN |
| `excl`, `co` | **0** | **0** | FROZEN |

All three match the registered expectations.  Flux-harness level
(`quantize_stdlib.py` on the three CSVs):

| arm | `b` | `N*` | surrogate `p` | verdict |
|---|---|---|---|---|
| `ref`, `co` | -0.00022 | (117, meaningless) | **1.0000** | INCONCLUSIVE (artifact-consistent) |
| `ref`, `far` | -- | -- | -- | INCONCLUSIVE (zero escapes) |
| `excl`, `co` | -- | -- | -- | INCONCLUSIVE (zero escapes) |

## What it establishes

1. **The ablation is no longer vacuous, and it has a definite result.**  On a
   configuration built to host exchange, the reference rules *do* exchange
   (5 captures, 2 escapes); with `EXCLUSION_FSM` the same configuration exchanges
   **nothing**.  The hard core blocks equal-charge contacts between different
   families, and since the membership transitions require equal charge words
   (`chiefContact` returns unless `main.ch == mirror.ch`), a reservoir in another
   family becomes *invisible* to the body.
2. **`EXCLUSION` isolates islands; it does not select a population.**  This is the
   mechanism that produces the `81 x 3` candidate state, measured from the other
   side: it does not impose a quantum, it *cuts the island off from its
   environment*.  The manuscript's candidate claim can now be stated that way,
   with a measured ablation instead of a structural argument.
3. **Aggregation is a one-off relaxation, not a homeostat.**  In the `ref`+`co`
   arm everything happens in a single journey (frame 3) and is then static for 38
   frames (`captures = escapes = 0`).  The flux fit has nothing to fit: the
   surrogate test returns `p = 1.0000` and the "restoring drift" `N* = 117` is
   meaningless extrapolation.  So even where exchange exists, the model has no
   stationary turnover -- which is why the manuscript's
   `Gamma_cap(N*) = Gamma_esc(N*)` formulation has no data to live on.
4. **A frozen group can be larger than 2, and its size is set by reach.**  The
   `ref`+`co` arm freezes at `K=3 D=3 S=0`, three groups, `max_population = 4`
   (one `K` with three delegates) and stays there.  This is the theorem's scope
   clause measured: co-located same-charge delegates cannot stay grouped, but
   constituents spread beyond the contact range can, and the population that
   freezes is the number of sources the configuration puts within reach (3
   reservoir singletons absorbed here) -- not a quantum of the model.

## What follows for the manuscript

- The Limitations/prospects text that promises an `EXCLUSION` on/off ablation can
  cite this note: the ablation exists, its first arm has turnover, and its result
  is that the identity hard core **isolates** islands rather than quantising them.
- The `Gamma_cap = Gamma_esc` sentence should say what the measurements say: in
  this rule set the exchange is a single relaxation event followed by a frozen
  corner, so no rate crossing exists to define `N*`.

## Reproduce

```bat
experiments\build_turnover_ablation.bat
build\turnover_ablation\turnover_ablation_ref.exe 40 co  build\turnover_ablation\ref_co
build\turnover_ablation\turnover_ablation_ref.exe 40 far build\turnover_ablation\ref_far
build\turnover_ablation\turnover_ablation_exc.exe 40 co  build\turnover_ablation\exc_co
python quantization\flux_from_census.py ref_co=build/turnover_ablation/ref_co > quantization\analysis\ab_ref_co.csv
python quantization\quantize_stdlib.py quantization\analysis\ab_ref_co.csv
```

## Limits

One geometry (`RMAX = 2`, long side 27), one charge word, 40 frames, no drive
pairs (the body does not move), and the reservoir is placed by hand.  The natural
next configuration is a **moving** body (drive pairs present, as in
`inertia_fixture::prepare`) crossing a reservoir, which is the only way to obtain
*continuing* exchange rather than a single absorption; that is the configuration
to run before concluding anything about rate crossings in general.

## The moving body (run: the configuration that gives continuing exchange)

Design as above, with the body replaced by a **single chief plus one drive pair**:
layer 0 is the `K` (charge `0x08`), layers 1 and 2 are a reciprocal pair
(`0x00`/`0x1F`, `m[0] = +RMAX`, bound to layer 0, exactly as
`inertia_fixture::prepare` builds a driven body).  The reservoir sits ahead at
`x = 8, 14, 20`, and the runs are 120 journeys.  Two controls: the same geometry
with **different charge words** in the reservoir (`move_far`), and the same
configuration under `EXCLUSION_FSM`.

| arm | captures | escapes | does the *body* exchange? |
|---|---|---|---|
| `ref`, `move_co` | **10** | **7** | yes -- it absorbs and splits, repeatedly |
| `ref`, `move_far` (other charge words) | 0 | 0 | no -- it passes through them |
| `excl`, `move_co` | 6 | 3 | **no -- chief 0 never gains a member** |

Flux-harness verdicts on the three arms:

| arm | mean `dN` by population | `b` | verdict |
|---|---|---|---|
| `mv_ref_co` | `N=1: +0.01, N=2: -0.02, N=3: +0.11` | **+0.030** | **FAIL** (no restoring drift) |
| `mv_ref_far` | (no flux at all) | -- | INCONCLUSIVE |
| `mv_exc_co` | `N=1: 0.00, N=2: +0.01, N=3: +0.33` | +0.025 | FAIL |

What the moving configuration establishes:

1. **Continuing exchange exists** (10 captures, 7 escapes over 120 journeys), so
   item 2's precondition is met: the ablation is no longer vacuous *and* no longer
   limited to a single absorption.  A mechanistic detail worth recording: a lone
   `K` with one pair **oscillates** (`x0 = 3, 4, 5, 6, ...` and back); it is the
   first capture that makes the transport directional, because the `P x D`
   contact is what sustains motion (as the manuscript states for drive pairs).
2. **`EXCLUSION` leaves the body inert.**  In the hard-core arm the body's chief
   (layer 0) never appears with a member in the whole run; the 6 captures and 3
   escapes are entirely *internal to the reservoir family* (chiefs 3 and 4
   merging among themselves).  This is the isolation statement measured once more,
   and it also shows why per-chief attribution matters: the aggregate counters
   alone would have suggested "turnover continues under the hard core".
3. **The control is clean.**  Identical geometry with different charge words gives
   zero exchange over 120 journeys although contacts are still counted, because
   `chiefContact` requires equal charge words.
4. **Even with continuing turnover there is no restoring drift.**  The binned
   drift *rises* with population (`N=2` loses, `N=3` gains; `b = +0.030`), so
   captures outrun escapes for larger islands.  Item 2 therefore closes with the
   strongest statement the data can support: in the best-case configuration we can
   build -- a driven body crossing a same-charge reservoir -- the model's
   population dynamics is anti-restorative, and the flux harness, now fed with a
   genuinely viable dataset, returns **FAIL** rather than INCONCLUSIVE.


