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

## Reproduce

    python experiments\analyze_island_spectrum.py build\census_corrected build\island_census\L15 build\island_census\ref_el9_s64
