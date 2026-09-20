# A2 -- why exactly one island per charge word, and where the cascade happens

**Question (from the work-plan backlog, item A2).**  The membership fixed point forbids a group larger
than 2 and allows at most one size-2 group per charge word; the reference measurement *attains* the bound
(one delegate per word).  Why one, and not zero?  Which encounter makes the pair?  Where in the transient
does the cascade complete?  Local questions, no new rule needed.

**Method (read-only).**  `CASCADE_LOG` records every encounter that changes a source's role -- tick, both
addresses, both kinds, the charge word and the parent before/after -- and `cascade_probe` dumps the
ledger after each light frame.  With the macro undefined the preprocessor removes the whole block, so the
reference build is unchanged (verified: preprocessed `interaction.cpp` byte-identical to `HEAD`).

```bat
experiments\build_cascade_probe.bat
build\cascade_probe\cascade_probe.exe 2 16384 9      rem 2 light frames, L=9, canonical superposed seed
```

**Measured (reference path, canonical seed, `L=9`, `W=243`, `FRAME=878`).**

| frame | ticks | role changes | S->K | S->D | D->K | K->D |
|---|---|---|---|---|---|---|
| 1 | 0-878 | **0** | 0 | 0 | 0 | 0 |
| 2 | 878-1756 | 12402 | **26** | **6292** | **6084** | 0 |

and per charge word (all eight alike):

    ch=0x00 notes=1664 first_tick=879 last_tick=1120  last=D(w=241)->K(partner=242)
    ch=0x04 notes=1534 first_tick=879 last_tick=1120  last=D(w=229)->K(partner=230)
    ch=0x19 notes=1534 first_tick=879 last_tick=1120  last=D(w=220)->K(partner=221)
    ch=0x1D notes=1534 ... last=D(w=232)->K(partner=233)
    ch=0x2A ... last=D(w=223)->K(partner=224)     ch=0x2E ... last=D(w=235)->K(partner=236)
    ch=0x33 ... last=D(w=226)->K(partner=227)     ch=0x37 ... last=D(w=238)->K(partner=239)

**Three answers.**

1. **Nothing happens in the first light frame.**  Zero role changes over ticks 0-878: the expansion
   transient produces no membership event at all.  The cascade starts at **tick 879**, the first tick
   after the first frame edge -- i.e. it is the frame edge itself (the partner rotation advancing one
   slice) that opens the encounters able to change roles.
2. **The cascade is one event of ~240 ticks and then stops.**  First tick 879, last tick 1120: 241 of the
   frame's 878 ticks (27%).  The remaining ~636 ticks of the same frame change nothing, and the state
   committed at that frame's edge already carries the final structure.
3. **The pair-making encounter is a `D->K` promotion whose partner is the word's MAXIMUM address.**  The
   eight survivors are 242, 230, 221, 233, 224, 236, 227, 239 -- for each word, the top address of its
   last family (the seed assigns one family of `ISLAND_SIZE` copies per word, cyclically; the last eight
   families are 219-221, 222-224, ..., 240-242, and their tops are exactly those addresses).

**Why the sweep must stop there (the mechanism).**  `promotesDelegate` requires `main.w < mirror.w`: a
delegate is promoted only when it meets a delegate of the same word at a **higher** address.  The cascade
therefore propagates *upward in address*, and the word's highest-address delegate can never satisfy the
condition -- there is no higher partner to meet -- so it is the one that remains.  **Zero is impossible**
(no transition demotes a delegate to `S`: `K->D = 0` and `S->D` only ever adds) and **two is impossible**
(they would meet, and the lower one would promote).  The bound is not just attained; it is *forced* by the
address ordering, and the specific survivor is decided by the seed's word-per-family assignment.

## Scaling of the cascade duration T (20 Sep 2026)

The open sub-question above is *why 241 ticks*.  Running the same probe at a larger lattice answers
most of it: **T tracks W, the number of layers, and nothing else.**

| | L=9 | L=12 | L=15 (prediction, run in flight) |
|---|---|---|---|
| `W = 3L^2` | 243 | 432 | 675 |
| `FRAME` | 878 | 1504 | 2285 (measured lengths of frame 1 at that size) |
| frame 1 notes | 0 | 0 | 0 (predicted, same as both smaller sizes) |
| first tick -> last tick | 879 -> 1120 | 1505 -> 1935 | 2286 -> 2959 |
| **T** | **242** | **431** | **674** |
| T / W | 0.996 | **0.997** | 0.999 |
| S->K / S->D / D->K / K->D | 26 / 6292 / 6084 / 0 | **0** / 11232 / 11024 / 0 | -- |

Commands (same probe, no code change):

```bat
build\cascade_probe\cascade_probe.exe 2 16384 9
build\cascade_probe\cascade_probe.exe 2 16384 12
```

**Reading.**  `T = W - 1` at both measured sizes: the cascade occupies exactly one partner-rotation
period (one slice of `rotatePartners()` per light frame, W slices per full rotation), so its duration is
set by the *number of layers*, not by the number of sources, not by the lattice volume, and not by the
seed's family multiplicity.  This also explains why the cascade starts on the first tick of a frame: the
frame edge is where the rotation advances a slice.

**A second by-product.**  The election channel `S x S` does not survive the change of size: it produces
26 chief births at `L=9` and **zero** at `L=12`.  Structure is built by the promotion cascade D->K
(6084 and 11024 notes respectively), with the S->D adoptions feeding it.  So the seed's election channel is
not merely marginal; at `L=12` it is absent altogether.

**Still open.**  Why the cascade length is `W - 1` rather than `W` (the last slice of a rotation apparently
finds nothing left to change), and whether the same law holds once islands have extent (the shell/escape
builds, where a promotion is no longer the only role transition).

**A by-product worth recording.**  The structure is built overwhelmingly by **promotion**, not by
election: `D->K = 6084` notes against `S->K = 26`.  The seed's `S x S` election channel is marginal
(26 events, i.e. a few per word); what creates the chief population is the promotion cascade, fed by
`S->D` adoptions (`6292`).  That is consistent with the earlier finding that disabling T2 collapses the
structure (1 chief, 241 unresolved) unless the nomination repair is added.

**What this closes and what it opens.**

* Closed: the *stopping point* (forced by `main.w < mirror.w`), the *survivor identity* (the word's
  maximum address), and the *timing* (one frame, starting at the first tick after a frame edge).
* Open: **why 241 ticks**.  The sweep needs a specific number of partner-rotation steps to reach the top
  address; the relation between that number, the frame length and the seed's family layout is not derived
  here.  Also open: whether the same stopping rule holds once an island has **extent** (the shell-brake
  and nomination-repair builds, where the cascade is no longer the only transition).
