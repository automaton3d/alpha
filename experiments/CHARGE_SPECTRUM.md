# The charge spectrum that the rule set actually allows (item 3)

**Question.** All four positive routes to charge quantisation are closed, so the charge
unit `L/3` is an axiom.  Item (3) of the menu asks the weaker but still positive question:
what does the *rule set itself* force about the charge spectrum?  This note answers it from
the code, with an exhaustive enumeration, and verifies the answer against a live run.

**Artifacts.** `experiments/analyze_charge_spectrum.py` (stdlib only) and its saved output
`build/charge_spectrum.txt`; census logs `build/mm0.txt`, `build/mm1.txt`.

## 1. The seed's charge words (code authority, not prose)

`initSim.cpp:76-80` builds every cell's word from its island index:

```
w0 = island % 2 ; w1 = (island >> 1) % 2 ; q = w0 ^ w1
ch = (island % 8) | (q << 3) | (w0 << 4) | (w1 << 5)
```

| island mod 8 | word | q | w0 | w1 | colour | class |
|---|---|---|---|---|---|---|
| 0 | `0x00` | 0 | 0 | 0 | N | matter |
| 1 | `0x19` | 1 | 1 | 0 | R | matter |
| 2 | `0x2A` | 1 | 0 | 1 | G | matter |
| 3 | `0x33` | 0 | 1 | 1 | Bbar | antimatter |
| 4 | `0x04` | 0 | 0 | 0 | B | matter |
| 5 | `0x1D` | 1 | 1 | 0 | Gbar | antimatter |
| 6 | `0x2E` | 1 | 0 | 1 | Rbar | antimatter |
| 7 | `0x37` | 0 | 1 | 1 | Nbar | anti-neutral (counted anti) |

The generator explains *why* exactly eight words appear: `island % 8` is the colour, whose
low two bits reproduce `w0` and `w1` (`c0 = w0`, `c1 = w1`), leaving only `c2` free, and
`q = w0 ^ w1` is then determined.  So the seed realises the **diagonal `c0 = w0, c1 = w1`
of the 32-word manifold `M = {ch : q ^ w0 = w1}`, i.e. 8 of 64 words.**

## 2. The reachable write set and a rule-reachability theorem

Only two places write `cell.ch`: the seed (`initSim.cpp:80`) and the gated conjugation hook
(`simulation.cpp:872`, `ch ^= 0x1F`, exact no-op unless `mm_eps != 0`; default
`mm_eps = 0.0` in `config.h:61`).  `ch ^= 0x1F` flips colour, `q` and `w0` while preserving
`w1`, hence preserves `q ^ w0 = w1`: **every word any cell can ever hold lies in `M`.**

Exhaustive enumeration over `M` against the real predicate (`interaction.cpp:147-173`):

| rule | pairs inside `M` | verdict |
|---|---|---|
| R1 graviton-type (full 6-bit complement) | 0 | **algebraically impossible** |
| R4 antineutrino-type (`0x3F/0x3F`) | 0 | **algebraically impossible** |
| R5 up-quark-type (q=0, w1=0, w0=1) | 0 | **algebraically impossible** |
| R2 photon-type | 16 | exactly the pairs `{w, w ^ 0x1F}` |
| R3 neutrino-type | 1 | `0x00/0x00` |
| R6 up-quark-type (q=1, w1=1, w0=0) | 6 | the six words `0x28 | c`, `c = 1..6` |

Three of the six channels are therefore **closed for every admissible word**, and the R2
channel is exactly the "word + its conjugate" geometry.  Consequence:

* With the canonical seed and the default configuration, the words are frozen at the eight
  seed words and **only R3 (`0x00/0x00`) and R6 (`0x2A/0x2A`, `0x2E/0x2E`) survive
  `canFormPair`** — the seed's words contain no conjugate pair (its conjugates are
  `0x1F, 0x1B, 0x06, 0x02, 0x35, 0x31, 0x2C, 0x28`, none of which the seed generates).
  **Section 5b corrects this**: "survives `canFormPair`" was read from the predicate alone,
  and the probe shows that the membership election intercepts every *identical-word* pair
  before the pair branch is reached, so R3 and R6 never fire either.
* **The photon (R2) channel is dormant in every canonical run**, not because it is weak but
  because the charge geometry it needs does not exist in the lattice.  This is the
  charge-side reason behind three earlier negatives (the electric channel `conv_repel` never
  firing in item (1), the family-selective design's null result, and `PHOTON_MEDIATION_FAR`).
  To open it without planting charges, the conjugation hook must run (`mm_eps != 0`), which
  adds the eight conjugate words and thus the 16 R2 pairs.
* R1 has no route even with the hook: `NOT w` leaves `M`, so a fully complementary pair can
  only be *planted* — which is exactly what the probes do (`0x00/0x3F`, `0x00/0x1F`).

Note also that `ch` is the model's identity label (`island_identity.h:6`,
`chief_transition.h`): with only 8 (or 16) distinct words and 10-11 families per word, no
`ch`-based predicate can distinguish families that share a word.  Any "distinct-family"
mediation channel is therefore blind by construction, which is consistent with the negative
item-C result and the P7 co-location control.

## 3. Matter/antimatter inventory of the seed: derived, then verified live

Signature rule (manuscript Sect. "Charges"; code `charges.cpp:177`, `popcount3(ch & 0x07) < 2`
— arithmetic sum and popcount agree for three bits): matter if `sig < 2`, else antimatter,
with colour `111` the anti-neutral word.  Applied to the seed's eight words:

* matter: `0x00` (N), `0x19` (R), `0x2A` (G), `0x04` (B) — 4 words
* antimatter: `0x1D`, `0x2E`, `0x33` — 3 words; anti-neutral: `0x37` — 1 word

Because `81 = 8 x 10 + 1`, the single extra family (island 80) carries word `0x00`, so the
seed's *bookkeeping* is 41 matter islands against 40 antimatter islands.  Each island is
`ISLAND_SIZE x 3^6 = 3 x 729 = 2187` cells, so the seed's starting asymmetry is exactly
`D_init = +2187` cells.  A live canonical run (L=9, 16 frames, `island_census`,
`build/mm0.txt`) reports at every sampled tick:

```
freeM=89667  freeA=87480  freeD=+2187      (and the closure identity Dtot==dPair+freeD: OK)
```

and the per-layer census (`[charges] w=... mat=729 anti=0`) is uniform, i.e. exactly the
seed's word assignment.  41 x 2187 = 89667 and 40 x 2187 = 87480: **the derivation matches
the simulator to the cell.**  This is an initial-condition asymmetry of the word inventory,
not a dynamical excess — it says nothing about the 83:3 hand-census ratio of Appendix B.

## 4. The spectrum itself, and where the axiom still sits

The electric charge is carried by the `q` bit and a bound state is a *count of fragments*, so
the model's spectrum is **integral in units of one fragment**; a spectrum in thirds requires
the multiplicity axiom `ISLAND_SIZE = L/3`, i.e. the seed's partition, not the algebra.  Item
(3) therefore does not reopen quantisation, and this note claims no quantisation.  What it
does deliver is a derived statement about the charge *pattern*: the 8 reachable words, their
matter/antimatter split, the 41:40 initial inventory, and the fact that of the six pair
channels the model implements, three are algebraically closed and the other three are exactly
`R3` and `R6` (canonically) plus `R2` (only with the hook enabled).

## 5. Verification of the published census arithmetic (Appendix B, Table `combina`)

`analyze_charge_spectrum.py` recomputes the table's own numbers:

* all 13 rows sum to Orbis 98,298 + Umbra 98,289 = **196,587 = the published total** ✓
* `W + Z` aggregate: 24,592 + 6,122 = 30,714 and 18,416 + 12,294 = 30,710 ✓
* charged species: 42 + 10 + 31 = 83 matter against 2 + 1 + 0 = 3 antimatter = 27.7:1 ✓

The arithmetic is sound.  Two caveats worth stating in the paper: the table is a *hand
scripted* recombination of **all 64 words** with hand-chosen probabilities, so (i) it does not
share the seed's eight-word reachability (section 1), and (ii) its gluon-type `36,760` and
photon-type `20` entries use R1/R2 geometry that the automaton itself cannot realise
(section 2).  The proton--electron ratio 1187 is likewise not recomputable from the table
alone, consistent with the appendix's own "scale-of-consistency coincidence" status.

## 5b. Measured: which pair channels actually fire (probe)

The reachability table of section 2 is *algebra*: it asks which word pairs `canFormPair`
accepts.  Whether such a pair can ever *form* is a different question, because the pair branch
sits **after** the membership transitions in `encounter()`.  `experiments/pair_channel_probe.cpp`
plants co-located, in-phase `S` sources with chosen words in a `15x5x5` tube, presets the sieve
bit and sets `s2b_target = 1` (so the electroweak gate is open, as in `island_census`), and
reports every formation.  It measures rule acceptance, not sieve timing.  The model now carries
a `PAIR_WORD_LOG` macro that prints the words of every formation, so a long canonical run can be
checked the same way.

| mode | planted words | pre-registered | measured | verdict |
|---|---|---|---|---|
| `r1` | `0x00` / `0x3F` | 1 pair | **1 pair** (`0x00/0x3F`) | confirmed |
| `r2` | `0x00` / `0x1F` | 1 pair | **1 pair** (`0x00/0x1F`) | confirmed |
| `r2b` | `0x2A` / `0x35` | 1 pair | **1 pair** (`0x2A/0x35`) | confirmed |
| `r3` | `0x00` / `0x00` | 1 pair | **0** | **falsified** |
| `r5` | `0x11` / `0x11` | 1 pair | **0** | **falsified** |
| `r6` | `0x2A` / `0x2A` | 1 pair | **0** | **falsified** |
| `r4`, `r6b` | `0x3F/0x3F`, `0x2E/0x2E` | 0 (diagnosed) | 0 | confirmed |
| `cross` | `0x00` / `0x19` | 0 | 0 | confirmed |
| `seedset` | the eight seed words + repeats of `0x00`, `0x2A`, `0x2E` | 3 pairs | **0** (also 0 after 40 frames) | **falsified** |

**The rule behind the four failures.**  R3, R4, R5 and R6 all require the two words to be
*identical* (R3 and R4 explicitly; R5 and R6 demand the same `q`, `w1`, `w0` and the same
non-trivial colour, which is the same word).  Two identical-word `S` sources are consumed by
`chiefContact` (T1) inside the same call, which turns one into a chief and the other into its
delegate; the following `internal` test then sees two bodies of one group and returns **before**
the pair branch.  The election therefore shadows every identical-word rule, and only the two
rules whose words *differ* — R1 and R2 — can ever form a pair.  This is not a timing effect: the
`r3` run was repeated with 40 frames and still formed nothing.

**Consequences.**

* The dynamic pair channel of the model is exactly `{R1, R2}` — i.e. the graviton-type and
  photon-type geometries, both of which need words the canonical seed never generates
  (`0x3F`, or a conjugate).  **In a canonical run no pair forms at all**, which is why the
  16-frame census run read `pairM = pairA = 0` at every sampled tick and why the electric
  channel was never observed to act (items 1 and 4, `PHOTON_MEDIATION_FAR`, item C).
* R5 is dead twice over: algebraically outside `M`, and dynamically shadowed because its two
  words are necessarily identical.
* The ledger row `S = 64: 145 K + 5 D + 93 P halves` in
  `DYNAMIC_QUANTIZATION_DERIVATION.md` is **inconsistent with the current rule order**: 93 pair
  halves cannot form from the seed's eight words under this branch ordering.  The row needs a
  log or a re-derivation (the lint lists it too).  Candidates for its origin: a run whose seed
  carried planted pair words, or a `P` count taken with the pair words present as initial
  conditions.
* `PAIR_WORD_LOG` makes the check cheap for whoever wants to settle it: any canonical run that
  prints more than zero `[pairword]` lines falsifies the statement above.

## 6. Conjugation-hook probe (started, not finished)

To test the dormancy claim empirically the census harness gained one optional argument
(`experiments/island_census.cpp`: `argv[5] = mm_eps`, default absent = 0, so every existing
invocation is unchanged; `build_island_census.bat` rebuilds it).  Two arms were launched,
`mm_eps = 0` (`build/mm0.txt`) and `mm_eps = 1` (`build/mm1.txt`), L=9, 16 frames
(`FRAME = 878` ticks/frame, so 14,048 ticks).

Observed before the runs were stopped (~1025 ticks):

* the hook is genuinely entered in both arms: `[mm] init eps=0 ...` / `[mm] init eps=1 ...`;
* the conjugation latch is not yet armed: `[mm] probe tick=1024 draft_t=1 RMAX=4 a=0`; with
  `draft_t` advancing 0 -> 1 in ~1024 ticks, the first opportunity (`t == RMAX = 4`,
  `simulation.cpp:851`) lies near tick ~4000;
* consequently both arms are identical so far, and **no pair has formed**: at ticks 1, 257,
  513 and 1025 both logs read `pairM=0 pairA=0 form=0 blob=0 ann=0`.

Status: incomplete (the runs were stopped to release the executable, which locks rebuilds).
The natural next step is a finished pair of runs; the prediction to test is that `mm_eps = 1`
conjugates matter islands at their turnarounds, adds the eight conjugate words, and is the
only configuration in which R2 pairs appear at all.

## 7. Falsifiable statements

1. In any canonical run (`mm_eps = 0`) **no pair forms at all**: the only dynamically reachable
   rules are R1 and R2, and both need words the seed never generates.  (Testable with
   `PAIR_WORD_LOG` -- any canonical run printing a `[pairword]` line falsifies it.  Measured:
   the probe's `seedset` mode plants the seed's own words and forms nothing in 40 frames.)
2. The seed's cell census starts at `freeM = 89667`, `freeA = 87480`, `D = +2187` for L=9 —
   41 matter islands against 40 antimatter, at `ISLAND_SIZE x 3^6` cells each.  (Verified.)
3. No macro re-opens R1/R4/R5: R1 needs a word outside the manifold `M` and R4/R5 are shadowed
   by the membership election (their words are necessarily identical), so their closure survives
   every candidate build.

## Limits

* `canFormPair` is a pure function of the two words, so section 2's closure statements are
  exact; whether an open channel is *exercised* is dynamical and remains partly unresolved
  (the 93 pair-halves of the long WP3.3 runs were not reproduced in the 16-frame probe).
* The `83:3` and `1187` numbers remain hand-census results; only their arithmetic is verified
  here, not their provenance.
* Section 6 is a partial result and is labelled as such.
