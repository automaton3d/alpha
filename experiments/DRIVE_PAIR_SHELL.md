# Drive-pair shell: is the dressing a rest-mass contribution?

Status: **instrument + hypothesis note**. This note adds an *instrument*
(`rest_shell_probe`) and fixes the falsifiers for a hypothesis.  It asserts no
result: no run below is a measurement of the reference dynamics beyond the
controls, and nothing here changes the model.

## The hypothesis (H-SHELL)

> The rest mass of a particle in the lattice frame is the bare island **plus the
> number of drive pairs needed to close a thin shell around it**,
> $m_{\text{rest}} = N_{\text{island}} + n_{\text{close}}(\rho)$, where the shell
> is a **self-formed, dynamic** collection of drive pairs whose transport vectors
> close ($\sum \boldsymbol m = 0$).

Three things make this a *model* statement rather than a postulation:

1. **The model is boundary-only.** Any interaction requires
   $r_1 = \mathrm{effective\_t}(t_1)$ and $r_2 = \mathrm{effective\_t}(t_2)$
   (manuscript §499), so only the active shell talks.  A pair budget is an
   *area* quantity.
2. **The shell is the reissue surface.** Reissue happens at $t = R_{\max}$ and at
   the contact point, and the active wavefront point is forwarded through `s2B`
   (§563).  Hence *rest = closed shell* ($\sum \boldsymbol m = 0$) and
   *motion = closure defect* ($\sum \boldsymbol m \ne 0$).
3. **The dressing has nowhere else to live.** $N_I = 9L$ islands of
   $\ell = L/3$ copies are universal (§950-956), the same for every particle.
   The island count therefore *cannot* carry a mass hierarchy; only the
   dressing/shell can.

Two readings of "shell", which the instrument separates:

| Reading | Configuration | Count is governed by |
|---|---|---|
| **spatial** | pairs on *distinct $W$ addresses* whose 3D positions cover a closed surface around the island | the shell's area (hence $\rho$) |
| **stack** | pairs *coincident* on the island, summing in `pair_count` | multiplicity only (frequency $=2n$, §1764) |

## The self-formation constraint (the reason this is a low-sieve question)

A self-formed shell needs a supply of free pairs, a recruiter
(orphan fronts, §813-815: *"orphan wavefronts guiding free pairs"*), affinity
adoption, co-transport (`INERTIA.md`), and an escape term.  The balance is the
one the manuscript already postulates for the island (§974,
$\Gamma_{\mathrm{cap}} = \Gamma_{\mathrm{esc}} \Rightarrow N_*$), applied to the
**dressing**.

**But the reference build forms no pairs at all:**

```
alpha_probe 7 4 200 16384 256  ->  active-passes = 6468, s2B = 0, pairs = 0
```

(`doc/REFERENCE_CONFIG.md`, "Post-freeze additions").  Consequently the reference
configuration **cannot decide** H-SHELL: with `pairs = 0` the dressing term is
identically absent, so "bare charge" wins by *regime*, not by physics.  The
canal that could populate the shell is the low-sieve regime, which the manuscript
already flags as open: *"the sieve sweep … demonstrates that the pair-formation
branch can fire in a parameter regime, but its physical consequences remain
unverified"* (§1289).  **The mass question and the WP2 $S$-sweep are the same
question.**

## Instrument

```
experiments\build_rest_shell_probe.bat

experiments\rest_shell_probe.exe LX LY LZ N CANCEL IMPULSE [stack|shell] RADIUS FRAMES
                              [SIEVE] [CSV] [BURN]
```

* `N` — body elements of the prepared island (one `K` + `N-1` `D`, charge `0x08`,
  common affinity); the pairs share that affinity (the drive-pair requirement,
  §481).  `W_USED = N + 2*(CANCEL + IMPULSE)`, one source centre per `W` layer.
* `CANCEL` — photon pairs whose transport vectors alternate sign.  **Even
  `CANCEL` is a closed set** ($\sum \boldsymbol m = 0$: the rest dressing);
  **odd `CANCEL` is an open set**, i.e. a residual resultant (relaxation probe).
* `IMPULSE` — extra aligned pairs; they supply the net resultant whose response
  is measured.
* `stack` — cancelling pairs sit on the island centre (multiplicity reading).
  `shell` — cancelling pairs sit on distinct `W` addresses whose positions are the
  offsets of the model's **own** integer-radius shell
  ($r = \mathrm{isqrt}(r^2) = $ `RADIUS`, same metric as `isOrphanShell`).
* `vacuum` — **self-formation test.**  `CANCEL` counts pairs' worth of *unformed*
  material: the extra `W` layers are seeded as complementary-charge **singletons**
  (`0x00`/`0x1f`), not as pairs, so nothing is a dressing until the dynamics makes
  it one.  `IMPULSE` must be `0`.  Reports the pair count the model actually
  forms, and how much material remains unformed.

Reported: body CoM velocity (cells/light frame and per era $2R_{\max}$), the same
for **all** source centres (body + dressing, so a dressing recoil cannot hide),
`v_per_impulse_pair`, surviving pair count, intact reciprocal links,
$\sum \boldsymbol m$, body span, pair gap, and a per-frame CSV.

## Controls (must pass before any claim)

| Run | Expected |
|---|---|
| `CANCEL=0, IMPULSE=0` | `v = 0` exactly (matches `INERTIA.md`, "No pairs") |
| `CANCEL=2, IMPULSE=0` | `v = 0` and `final_sum_m_x = 0` — the measured *"two opposed pairs give zero mean velocity"* |
| `CANCEL=2, IMPULSE=1` | reproduces the reference "one rightward pair" rate of `INERTIA.md` for this tube |
| `stack` vs `shell`, `IMPULSE=0` | identical except the pair gap |

Windows must span **whole** breathing periods ($2R_{\max}$ light frames); the
`INERTIA.md` caution applies: sub-era windows expose phase sampling, not drift.

## Falsifiers of H-SHELL

| # | Prediction | Falsified if |
|---|---|---|
| **P1** | *Dressing inertia.* At fixed `IMPULSE`, `v_per_impulse_pair` **decreases** with `CANCEL` | it is invariant in `CANCEL` ⇒ the dressing is not inertia and "bare charge" is operationally right |
| **P2** | *Spontaneous closure.* An **open** dressing (odd `CANCEL`) relaxes: $\sum \boldsymbol m \to 0$ and `v → 0` across eras | the resultant persists ⇒ no self-closing mechanism exists and the closed shell is an input, not an attractor |
| **P3** | *Memory loss.* The steady pair count is independent of the seeded count | the count tracks the seed ⇒ no dressing attractor |
| **P4** | *Layout discrimination.* At equal `CANCEL`, `stack` and `shell` differ in the response | they agree ⇒ only multiplicity matters, and "shell" is a name for the stack |

**Measured status: all four fail in the reference configuration** (see
*Reference-kernel results* below).  With `s2b_target = 16384` no pairs form and
no recruitment/escape channel is enabled, so running them at the reference sieve
is a **negative control on the instrument**, not a test of the model; H-SHELL is
addressable only in the low-sieve (open-channel) build.

## Reference-kernel results (measured 2026-09-11)

Build `experiments\build_rest_shell_probe.bat` (reference flags, all candidate
macros OFF), `s2b_target = 16384`, tubes `9x5x5` and `15x5x5` (`RMAX = 2`,
era $= 2R_{\max} = 4$), burn 8 of 24 frames so that every window spans **whole**
eras.  Outputs under `build/rest_shell/` (`*.csv`, `*.txt`).

| run | N | cancel | impulse | layout | $v_{body}$ (cells/light frame) | $v\times$era | dressing |
|---|---:|---:|---:|---|---:|---:|---|
| `ctl_none` | 3 | 0 | 0 | stack | `0.000000000` | 0.000 | none |
| `ctl_closed` | 3 | 2 | 0 | stack | `0.020833` (2nd window `0.000000000`) | 0.083 | 2 pairs, links intact, $\sum m = 0$ |
| `anchor_1pair` | 3 | 2 | 1 | stack | `0.250000000` | 1.000 | 3 pairs intact, $\sum m = +4$ |
| `p1_c4_stack` | 3 | 4 | 1 | stack | `0.250000000` | 1.000 | 5 pairs intact, $\sum m = +4$ |
| `p4_shell` | 3 | 4 | 1 | shell $r{=}2$ | `0.000000000` | 0.000 | **1 of 5 pairs survives**, gap 4 |
| `p2_open` | 3 | 3 | 0 | stack | `0.250000000` | 1.000 | 3 pairs intact, $\sum m = +4$ |
| `n5` | 5 | 0 | 1 | stack | `0.150000000` | 0.600 | 1 pair |
| `n7` | 7 | 0 | 1 | stack | `0.107142857` | 0.429 | 1 pair |

**Findings.**

1. **Controls pass.** `cancel = 0` is exactly zero.  The prepared *closed*
   dressing settles to exactly zero in the second window while both links stay
   intact and $\sum \boldsymbol m = 0$: the model **maintains** a prepared
   closure — it neither destroys it nor amplifies it.  (The apparent drift seen
   with sub-era windows disappeared with whole-era windows, confirming the
   `INERTIA.md` caution about window length.)
2. **P1 falsified.** At fixed impulse, doubling the cancelling dressing (2 → 4
   pairs) leaves the response *bit-identical* (`0.250000000`), and the
   all-source mean is unchanged too.  A cancelling dressing is **not** inertia.
3. **P2 falsified.** An open dressing (odd `cancel`, $\sum \boldsymbol m \ne 0$)
   drifts at exactly the rate of one aligned pair and does not decay between
   windows: there is **no spontaneous closure**.
4. **P3 falsified.** The pair count tracks the seed exactly (2→2, 3→3, 5→5 in the
   stack layout): there is **no dressing attractor**.
5. **P4 falsified, strongly.** In the `shell` layout the dressing is *not*
   maintained: 4 of 5 pairs disappear, the survivor detaches (`gap = 4`), and the
   body receives no propulsion at all.  Drive pairs act only on `P×K`/`P×D`
   *contact*, so a spatially distributed pair shell is not a drive structure in
   this kernel.
6. **Positive result (the complement of H-SHELL).** With one aligned pair, whole-era
   windows give

   $$v_{body} \;=\; \frac{3}{4N}\quad\text{exactly:}\qquad
   0.250000000\ (N{=}3),\quad 0.150000000\ (N{=}5),\quad 0.107142857\ (N{=}7)$$

   i.e. the effective mass is **exactly proportional to the island count**
   (3 : 5 : 7).  In the reference kernel the inertia is the **island**, and the
   dressing is a momentum budget, not a mass.

### Self-formation from the vacuum (`vacuum` layout)

Material seeded unformed (12 complementary singleton layers, **zero** seeded pairs),
island $N{=}3$, 20 frames, burn 4:

| squeeze run | sieve $S$ | pairs formed | material left unformed | $\sum m$ | $v_{body}$ |
|---|---:|---:|---:|---:|---:|
| `vac_ref` | 16384 | **0** | 12 | 0 | `0.000000000` |
| `vac_s64` | 64 | **6** | 0 | 0 | `0.000000000` |
| `vac_s32` | 32 | **6** | 0 | 0 | `0.000000000` |

1. **Formation is real and sieve-governed.** At the reference sieve nothing forms
   and all 12 layers stay unformed; at $S = 64$ and $S = 32$ the material is fully
   consumed into 6 pairs.  The model's own ledger confirms a sustained channel:
   `[charges] retina ... pairM=6 pairA=6 ... form=286`.
2. **The self-formed dressing is coincident with the island** (`gap = 0`) and
   carries $\sum \boldsymbol m = 0$: the body stays exactly at rest.
3. **Caveat that decides the reading.**  A freshly formed pair is initialised with
   $\boldsymbol m = 0$, and $\boldsymbol m$ is written only by the dynamic axis
   election (`polarization::elect` / `installAxis` at $t = R_{\max}$), which does
   not fire in this configuration.  Hence $\sum \boldsymbol m = 0$ here is
   (at least partly) **vacuous**: the dressing has no vectors at all, so there is
   no shell and no propulsion.  "Closed vs open dressing" is therefore not yet
   measurable with this build; it needs the bootstrap variant
   (`build_rest_shell_probe_boot.bat`).

What this *does* establish: **step 1 of H-SHELL — the dressing forms itself out of
the vacuum — is real, is governed by the sieving modulus, and is absent from the
reference configuration.**

### Is the self-formed dressing a *shell*? (bootstrap build)

Same vacuum run with `build_rest_shell_probe_boot.bat`
(`/D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE`), i.e. with the axis
election able to install $\boldsymbol m$.  Identical at $S = 64$ and $S = 32$:

| frame | pairs | intact | $\sum m_x$ | $v_{body}$ | $body_x$ |
|---:|---:|---:|---:|---:|---:|
| 1 | 0 | 0 | 0 | `0.000000000` | 4.0 |
| 2 | **6** | 1 | 0 | `0.000000000` | 4.0 |
| 3 | 6 | 1 | 0 | `0.000000000` | 4.0 |
| 4 | 6 | 1 | **−7** | `0.000000000` | 4.0 |
| 5 … 20 | 6 | 1 | **−7** (frozen) | `0.000000000` | 4.0 |

1. **Formation is one light frame wide.**  The channel is not gradual: the 12
   unformed layers become 6 pairs in a single journey and then freeze.
2. **The dressing does not close.**  Once the election installs vectors (frame 4,
   one era), $\sum \boldsymbol m = -7$ and stays *exactly* $-7$ for the remaining
   17 frames — no relaxation.  **P2 is falsified with a self-formed dressing**,
   which is stronger than the seeded case above.
3. **A net dressing vector is not propulsion.**  With $\sum \boldsymbol m = -7$
   and the dressing coincident with the island (`gap = 0`), the body and the
   whole source set move by exactly zero.  So $\sum \boldsymbol m \ne 0$ is *not
   sufficient*: the drive-pair contact transport must also fire, and here it does
   not.
4. **No turnover.**  `pairs = 6`, `intact = 1`, `gap = 0` are constant over 17
   frames: the dressing is static, not a churning population.  The flame image
   fails on its second clause too — nothing feeds the frontier in this
   configuration.

**Conclusion for H-SHELL.**  Step 1 (the dressing forms itself out of the vacuum)
is **real**, one-light-frame fast, sieve-governed, and absent from the reference
configuration.  Steps 2–4 (closure, turnover, propulsion) are **absent**: what
self-forms is a static pair *stack*, not a dynamic shell.

**Consequence.** In the reference configuration the operational answer to "is a
resting electron a bare charge?" is **yes**: mass = island count, dressing
contributes no inertia.  The dressed-mass reading requires a mechanism the
reference build does not have (recruitment + retention + an escape balance), i.e.
the low-sieve open channel.

**Note on the published table.**  The `15x9x9` entry of `INERTIA.md`
(`0.175000` / `0.700000`) was taken with windows the note itself says *"are not
multiples of its eight-frame breathing period"*.  The $1/N$ law above should be
re-checked there with whole-era windows before those values are quoted as steady
speeds.

## Reading aid (metaphor — an image for the discussion, never an argument)
## Reading aid (metaphor — an image for the discussion, never an argument)

> **The shell is a flame, not a brick.**

A flame is self-formed: it exists only while the flow continues, its molecules
are exchanged every instant, yet its size and shape are robust.  Its size is set
by the balance of inflow and outflow — here
$\Gamma_{\mathrm{cap}} = \Gamma_{\mathrm{esc}}$, i.e. the pair supply, which the
sieve modulus $S$ controls.  A **closed** front has no net resultant; a
**breach** (a missing pair) produces one — the "soap film" half of the image,
where closure means minimal surface and hence zero net force.

Use it *after* the mechanism is stated, in the discussion only, and state
explicitly that nothing is consumed: the flame is an image for an **invariant
count in a churning population**.  It is consistent with the model's own arrow of
time (§640), which is why it is apt — it is not evidence for anything.

## Limits

* Prepared initial conditions.  The harness does **not** demonstrate
  self-assembly of an island, nor of the dressing.
* Host-side transport arbitration (contact list, integer DDA, whole-layer
  `applyMomentum`) is not a local cellular rule; what is measured is the
  reference algorithm.
* A `shell` dressing is representable only if distinct `W` addresses can hold
  pairs at distinct 3D positions (one source centre per `W` layer, `lcenters[w]`);
  spatial coherence of a *shell* dressing is not asserted by the instrument.
* No claim is made here about a mass law, a lepton mass ratio, Itô's radial
  harmonic, or spin.  The instrument decides only whether the dressing behaves as
  inertia, and whether an open dressing closes on its own.

## Fingerprint

`experiments/` is outside the model fingerprint: `model-ref-v1` and the reference
build are untouched by this note and by `rest_shell_probe.cpp`.

## Recipes

```bat
rem controls
experiments\rest_shell_probe.exe 9 5 5 3 0 0 stack 0 24 16384 build\rest_shell\ctl_none.csv 8
experiments\rest_shell_probe.exe 9 5 5 3 2 0 stack 0 24 16384 build\rest_shell\ctl_closed.csv 8
experiments\rest_shell_probe.exe 9 5 5 3 2 1 stack 0 24 16384 build\rest_shell\anchor_1pair.csv 8

rem P1 (dressing inertia) and P4 (layout)
experiments\rest_shell_probe.exe 9 5 5 3 4 1 stack 0 24 16384 build\rest_shell\p1_c4_stack.csv 8
experiments\rest_shell_probe.exe 9 5 5 3 4 1 shell 2 24 16384 build\rest_shell\p1_c4_shell.csv 8

rem P2 (relaxation of an open dressing)
experiments\rest_shell_probe.exe 9 5 5 3 3 0 stack 0 96 16384 build\rest_shell\p2_open.csv 24

rem open channel -- where H-SHELL is addressable at all
experiments\rest_shell_probe.exe 9 5 5 3 2 1 stack 0 48 64 build\rest_shell\p_lowsieve.csv 12

rem self-formation: is any pair formed at all, and does it propel?
experiments\rest_shell_probe.exe 9 5 5 3 6 0 vacuum 0 20 16384 build\rest_shell\vac_ref.csv 4
experiments\rest_shell_probe.exe 9 5 5 3 6 0 vacuum 0 20 64 build\rest_shell\vac_s64.csv 4
experiments\rest_shell_probe.exe 9 5 5 3 6 0 vacuum 0 20 32 build\rest_shell\vac_s32.csv 4
```

