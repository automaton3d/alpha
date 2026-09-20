# The spin programme: `J` is structurally zero, and once a rule consults it, it protects

**Why this exists.** The dynamic-quantisation line ended with a frozen bound: the
membership transitions `T1`-`T5` cap a group at population 2, and every attempt to
obtain anything larger either read the seed multiplicity (`DD_INTRA_ISLAND_FIX`,
`EXCLUSION_FSM`, `w / ISLAND_SIZE`) or produced no interior balance at all
(`SURFACE_ESCAPE_FSM`, `PFI` of `PREDICTED_NS_RULE.md`; flux harness `FAIL`,
`b = +0.030`, `TURNOVER_ABLATION.md`).  What every one of those mechanisms lacks is
**circulation**.  A pile of members can be deformed continuously from 3 to 2; a
rotating one cannot, because winding has to be carried away by something.

So the programme is: give the group a conserved spin

    J = sum over the members   r x m          r = member - chief (toroidal), m = member momentum

and ask, in order, five falsifiable questions:

| step | question | instrument | state |
|---|---|---|---|
| **J0** | is `J` even non-zero anywhere today? | `island_census` + `J0_spin_reader.py` | **DONE: no, structurally** |
| **J1** | does `J` protect a group against an existing threat? | `spin_probe.cpp`, `SPIN_GATED_FSM` | **DONE: yes, decisively** |
| J2 | does `J` *select* a population (`dN` vs `N` crossing)? | `spin_ring_probe.cpp`, coherent vs cancelling momenta | **DONE: no -- protects, does not select** |
| J3 | is `J` conserved -- does it leave as photons? | `dJ` vs `P` birth correlation | open |
| J4 | does size scale with cavity (`RMAX`) and not multiplicity? | same rules, `L = 9` vs `L = 15` | open |
| J5 | does mixing different words raise the ceiling? | `CROSS_GLUE` | open |

`J` contains **no `L`, no `L/3`, no `ISLAND_SIZE`**: it reads only `w`, `parent`,
`lcenters[]` and `m[]`.  That is the whole point -- it is the first quantity in this
project that could grow a population quantum without restating the seed.

## J0 -- the baseline: `J` is structurally zero

**Instrument.**  `experiments/island_census.cpp` gained `chiefSpin()` (J = sum r x m
with toroidal offsets, `int64` accumulators), per-frame `spin_groups` and
`max_Jmag2` columns in `census.csv`, and `Jx, Jy, Jz` per group in `groups.csv`.
`experiments/J0_spin_reader.py` (stdlib only) reads those columns, and falls back to
the pre-J0 logs by reporting `NO-J-COLUMNS` rather than pretending.

**Result over every archive in the repository:**

| archive | groups | nonzero `J` | max `|J|^2` | J columns |
|---|---|---|---|---|
| `build/island_census/J0_el9` (new, instrumented, `L=9`, `S=16384`) | 705 | **0** | **0** | yes |
| `build/island_census/L15` | 2001 | 0 | 0 | pre-J0 |
| `build/island_census/ref_el6` | 500 | 0 | 0 | pre-J0 |
| `build/island_census/mm0`, `mm1` | 0 | 0 | 0 | pre-J0 |
| `experiments/census_ref_el9_s64` (open gate, `S=64`) | -- | 0 | 0 | pre-J0 |
| **total** | **3206** | **0** | **0** | |

**Why it is zero -- and this is the useful part.**  It is not that rotation happens to
cancel; it is that rotation is never *supplied*.  `inertia_fixture::prepare()`
(`experiments/inertia_fixture.h:65-70`) assigns `s.m[axis] = RMAX * direction` **only
for the propeller layers** (`prop`), so every body -- `K`, `D`, `S` -- keeps
`m = (0,0,0)`.  And in the production path `m` is not self-propulsion anyway: it is
preserved (`utils.cpp:181-183`, the copy; `simulation.cpp:413-415`: "the long-term
momentum-direction vector m is preserved; only reloc is consumed") and `interaction.cpp` never
writes it (it only reads it: `:729` for `J`, `:1541-1543`/`:1867-1869` for `reloc`).  It enters
motion only when a *partner* contributes its `m` to `reloc`
(`interaction.cpp:1541-1543`, `1867-1869`), which `applyMomentum()` (`simulation.cpp:423`)
then consumes.  A body's own momentum
therefore never moves it.

So J0 closes with two statements: (i) the measurement exists and reads zero
everywhere, and (ii) zero is an artefact of the *seed*, not a dynamical result --
which makes any later non-zero `J` attributable to whatever supplied it.

## J1 -- the question: does circulation protect?

**The threat already exists.**  `SURFACE_ESCAPE_FSM` (candidate of item 4,
`PREDICTED_NS_RULE.md`) releases a delegate that shares no same-charge shell overlap
with its own group for one full partner rotation (`W_USED` light frames).  It was
already measured to bite: on the `p7` `mid`/`edge`/`spread` geometries it releases
both delegates (`escapes = 2`), on `co`/`chain`/`gap2` it releases none.

**The candidate rule.**  New macro `SPIN_GATED_FSM`, always built together with
`SURFACE_ESCAPE_FSM`; every line sits inside the `#ifdef`, so the reference build is
untouched (verified: the plain `island_census` build still compiles and the diff is
`+44` lines, all guarded).

* **S1 (chief frame).**  Once per frame, before the release test, accumulate
  `J = sum r x m` for each chief from its delegates (`r` via the existing toroidal
  `delta(g, w, axis)`; a chief contributes `r = 0`).  Integer only.
* **S3 (leave-veto).**  A delegate whose escape timer has expired is **not**
  released while its chief's `J != 0`: the departure would carry the spin away.
  The timer is *not* reset, so a group that later sheds or cancels `J` is released
  on a later frame -- the veto is a conservation constraint, not a freeze.

**The configuration -- a frozen rotor.**  `experiments/spin_probe.cpp`, tube
`15 x 5 x 5`, `W = 3`, `RMAX = 2`, contact range `2*RMAX = 4`; `K` at `x = 2`, the two
delegates at `x = 7` and `x = 12`, all at `y = z = 2`.  Every pairwise **toroidal**
distance is `5 > 4`, so no member ever contacts another: nothing promotes, nothing
else releases, and `SURFACE_ESCAPE_FSM` is the only agent that can act.  Two arms:

* `j0` -- every `m = 0`, so `J = (0,0,0)`.
* `jnz` -- the delegates carry **opposite** transverse momenta, `m = (0,+1,0)` and
  `m = (0,-1,0)`, so `sum(m) = 0` (zero net linear momentum: the geometry cannot
  drift) while `J != 0`.  The toroidal offsets make both terms add:
  `r = (+5,0,0)` with `m = (0,+1,0)` gives `(0,0,+5)`, `r = (-5,0,0)` with
  `m = (0,-1,0)` gives `(0,0,+5)`, hence **`J = (0,0,+10)`**, `|J|^2 = 100`.
  The log prints each member's `m` and `r`, so this is checkable from the artifact.

**Predictions, registered before the run** (and printed by the harness itself, so
they are part of every log file):

```
ref , j0  -> escapes 2, final 1K+2S   (baseline; matches the p7 spread regime)
ref , jnz -> escapes 2, final 1K+2S   (m alone gates nothing: J is inert)
gate, j0  -> escapes 2, final 1K+2S   (no circulation to conserve)
gate, jnz -> escapes 0, final 1K+2D   (S3: the spin cannot leave)
```

**Results** (`build/spin_probe/{ref,gate}_{j0,jnz}.log`, 12 light frames each; the
harness reports the verdict itself):

| build | arm | initial `J` | `escapes` | `spin_vetoes` | final | verdict |
|---|---|---|---|---|---|---|
| `ref` (threat alone) | `j0` | `(0,0,0)` | **2** | 0 | `1K+0D+2S` | CONTROL OK |
| `ref` (threat alone) | `jnz` | `(0,0,10)` | **2** | 0 | `1K+0D+2S` | CONTROL OK |
| `gate` (threat + S1/S3) | `j0` | `(0,0,0)` | **2** | 0 | `1K+0D+2S` | CONTROL OK |
| `gate` (threat + S1/S3) | `jnz` | `(0,0,10)` | **0** | **20** | `1K+2D+0S` | **SPIN PROTECTS** |

All four registered predictions are confirmed exactly.  The decisive cell is the
last one: with the same geometry, the same escape rule and the same number of
expired timers, the only difference is that the group carries `J = (0,0,10)`, and the
group survives -- `escapes = 0`, `spin_vetoes` climbing by exactly 1 per frame per
delegate (2 per frame, 20 over 10 tested frames), final population 3 where every
other cell ends at population 1.

## What J1 establishes

1. **`J` is inert by itself.**  `ref + jnz` behaves exactly like `ref + j0`: adding
   the rotor's momenta changes nothing, because no existing rule reads `J`.  This is
   the control that makes the fourth cell meaningful -- the protection is attributable
   to S3, not to the momenta having perturbed the trajectory.
2. **`J` is load bearing once a rule consults it.**  This is the first measured case
   in the project where a group's membership is decided by its *circulation* rather
   than by a read of the seed multiplicity.  `EXCLUSION_FSM` and
   `DD_INTRA_ISLAND_FIX` both keyed on `w / ISLAND_SIZE`; S3 keys on
   `sum r x m`, which contains no `L` and no `L/3`.
3. **The veto is a constraint, not a freeze.**  `spin_vetoes` increments every frame
   while `escapes` stays 0, and the timer keeps running, so the group is released the
   moment its `J` goes to zero or is carried away.  S3 therefore states a conservation
   law -- "spin cannot be destroyed by a departure" -- rather than pinning members in
   place.
4. **The programme is now testable.**  J0 supplies the baseline and the columns; J1
   supplies the first positive and a one-file harness; J2-J5 are sharp questions with
   registered falsifiers (below) rather than open-ended searches.

## J2 -- does `J` *select* a size, or only protect a size?

**The structural argument first.**  S3 as written tests

    if (chiefJ[g][0] || chiefJ[g][1] || chiefJ[g][2])   -> veto

which is a **boolean**: it has no dependence on the population `N`, nor on the
magnitude `|J|`.  Protection should therefore be *flat* in `N` -- every group with
`J != 0` is kept, regardless of how many members it has.  If that is what the
measurement shows, then `J` protects but does not select, and the missing
ingredient is a *magnitude*-dependent cost (the `|J|` cap / spin fission of S4),
not a stronger veto.  Registering the flat curve is what makes an S4 test
meaningful later.

**Configuration.**  `experiments/spin_ring_probe.cpp`, tube `61 x 5 x 5`
(`RMAX = 2`, contact range `4`), chief at `x = 30`, `N` delegates on the long axis,
every pairwise toroidal distance `>= 5 > 4`: no member ever contacts another, so
nothing promotes, nothing else releases, and the escape rule is the only agent.
Three arms:

* `j0` -- members at `+5k`, every `m = 0` -> `J = (0,0,0)`, no momenta.
* `coherent` -- members at `+5k`, every `m = (0,+1,0)` -> `J = (0,0,5*N*(N+1)/2)`.
* `cancel` -- members on **alternating sides in pairs** at the same distance
  (`+5,-5,+10,-10,...`), every `m = (0,+1,0)` -> `J = (0,0,0)` exactly.

`coherent` vs `cancel` is the decisive pair: both arms carry the same number of
members with the same individual momenta `m`, and the only difference is whether
the contributions `r x m` add up or cancel.  (For odd `N` the single unpaired
outermost member is given `m = 0`, so `J` is exactly 0 at every `N`.)

**Registered prediction:** `ref` releases all `N` in every arm; `gate` releases all
`N` for `j0` and `cancel`; `gate` releases **0 for every `N`** in `coherent` -- i.e.
`dN` vs `N` has zero slope.

**Results** (36 runs: 2 builds x `N = 1..6` x 3 arms, 8 light frames each,
`build/spin_probe/ring_<build>_N<N>_<arm>.log`):

| build | arm | `J` for `N = 1..6` | escapes | final |
|---|---|---|---|---|
| `ref` | `j0` | `0` | `N` | all released (`1K+0D+NS`) |
| `ref` | `coherent` | `5, 15, 30, 50, 75, 105` | `N` | all released |
| `ref` | `cancel` | `0` | `N` | all released |
| `gate` | `j0` | `0` | `N` | all released |
| `gate` | `cancel` | `0` | `N` | all released |
| `gate` | **`coherent`** | `5, 15, 30, 50, 75, 105` | **0 for every N** | **all held (`1K+ND`)** |

Both halves of the prediction are confirmed exactly.

**The veto arithmetic is fully understood.**  The observed veto counts for
`gate + coherent` are `7, 12, 15, 16, 15, 12` for `N = 1..6`, which is exactly
`N * (8 - N)`: the escape timer expires at tick `W_USED = N + 1`, so over an
8-frame run each of the `N` members is vetoed once per frame for the last
`8 - N` frames.  One veto per member per expired frame, with no free parameter.

## What J2 establishes

1. **Protection tracks `J` as a collective quantity, not the presence of `m`.**
   This is the strongest single result of the two steps.  `coherent` and `cancel`
   have *identical individual momenta* -- same count, same direction, same
   magnitude -- and differ only in whether the `r x m` terms add up.  `coherent`
   survives at every `N`; `cancel` is released at every `N`, exactly like `j0`.
   Whatever protects the group is therefore the **coherence** of the circulation,
   not the motion of its members.
2. **`J` does not select a size.**  With `escapes = 0` for `N = 1..6` alike, the
   protection curve is flat: no `N` is special, and there is no `dN` crossing.
   The registered negative is confirmed, and it is a *structural* consequence of
   S3 being a boolean, not an accident of the geometry.
3. **`dN` vs `N` is therefore the wrong instrument for S3, and the right one for
   S4.**  For a size to be *selected*, the rule must respond to `|J|`, which grows
   with `N` (`|J| = 5*N(N+1)/2` here).  The natural next patch is the `|J|` cap of
   S4 with `J_max` set by the cavity (`RMAX`), which predicts a ceiling at
   `N* ~ J_max / (r*|m|)` -- a quantity that contains `RMAX` and no `L/3`, and
   whose falsifier is a `RMAX` sweep against a seed-multiplicity sweep.

## What it does **not** establish

* **`J` does not select a population.**  This is now *measured*, not merely open:
  J2 shows `escapes = 0` for every `N = 1..6` in the coherent arm, so the
  protection curve is flat and there is no `dN` crossing.  S3 protects a size; it
  does not set one.  No quantum is produced by J1 or J2 -- cf.
  `DYNAMIC_QUANTIZATION_DERIVATION.md` section 6.
* **The rotor is planted, not generated.**  One geometry (`RMAX = 2`, long side 15
  for J1, 61 for J2), one charge word (`0x08`), and members that never move
  (a body's own `m` does not self-propel it -- see J0): nothing here shows that the
  model can *build* circulation, only that it can respect and conserve one.  There
  is no orbital step (S2) yet, so the members do not actually revolve.
* `J` is recomputed from scratch each frame and is not yet conserved across events
  (joining, promotion, pair formation): J3 does not exist yet, so "conserved" is so
  far a statement about the escape test alone.
* No `L`-sweep and no `RMAX` sweep, so nothing is said yet about whether any
  protected or selected size tracks the cavity.

## Falsifiable follow-ups (in order)

1. **S4 / J2b -- can a *magnitude* rule select a size?**  J2 shows the selection
   cannot come from S3's boolean.  Add the `|J|` cap: if `|J| > J_max` with
   `J_max` set by the cavity (`RMAX`), fission (elect a second chief in the far
   half and divide `J`).  Prediction: a ceiling at `N* ~ J_max / (r*|m|)`, which
   contains `RMAX` and no `L/3`.  Falsifier: `N*` unchanged when `RMAX` changes, or
   `N*` tracking the seed multiplicity.
2. **S2 / J2c -- orbital step.**  Give a delegate one face-step orthogonal to `r`
   with sign from `J x r`, so members actually revolve.  This is what makes the
   rim-speed cap (`v <= 1 cell/frame`) physical, and it is the precondition for a
   geometric rather than imposed `J_max`.  Falsifier: no change to the flat curve.
3. **J3 -- does `J` leave as photons?**  Correlate each `|J|` drop with `P` births
   within one frame and two cells.  *Predicted to fail as the code stands:* pairs are
   formed with `m = 0`, so photons cannot carry spin.  That failure is itself the
   finding -- it identifies the missing half of the mechanism.
4. **J4 -- cavity or multiplicity?**  The same rules from a sparse-`S` soup at `L = 9`
   and `L = 15` (not the superposed seed, which plants `L/3` co-located).  If the
   selected size tracks `RMAX` it is geometric; if it does not move it is absolute;
   if it tracks the seed multiplicity the rule has leaked `L/3` and must be rejected.
5. **J5 -- does mixing raise the ceiling?**  Same `N`, all-same-word vs alternating
   words with a cross-word stay flag.  Prediction: pure groups cap at 2-3, mixed ones
   hold more, with larger `|J|`.  The `mixed_charge` column of the census already
   measures the first half.

## Reproduce

```bat
rem J0 -- the baseline (reader only; the census it reads is built by build_island_census.bat)
build\island_census\island_census.exe 4 16384 build\island_census\J0_el9 9
python experiments\J0_spin_reader.py build\island_census\J0_el9 build\island_census\L15 ^
       build\island_census\ref_el6 build\island_census\mm0 build\island_census\mm1

rem J1 -- the threat, with and without the spin gate
experiments\build_spin_probe.bat
build\spin_probe\spin_probe_ref.exe  12 j0
build\spin_probe\spin_probe_ref.exe  12 jnz
build\spin_probe\spin_probe_gate.exe 12 j0
build\spin_probe\spin_probe_gate.exe 12 jnz

rem J2 -- the same build produces the ring harness; sweep N = 1..6 x 3 arms
rem (the `cancel` arm is the decisive control: same momenta as `coherent`, J = 0)
for %B in (ref gate) do for %N in (1 2 3 4 5 6) do (
  build\spin_probe\spin_ring_%B.exe 8 %N coherent
  build\spin_probe\spin_ring_%B.exe 8 %N cancel
  build\spin_probe\spin_ring_%B.exe 8 %N j0
)
```

Timing note: the J2 ring runs are ~10 s each on the reference CPU path (the tube is
`61 x 5 x 5` with `W = N+1` layers), so the 36-run sweep is best launched in parallel.

Note for the J0 census run: `EL` must be a multiple of 3, and the cost grows roughly
as `L^5`, so `L = 9` needs about 53 s per light frame on the reference CPU path.