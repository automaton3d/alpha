# The island size spectrum under the concrete definition of quantization

**Definition measured here** (the concrete one; not an invariant, not a value): quantization is the
**spontaneous formation of islands whose bubbles carry the same charge and have approximately equal
size** -- a geometric, structural, emergent phenomenon.  Measured on the **reference build** (all
candidate macros OFF), at two lattice sizes, from the censuses already in the repository.  Nothing
new was run.

**Artifacts.** `experiments/analyze_island_spectrum.py` (stdlib only); input = the census
`groups.csv` (`frame,tick,chief,population,seed_families,distinct_centers,max_distance_to_chief,`
`charge_words,valid_chief`) plus its sibling `census.csv` for `W`; the charge word of a chief is the
seed's own address map applied to its family (`initSim.cpp:76-80`), so the label does not assume any
dynamical claim.

## Measured

| census | L | seed multiplicity | groups | size histogram | non-singleton islands | words carrying one | spread(max-min) | mixed charge | centres |
|---|---|---|---|---|---|---|---|---|---|
| `census_corrected` (S=16384) | 9 | 3 | 235 | 1:227, 2:8 | 8 | 8 of 8 | **0** | 0 | 1 |
| `island_census/L15` (S=16384) | 15 | 5 | 667 | 1:659, 2:8 | 8 | 8 of 8 | **0** | 0 | 1 |
| `island_census/ref_el9_s64` (S=64) | 9 | 3 | 145 | 1:140, 2:5 | 5 | 5 of 5 | **0** | 0 | 1 |

Facts that hold in every row, and hold **frame after frame** (frames 2..4 at S=16384, 2..6 at S=64;
the group count does not move at all):

1. **Same charge: exact.**  `mixed charge = 0` everywhere -- every group is single-charge, as the
   reference transitions require (T1--T5 read `ch`).
2. **Equal size: exact, not approximate.**  All non-singleton islands have size **2**; the spread of
   their sizes is **zero**.  This is not a fitted value: it is the fixed-point bound of the reference
   membership transitions, and the bound is *attained*.
3. **One per charge word.**  The number of size-2 islands equals the number of charge words that carry
   any island at all -- 8 at `L=9` and 8 at `L=15`, 5 at the open gate -- i.e. the pattern is
   **independent of the lattice size** while the seed multiplicity changes from 3 to 5.  Nothing in the
   membership rules reads the multiplicity, so this is an emergent regularity, not a partition effect.
4. **Spontaneous and stable.**  No candidate macro is involved (reference build), and from frame 2 on
   nothing changes: no captures, no escapes, no growth.

## What is therefore true, and what is not

**True under the definition:** the reference dynamics *does* spontaneously produce a same-charge,
equal-size island population -- one two-bubble island per reachable charge word, at every lattice size
tested, with an exactly uniform size.

**Not true under the definition, and this is the real gap:**

* **It is not spatial.**  All groups sit at `distinct_centers = 1`: what forms is a *membership*
  structure, not a set of spatially separated islands.  "Ilha" in the geometrical sense is not measured
  yet.
* **The aggregating sector is marginal.**  The mode is the singleton: 227 of 235 groups at `L=9`
  (96.6%), i.e. only 8 of 243 sources are inside a size-2 island.  "Islands of equal size" describes
  the *non-singleton* sector exactly, and that sector is small.
* **Nothing grows past 2.**  The same fixed-point bound that makes the size uniform also caps it, so the
  phenomenon is "one bound pair per charge word", not "islands of N bubbles".

## What follows for the next battery (from the definition, not from an abstract premise)

* **The cap is already emergent** -- it comes from the local `D x D` promotion with no partition input.
  That is the part of the definition that is *met*.
* **What is missing is spatial formation, and it needs transport.**  Nothing moves in the reference
  (B0 baseline; J0 `J = 0` identically), so bubbles cannot co-locate into spatially distinct islands.
  The mechanical transport exists (drive pairs: measured 0.25 cells per light frame, B0), but no
  reference process *generates* a driver from the seed.  So the next experiment is not "add a rule that
  makes islands of size N" (that is the retired `L/3` route and it is circular) but: **can an island
  assembled by the reference rules acquire a driver spontaneously, and if it does, do the islands
  separate while keeping equal size?**
* **Falsifier to register before that run:** if the size-2-per-word pattern survives spatial separation
  *and* the sizes stay equal, the phenomenon is both geometric and quantized under this definition.
  If separation destroys the pattern (sizes spread), then the uniform size is a property of the
  frozen co-located plateau only -- and that would be a clean negative.
* **A second, cheaper question already answerable:** why *one* per word and not two, three, or none?
  The fixed-point theorem says "at most one", and the measurement says "exactly one" for every word that
  has any group.  The mechanism that makes it exactly one (rather than zero) has not been isolated; that
  is a local, partition-free question.

## Per-word spectrum: does `ch` organise the population? (20 Sep 2026)

If the charge word were the organising principle of quantization, the spectrum should be **structured by
`ch`** -- by the colour index, by `q`, by `(w0,w1)`, by the matter/antimatter class -- and its coefficient
should come from the 6-bit alphabet (64 words).  The per-word block of
`experiments/analyze_island_spectrum.py` measures exactly that, on censuses already in the repository.

**Reference build, three lattice sizes and three seed multiplicities** (`--frame` defaults to the last):

| L | `W` | multiplicity `W/(9L)` | populated words | non-singletons per word | island size | pair addresses (chief, delegate) |
|---|---|---|---|---|---|---|
| 6 | 108 | **2** | 8 of 8 | **1** | **2** | w=96 (96,97), w=98, 100, 102, 104, 106, 92, 94 |
| 9 | 243 | **3** | 8 of 8 | **1** | **2** | w=241 (241,242), w=220 (220,221), ... |
| 15 | 675 | **5** | 8 of 8 | **1** | **2** | w=643 (643,644), w=648, 653, 658, 663, 668, 673, 638 |

Two things follow immediately, and both are independent of the earlier aggregate reading:

1. **`ch` does not organise the spectrum.**  All eight words carry an *identical* island spectrum: one
   non-singleton of size 2 each, same shape.  The slices by bit field are flat -- `c = 0..7`: one
   non-singleton each; `q = 0` and `q = 1`: four each; `(w0,w1) = 00/01/10/11`: two each; class
   matter/antimatter/anti-neutral: 4/3/1, exactly in proportion to how many words each class contains.  The
   only variation anywhere is the **island count per word**, and it follows the seed's family map
   (17 families per word -> 84 islands; 16 -> 79 at `L=15`), i.e. seed accounting, not `ch`.  So the measure
   of the phenomenon is word-blind: the dynamics uses `ch` only in the equality test `main.ch == mirror.ch`.
2. **The size-2 island is not the seed's family.**  At `L=6` the multiplicity is 2 and the island size is 2
   *coincidentally*; at `L=9` and `L=15` the families carry 3 and 5 copies and the island is still 2.  So
   the aggregated size is a property of the local rule, not of the partition.

**The pair law, refined (all three multiplicities agree).**  In every word the pair is the **top two
addresses of that word's highest-numbered family**: with the family's base at `b` and size `m`, the pair is
`(b+m-2, b+m-1)` and the chief is the lower of the two -- `(96,97)` for `m=2`, `(241,242)` for `m=3`,
`(643,644)` for `m=5`.  The chief is never the maximum address, which is what `promotesDelegate`'s condition
`main.w < mirror.w` predicts (`PAIR_MECHANISM.md`); the new part is that the position inside the family
shifts with `m` while the size stays 2.

**The candidates, where sizes exceed 2, are still word-blind** (frame 10, `L=9`):

| build | spectrum | per word |
|---|---|---|
| `repair` (T2 off + nomination repair) | `30:7 33:1`, spread 3 | one island per word, ~30; the exception is the colour-0 word (chief `w=0`, 33) |
| `shell` (completed rules + shell brake) | `1:1 2:2 3:7 6:1 22:1 26:7`, spread 24 | one island of **26** and one of **3** per word; word 0x00 carries 22 + 6 + 2 + 2 |

So the dynamic candidate's structure is "one big + one small island per word", the same shape in every
word, with the sizes varying by word only at the edges.

**What this does to the `ch`-derived targets.**  A target such as `192L` islands of `L/64` elements is read
off the 6-bit alphabet (`192 = 3 x 64`, `64 = 2^6`).  The measurement says the alphabet has **no dynamical
footprint**: only 8 of the 64 words are populated by the seed, all 8 are treated identically, and the count
of islands per word is set by the seed's family map rather than by any bit.  A coefficient taken from `ch`
is therefore not implied by anything the dynamics does -- it would have to be *installed* by a rule that
reads `ch` (or the partition), which is the retired route.  The two testable clauses such a target implies
are measured and both are flat: **words active = 8/64**, and **islands per word = 1, independent of L**
(6, 9, 15) and of the multiplicity (2, 3, 5).

## Reproduce

    rem aggregate + per-word spectrum (three lattice sizes, three multiplicities)
    python experiments\analyze_island_spectrum.py build\census_corrected build\island_census\ref_el6 build\island_census\L15
    rem the candidates (sizes above 2), at their last frame
    python experiments\analyze_island_spectrum.py build\island_census_parent\repair_run10 build\island_census_parent\shell_run10
    rem pick a frame explicitly (default: the last one present)
    python experiments\analyze_island_spectrum.py --frame 2 build\island_census\ref_el9_s64
