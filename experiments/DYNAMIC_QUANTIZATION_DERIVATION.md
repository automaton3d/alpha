# Dynamic charge quantization: what the rules actually fix

Non-circular answer to item (A)/(D): instead of asking which parameter makes the
island population reach `L/3`, derive the **fixed-point structure of the
membership dynamics from the reference rules themselves** and read off `N*`.

**Result.** With the reference transitions the island population is quantised to

    N* = 2   (max),   at most one non-singleton group per charge word,

for **every** `L` with `L/3 >= 2`, and independently of the sieve modulus `S`.
The value `L/3` does not appear in any reference transition: it can only be
produced by *adding* a rule that blocks the promotion inside a family
(`DD_INTRA_ISLAND_FIX`), in which case the "quantum" equals the seed's
multiplicity by construction.

All three statements are checked against runs already in the repository (table
in section 4).

## 1. The transitions, exactly as coded

Source kinds: `S` singleton, `K` chief, `D` delegate, `P` pair half.  A *group*
is a chief plus the delegates whose `parent` names it.  Code anchors (every `file:line` here
is relative to `src/`, re-verified 19 Sep 2026 at commit `b901034` against the compiled tree;
the transitions live in `src/include/model/chief_transition.h` and are reached through the
single call site `src/model/interaction.cpp:1592`, `chiefContact(currSrc,partnerSrc,currDraft)`):

    T1  chiefContact, S x S (different layers, same charge word ch)
        -> the minimum W address becomes K, the other becomes D of it
           [chief_transition.h:66-73; the S x S minimum is line 70]
    T2  promotesDelegate: D x D, same ch, main.w < mirror.w
        -> the lower-address delegate becomes K  (group loses one member)
           [chief_transition.h:14-26; invoked at chief_transition.h:60]
    T3  demotesChief: K x K, same ch, main.w > mirror.w
        -> the larger-address chief becomes D of the smaller  (groups fuse)
           [chief_transition.h:30-33; invoked at chief_transition.h:61-64]
    T4  S meets K or D  -> the S becomes D of that chief (group gains one)
           [chief_transition.h:66-73; reached via interaction.cpp:1592]
    T5  different charge words -> no membership change (only the gated
        electroweak branches act)                      [chief_transition.h:38]

    GATE: a contact requires both cells active and of positive radius
          (interaction.cpp:942), i.e. the two wavefronts must overlap.

An earlier revision of this note cited pre-move line numbers (`chief_transition.h:53-56` for T1,
`interaction.cpp:1202-1221` for T4, gate at `:631-633`).  Those anchors pointed into a stale,
never-compiled duplicate of the interaction source (`src/interaction.cpp`, since parked in
`attic/`); the numbers above are the current, compiled ones.  Note that
`chief_transition.h:52-58` is the `DD_INTRA_ISLAND_FIX` candidate *override* of T1, not the
reference transition.

None of T1-T5 reads `ISLAND_SIZE`, `ISLAND_COUNT` or `L/3`: they read `w`,
`parent`, `kind`, `ch` and the **numerical order** of `w`.  The only place the
multiplicity `m = L/3` appears is the seed (`w / m` giving the family index,
`initSim.cpp`) and the two candidate macros.

## 2. Fixed-point theorem

**Theorem.** Let a state be *frozen* (no further membership transition with the
gate open) **and let its same-charge delegates be in contact**, i.e. co-located
at one site with the gate open -- which is the case of the canonical superposed
seed.  Then:

1. every group has population <= 2;
2. within one charge word, at most one group has population 2.

**Proof.**

(1) A group of population >= 3 is `1 K + n D` with `n >= 2`, so it contains two
delegates of the same charge word.  The encounter schedule rotates the partner
lattice by one slice per frame (`rotatePartners()`, `utils.cpp:36`), so every
ordered pair of layers meets within `W` frames.  When the two delegates meet
with the gate open, `promotesDelegate` fires for one of the two orderings
(`main.w < mirror.w`), promoting a delegate to a chief: the group's population
drops by one.  Hence no frozen state has a group of population >= 3.

(2) Suppose two distinct groups of the same charge word each had population 2,
i.e. two delegates `D1`, `D2` with `ch(D1) = ch(D2)`.  They meet within `W`
frames; `T2` promotes one of them, so one of the two groups loses its delegate
(and the promoted delegate becomes a singleton).  Therefore a frozen state has at
most one group of population 2 per charge word.  **QED**

The theorem is a statement about the *reachable* set: it needs the pair to meet
while the gate is open.  The measurement shows the gate closes after the
transient (zero captures/escapes/births/deaths from frame 3 on), which is what
turns "no admissible pair" into "frozen".  So the reachable configuration is a
**corner** of the membership dynamics, not an interior balance.

**Scope (why co-location is in the hypothesis).**  The proof needs the two
delegates to *meet with the gate open*; the contact gate is per cell
(`encounter()`, `interaction.cpp:942`), so delegates sitting at *different*
sites never fire `T2`.  This is not a technicality: the prepared islands of the
inertia campaign (a 3- and a 5-element body in a tube, `INERTIA.md`) transport
with their `K/D` roles conserved over 48-160 frames, i.e. a frozen state with
`N = 5 > 2` exists -- its constituents are spread (max pairwise separation one
cell), so its delegates never meet.  Two consequences:

- the bound describes **what co-location does**: from the superposed seed, where
  all `W` sources share one cell, aggregation *cannot* build a compact island of
  `N >= 3` -- the promotion shatters it.  Formation of a larger island requires
  the constituents to be already spread (a formation route the canonical seed
  does not provide), which is the open formation question of
  `SEED_ASYMMETRY.md`;
- it yields a new falsifiable prediction: **if a prepared island were
  re-concentrated at one site with the gate open, the `D x D` promotion would
  split it** (`P7`).  This has now been run and **confirmed**, see section 8.

**Corollary (the quantum does not scale with L).**  The number of copies of a
charge word is `m = L/3`, but the bound above is 2 for every `m >= 2`.  Hence the
island population is `N* = 2` for every `L`; `L/3` is not an attractor of the
reference rules at any `L`.  Measured at `L = 9`: `max_population = 2` at
`S = 16384` and at `S = 64`.

## 3. The ledger (the "rates" of the manuscript, made explicit)

The membership transitions are **ungated** (T1-T5 fire whenever the contact gate
is open); the sieve modulus only decides how many sources *leave* the membership
game through the gated pair branch.  So the "rates" are integer step functions,
not smooth functions of `N`:

| run | sources entering | membership outcome | gated branch | groups of pop 2 | frozen |
|---|---|---|---|---|---|
| reference, `S = 16384` | 243 | 235 K + 8 D | 0 P | **8** (one per charge word) | 61 journeys |
| gate open, `S = 64` | 150 | 145 K + 5 D | 93 P halves | **5** | 11 journeys |
| `EXCLUSION_FSM` + `DD_INTRA_ISLAND_FIX` | 150 (+93) | 81 K + 162 D | 0 | 0 (all groups have pop 3) | 10 journeys |
| reference, `L = 15`, `S = 16384` | 675 | **667 K + 8 D** | 0 P | **8** | 3 journeys |

Arithmetic closes in every row: `235 + 8 = 243`; `145 + 5 + 93 = 243`;
`81 x 3 = 243`.  In the reference the number of non-singleton groups is exactly
the number of distinct charge words (8: `w0`, `w1` and the 3-bit colour derived
from `nu = w / (L/3)` give `lcm(4, 8) = 8` words), which is the quantitative
content of the theorem's part (2).

## 4. Falsifiable predictions and their status

| # | prediction | status |
|---|---|---|
| P1 | any reference-build census at any `L` (with `L/3 >= 2`) has `max_population = 2` | **confirmed at `L = 9` and `L = 15`**: at `L = 15`, `W = 675`, five copies per family, the frozen state is 667 K + 8 D with `max_population = 2`, `pop == L/3 = 0` (section 9); at `L = 9` for `S = 16384` and `S = 64` |
| P2 | number of non-singleton groups <= number of distinct charge words (8) | confirmed: 8 (reference), 5 (low `S`) |
| P3 | `max_population = L/3` appears iff the intra-family promotion is disabled | confirmed: `prod_exc_fix` gives 81 groups of pop 3 = `L/3` |
| P4 | no reference build shows `pop == L/3 at end` | confirmed: 0 in 64 journeys (`run64`) and 0 in 12 journeys (`sweep_S64_long`) |
| P5 | the frozen state has zero events (a corner, not an interior balance) | confirmed: `captures = escapes = births = deaths = 0` from frame 3 in every run |
| P6 | the observed regression "equilibrium" degrades with `L` instead of converging | consistent: slope `-0.176` (r2 0.17) at `L = 7`, `-0.088` (r2 0.095) at `L = 11` |
| P7 | a prepared island re-concentrated at one site with the gate open splits through the `D x D` promotion | **confirmed** (`p7_concentration`, section 8): reference build 1 promotion at frame 2 and `1K+2D -> 2K+1D`; spread control 0 contacts, roles conserved; `DD_INTRA_ISLAND_FIX` fires contacts but 0 promotions |

`P1` at `L = 15` is the decisive non-circular falsifier: it needs no candidate
macro, only the ordinary build with `EL = 15`, `W = 675` (`BLOCK = 2.28 x 10^6`
cells, ~13x the `L = 9` cell count and ~2.6x its tick count, i.e. ~33x the cost
per frame).  The harness takes the lattice side as an optional fifth argument
(`island_census [frames] [sieve] [outdir] [EL]`, default 9; `EL` must be a
multiple of 3):

    build\island_census\island_census.exe 4 16384 build\island_census\L15 15

Any `max_population != 2` there falsifies the theorem; a `max_population = 2`
(and `pop == L/3 = 0`) would confirm that the model's own quantum is 2, not
`L/3`.

## 5. Why the `Gamma_cap(N) = Gamma_esc(N)` formulation misleads

The manuscript defines dynamic charge quantization by a rate crossing,
`Gamma_cap(N*) = Gamma_esc(N*)`.  In this rule set the crossing is degenerate:
at the frozen configuration both rates are **zero**, so `N*` is selected by the
*reachable set* of the transition rules, not by a balance of opposing flows.
Consequences:

- the "equilibrium" regressions measure the transient, not a homeostat (P6);
- the population is fixed by combinatorics (address order + charge-word
  multiplicity), not by a shell/geometry argument;
- consequently no choice of `S`, `U0` or `L` can move `N*` to `L/3`: the only
  lever is a rule that removes T2 inside a family.

## 6. What a genuinely dynamical quantum would require

To make `L/3` an attractor, a rule must make a group of size `m = L/3` the
*stable* configuration, e.g. by having the escape rate exceed the capture rate
only above `m`, with the crossing produced by `L`-dependent geometry.  The
current shell capacities argue against such a mechanism at `m = 3`: the first
shells hold 26, 66, 158 cells at `r = 1, 2, 3` (the free-sphere and toroidal
conventions coincide up to `RMAX`), and the contact graph at the turnaround is
complete for every `L <= 15`, so the contact capacity is `W` rather than a small
integer.  The hypothesis that the quantum is geometric was tested explicitly and
falsified -- see the companion note `GEOMETRIC_QUANTUM.md`; what the
present derivation establishes is only the negative direction: the reference
rules quantise the island population to **2** and cannot produce `L/3`.

## 7. Reproduce

```bat
rem reference build, canonical seed: 235 K / 8 D, max pop 2, 8 non-singleton groups
build\island_census\island_census.exe 12 16384 build\island_census\check_ref

rem same at an open gate: 145 K / 5 D, max pop 2, 5 non-singleton groups
build\island_census\island_census.exe 12 64 build\island_census\check_s64

rem with the intra-family promotion disabled (candidate): 81 groups of pop 3 = L/3
build\island_census\island_census_exc_fix.exe 12 16384 build\island_census\prod_exc_fix
```

The group histogram is in `groups.csv` (columns `population`, `seed_families`,
`charge_words`); the counters and `max_population` are in `census.csv`.

## 8. P7: the co-located island splits (measured)

Harness `experiments/p7_concentration.cpp`, build
`experiments/build_p7_concentration.bat` (two binaries: reference and
`DD_INTRA_ISLAND_FIX`).  It plants a prepared `1K + 2D` island of one charge word
(`w = 0,1,2`, `ch = 0x08`, `ISLAND_SIZE = 3`) in a 15x5x5 tube, where `RMAX = 2`
so two wavefronts can meet only while their centres are within `2*RMAX = 4`
cells; the encounter compares the SAME cell `(x,y,z)` in the two layers, so an
overlap is exactly what fires T2.

    build\p7_concentration\p7_concentration_<ref|fix>.exe [frames] [co|spread]

* `co`: all three constituents at one site (7,2,2);
* `spread`: the constituents 5 cells apart along the long axis, i.e. beyond the
  contact range.

| build | mode | contacts per frame | promotions | final roles | max group |
|---|---|---|---|---|---|
| reference | co | 0, 0, **156**, 396, 156, 0, 156, 396, ... | **1** (the delegate `w=1` promoted at frame 2) | **2 K + 1 D** | **2** |
| reference | spread | **0 in every frame** | 0 | 1 K + 2 D | 3 |
| `DD_INTRA_ISLAND_FIX` | co | identical to the reference row | **0** | 1 K + 2 D | 3 |
| `DD_INTRA_ISLAND_FIX` | spread | 0 | 0 | 1 K + 2 D | 3 |

Three things are established.  (i) The promotion fires in the first journey in
which the shells overlap (frame 2, the same journey as the canonical cascade),
the co-located island loses one member and settles at population 2: the
theorem's bound reached dynamically.  (ii) Beyond the contact range there are
**zero** contacts, so the spread island keeps `1K + 2D` for the whole run --
the scope clause measured, and the reason the inertia islands survive transport.
(iii) The candidate rule changes nothing about the geometry and everything about
the transition: contacts are identical in the `fix` build, the promotion is
absent, population 3 survives.  The candidate's `81 x 3` positive is therefore
exactly the removal of T2 inside a family, now isolated at rule level with a
control that separates the geometry from the transition.

## 9. The lattice-side falsifier: `L = 15` confirms the bound

Run with the optional lattice side (`island_census [frames] [sieve] [outdir] [EL]`):

    build\island_census\island_census.exe 4 16384 build\island_census\L15 15

`EL = 15`, `W = 675` (135 families of 5 copies), `RMAX = 7`, `FRAME = 2285`
ticks, wall `7460 s` (31 min/frame, i.e. the predicted ~33x the `L = 9` cost per
frame).  Canonical superposed Platonic seed, reference build, reference sieve:

| frame | tick | K | D | S | P | centres | groups | unresolved | mixed | max pop |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 675 | 0 | 1 | 0 | 0 | 0 | 0 |
| 1 | 2285 | 0 | 0 | 675 | 0 | 1 | 0 | 0 | 0 | 0 |
| 2 | 4570 | **667** | **8** | 0 | 0 | 1 | 667 | 0 | 0 | **2** |
| 3 | 6855 | 667 | 8 | 0 | 0 | 1 | 667 | 0 | 0 | 2 |
| 4 | 9140 | 667 | 8 | 0 | 0 | 1 | 667 | 0 | 0 | 2 |

Report card: `chiefs_total=667 chiefs_at_end=667 localized_at_end=8
pop==L/3_at_end=0`.

What it establishes:

- **`max_population = 2` with a multiplicity of five.**  The bound does not
  track `L/3`: the frozen population is 2 at `L = 9` (multiplicity 3) and at
  `L = 15` (multiplicity 5) alike, and `pop == L/3 = 0` in both.  The prediction
  `P1` is confirmed at a second lattice size, with no candidate macro involved.
- **The per-charge-word count is exactly the number of charge words.**  `K = W - 8`
  and `D = 8` at both sizes (`675 = 667 + 8`, `243 = 235 + 8`): one delegate per
  charge word and every other source a singleton chief.  The theorem's minimum
  (at most one group of population 2 per charge word) is *attained*, not merely
  satisfied -- the sharpest form of the result.
- The corner behaviour repeats: no capture, escape, birth or death from frame 3
  on (frames 3 and 4 are identical).



