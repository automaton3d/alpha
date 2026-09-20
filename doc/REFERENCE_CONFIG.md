# Reference configuration and MODEL_VERSION

This file freezes the **reference build** of the `alpha` automaton: the exact
source state, toolchain and macro set that every quantitative number in the
manuscript must be traceable to.  It is the anchor for WP1 of
`experiments/WORK_PLAN.md`.

## MODEL_VERSION

| Field | Value |
|---|---|
| Git commit | `4501f25e68371c522b6c0b68ada9a73906fae77f` |
| Branch | `polarization-broadcast-fix` |
| Annotated tag | `model-ref-v1` |
| Model source fingerprint (SHA256) | `5b0944258b8f9b750688375f2e9a4cd99e74f0355f1c6b1aa8edc769ab743673` |
| Frozen on | 2026-09-11 |

The fingerprint covers every model source: `src/model/*.cpp`,
`src/model/*.inc`, `src/include/model/*.h` (see the per-file table below).  It is
the authoritative drift check: if it changes, the dynamics may have changed and
any table must be regenerated.  Verify with:

```powershell
powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1
```

The tag `model-ref-v1` marks this model state.  Later commits that only touch
documentation, scripts or the GUI do **not** change the fingerprint and do not
invalidate results.

## What "reference" means

- **CPU build**, MSVC, C++20, optimisation `/O2`.
- **All candidate macros OFF** (the ordinary `simulation()` / `encounter()`
  path).  The reference build is what `run_all.bat` and `build_gui.bat`
  produce, and what `experiments/build_probe.bat` links.
- **GPU path disabled.**  See gate G0 below.

## Compiler and flags

| Item | Value |
|---|---|
| Compiler | MSVC 14.50 (Visual Studio 18 Community, `HostX64\x64`) |
| Language | `/std:c++20` |
| Optimisation | `/O2 /EHsc /MD /D NOMINMAX` |
| Includes | `/I src\include /I src\include\zlib /I src` (+ vcpkg `include`, `include/freetype2`) |
| vcpkg | `E:\vcpkg\installed\x64-windows` (freetype, glfw3, zlib, bz2, brotli, libpng) |
| Link | `opengl32 glfw3dll freetype brotlidec brotlicommon bz2 zlib user32 gdi32 shell32 kernel32 ole32 comdlg32` |

The headless harnesses compile the model sources directly with the same flags
(see e.g. `experiments/build_probe.bat`); the GUI uses `nmake` (`Makefile`).

## Candidate / experimental macros -- MUST BE OFF in the reference build

These macros gate separate mechanisms that are **not** part of the model.  Any
executable that defines one of them is a *separate binary* and is **not** an
interchangeable measurement of the reference dynamics.

| Macro | Gates | Where |
|---|---|---|
| `ORPHAN_GUIDANCE_FSM` | experimental P1/P2 photon-mediated impulse (orphan shell, recruit/relay) | `interaction.cpp` (154, 161, 547, 567, 991, 1190), `simulation.h` (425), `attic/alpha_campaign/alpha_probe.cpp (retired 19 Sep 2026)` (778) |
| `ORPHAN_GATE_ONLY`, `ORPHAN_NO_SHELL`, `ORPHAN_NO_RELAY`, `ORPHAN_NO_ANNIH`, `ORPHAN_KICK_CAP`, `ORPHAN_KICK_PER_WINDOW`, `ORPHAN_MEDIATOR_PROPAGATES`, `ORPHAN_MEDIATOR_SUSTAIN`, `ORPHAN_RELAY_KEEPT`, `ORPHAN_RELAY_NOOP`, `ORPHAN_RELAY_SHUTTLE` | sub-options of the orphan-mediation schemes A-E | `interaction.cpp` (e.g. `ORPHAN_RELAY_NOOP` at 1077, `defined()` form), `simulation.h`, `attic/alpha_campaign/alpha_probe.cpp (retired 19 Sep 2026)` |
| `EXCLUSION_FSM` | Pauli-like identity hard core across seed families + push | `interaction.cpp` (776, 892, 1575), `color_fsm.inc` (28, 88, 132, 177, 226, 233, 304), `attic/closed_programmes/turnover_ablation.cpp` (202) |
| `EM_FIRST_FSM` | decide the electroweak channel before identity | `interaction.cpp` (1451-1473) |
| `EM_FORCE_PREREQ` | (WP4.2 spike) force the `EM_FIRST_FSM` prerequisite s2B+pB at each contact | `interaction.cpp` (1251) |
| `EM_NOS2B_FSM` | (Route B) drop the probabilistic sieve requirement from the EM reorder | `interaction.cpp` (1464) |
| `DD_INTRA_ISLAND_FIX` | (WP3.3 candidate) intra-family election uses the family-minimum chief, so a 3-copy family stays 1 K + 2 D | `chief_transition.h` |
| `COLOR_ENCOUNTER_FSM` | colour FSM replaces `update_lattice_cpu` | `simulation.cpp` (647) |
| `COLOR_CHIEF_FSM`, `COLOR_COHESION_FSM`, `COLOR_MATCHED_FSM` | colour/election/cohesion/matched candidates | `color_fsm.inc`, `color_matching.inc` |
| `DD_PROMOTION_FSM`, `KK_CLASH_FSM` | role transitions (D x D promote, K x K clash) | `interaction.cpp` |
| `INERTIA_MATCHED_FSM` | matched-experiment inertia harness | `experiments/inertia_matched.cpp` |
| `LIGHT_MATTER_FSM` | provisional pB/sB decoupling stand-in | `color_fsm.inc`, `attic/closed_programmes/lightmatter_decouple.cpp` |
| `ISLAND_SEED_EXPERIMENT` | reduced seed branch (excluded by the census) | `initSim.cpp` |
| `COAGULATION_RANDOM_FSM` | coagulation random experiment | `attic/closed_programmes/coagulation_random.cpp` |
| `POLAR_BOOTSTRAP_ADDRESS`, `POLAR_BROADCAST_WAVE` | experimental polarization bootstrap/broadcast | `polarization.cpp` |
| `ORPHAN_PRINT_ONLY` | probe-only source diagnostic (companion of `ORPHAN_DUMP_SRC`) | `attic/alpha_campaign/alpha_probe.cpp (retired 19 Sep 2026)` (778, `defined()` form) |
| `USE_CUDA`, `CUDA_BRIDGE_CU` | GPU path (see G0) | `bridge.cu`, `cuda_automaton.cu`, `Makefile` |

Include guards (`ATTRACTOR_H_`, `GEOMETRY_H` -- note: no trailing underscore,
`POLARIZATION_H_`, `SIMULATION_H_`, `WAVEFRONT_H_`) are not feature macros and are ignored.

**The two tables are one inventory, split chronologically.**  Everything in the first table already
existed at the `model-ref-v1` freeze (`interaction.cpp` carried only `EM_FIRST_FSM`,
`EXCLUSION_FSM`, `ISLAND_SEED_EXPERIMENT` and the `ORPHAN_*` set there); everything added afterwards
is in "Post-freeze additions" below.  Together they are the **complete** set -- 45 feature macros in
the model sources, no more.  All line numbers in both tables were re-verified on 19 Sep 2026
against the compiled tree (`src/model/`, `src/include/model/`); they shift whenever a macro-guarded
block is added, so re-check with:

```powershell
Get-ChildItem src\model\*.cpp, src\model\*.inc, src\include\model\*.h |
  Select-String -Pattern '(?:ifdef|ifndef)\s+([A-Z_][A-Z0-9_]*)|(?:if|elif)\s+defined\s*\(\s*([A-Z_][A-Z0-9_]*)' -AllMatches |
  ForEach-Object { $_.Matches | ForEach-Object { if ($_.Groups[1].Success) { $_.Groups[1].Value } else { $_.Groups[2].Value } } } |
  Sort-Object -Unique
```

That prints those 45 macros plus the five include guards, one-to-one with the two tables.  The only
macro-gated names *outside* this inventory are harness-only selectors in `experiments\*.cpp`
(`ORPHAN_PRINT_ONLY`, `ORPHAN_DUMP_SRC`, `INERTIA_REVALIDATION`,
`PROPELLER_K_FLIGHT_EXPERIMENT`), which cannot change model behaviour.

## Per-file model fingerprint

| File | SHA256 |
|---|---|
| attractor.cpp | `F63FC9AD80DA5463F20508C4E3FF4C4727F5F5C124C7657CDCC9D7715F0BA4FB` |
| attractor.h | `14A0398BA9D02A53519C1C448B7548A32A0EF952BAC2A6CFD4A49B8D0B0CCBFA` |
| bridge.cpp | `CF3A9E9F179E0397B4FDB61C3ED4111359A6A75AE63128C5FCA4BEFE13183312` |
| charges.cpp | `4F33766AD87C843ED1E102E0E4154ADE89D1A200C8A1985F9FF9309A739AB03D` |
| chief_transition.h | `F483F4645D84C50E6604C7BBB716452D49F9930F3E4E27A60A68C5237E9EAB4C` |
| color_contact.h | `777A4DBD19DDF166193E27F39FC56EE5D88C0F308EB3235221903DB79746478F` |
| color_fsm.inc | `1447EEE895F54BA85F5C4D58498C81BD965F948147843D109E238072260DFBE0` |
| color_matching.inc | `EB91C1D66919648E9610B5F04FAEFB117BBB6D42AA3F7C001CCE963C3F7B3F2D` |
| election_payload.h | `D945598DC552DDA73C2241C69F3E896A0F3AC3C9E296CFDF1DE5DA06BA95649B` |
| geometry.cpp | `A01551A8FD136B4CFE0EE62A1F1F8A9D96AEBB831CAF62DF524E23A49F9CA99C` |
| geometry.h | `460780AFAD49C1D431683123138F28A40CEFF2F667A0B1E71BC02B50978F601F` |
| inertia_contact.inc | `82E9DC8A2E934BE3ED5AABA63D886FAD7F12FA5BE10B5991980F4A16CA9237FE` |
| initSim.cpp | `83043A463E435834AA603B69910AB9E6CDDE13BF29182CD91FFB58E04AAF7D86` |
| interaction.cpp | `29225F56BCEB0C7BE37B0523CF67D9A34E62364FA19B2CD7F7A5B4B9C9CD83DC` |
| island_identity.h | `068EA453D689CFFA58CA9DE9C0AE3C481052CDB38C2602AFB2FA7070BBEE127F` |
| polarization.cpp | `38B009D6D5034140B8D5499314C328BDD5779B5B91DEBEB4BCE0EC1BD1F3CDA4` |
| polarization.h | `77222B76CB3126715E1F879C8511DF51AA4AD9BE1FFE80EF15C5664CF2D4A509` |
| polarization_candidate.h | `FF52A1E0698485B779243494B40B44E2B4EEB4D610DD1380672B280784C4EC31` |
| simulation.cpp | `E46D0E47E57FFACBB84AFB352E30B7B59BEF92B91C45BCCE97389FE28953732B` |
| simulation.h | `011CAE080CE74D0BB0CDE058B3A36A44C97566998981EDE8DBD99060F909751D` |
| utils.cpp | `57EE9272A0E44FD6B0CF0A58558887250555FC02E29157EBC92FDEC0C80F08FC` |
| wavefront.cpp | `E2CDA484BFD645295D4DDEB2CF4937EB33DBB4075855E779B304474AB90C0162` |
| wavefront.h | `BD3F13BA298EFC9ADF81599133FEC27CFC5BE9B569B3E89F571DE117A699BDD0` |

Combined fingerprint (model-ref-v1): `5b0944258b8f9b750688375f2e9a4cd99e74f0355f1c6b1aa8edc769ab743673`.

## Post-freeze additions (working tree)

Since the `model-ref-v1` freeze the working tree has added the macro-guarded blocks below.  All are
OFF in the reference build, so "unchanged when OFF" holds by construction (the code is compiled
out); it is marked "(verified)" only where an A/B run is recorded in `experiments\*.md`.  The
"Where" numbers are the `#ifdef` sites, re-verified 19 Sep 2026.

| Macro | Effect | Where | Reference behaviour |
|---|---|---|---|
| `EM_FORCE_PREREQ` | forces the `EM_FIRST_FSM` prerequisite (s2B + pB) at each contact (WP4.2 spike) | `interaction.cpp` (1251) | unchanged when OFF |
| `DD_INTRA_ISLAND_FIX` | intra-family election uses the family-minimum chief (WP3.3); a 3-copy family stays 1 K + 2 D | `chief_transition.h` (17, 46) | unchanged when OFF (verified: `attic/closed_programmes/ABLATION_81x3.md`) |
| `EM_NOS2B_FSM` | drops the probabilistic `s2B` gate from the EM reorder (Route B); needs `EM_FIRST_FSM` | `interaction.cpp` (1464) | unchanged when OFF |
| `SURFACE_ESCAPE_FSM` | item-4 candidate: a delegate whose group has had no internal contact for one full light frame is released back to `S` (the "surface escape" rule; the only release agent in the frozen-group probes) | `interaction.cpp` (84, 140, 149, 571, 692, 895, 964), `simulation.h` (304) | inert when OFF; ON is a separate binary (`p7_concentration`, `spin_probe`, `spin_ring_probe`) |
| `SPIN_GATED_FSM` | J-programme S1/S3: computes the group's circulation `J = sum r x m` on the chief's frame and vetoes a release that would carry its last unit away; always built with `SURFACE_ESCAPE_FSM`, whose release test it gates | `interaction.cpp` (90, 713, 740), `simulation.h` (307) | **verified** (`J1_SPIN.md`): the reference releases every `N = 1..6`, the gated coherent arm holds every `N` (vetoes `N*(8-N)`) |
| `WINDING_GATED_FSM` | T1 spatial-winding candidate, **PARKED**: the leave-veto keys on the file-scope global `chief_W[3]`.  No winding number is computed anywhere in the model, and because the label is global, `W != 0` vetoes every release, not only the group that "carries" it | `interaction.cpp` (106, 754), `simulation.h` (312) | inert when OFF; **no harness in the tree** -- parked with its probe in `attic/` (`attic/WINDING_PROBE_PARKED.md`); explicitly UNTESTED, not a measurement |
| `PHASE_DISTINCT_FSM` | identity is (charge word, breathing phase): refuses merges between sources that agree in word AND phase, covering T1/T3/T4 and, through `promotesDelegate`, T2 | `chief_transition.h` (4, 39) | inert when OFF; see `attic/closed_programmes/EMERGENCE_SEARCH.md` |
| `FAMILY_SELECTIVE_FSM` | producer side of WP8 option 2: only intra-family pairs (`w/3`) write the directional field, in the relative-displacement encoding, excluding the cross-family producers | `interaction.cpp` (1309) | inert when OFF; see `attic/closed_programmes/FAMILY_SELECTIVE_DESIGN.md` |
| `ADDRESS_TARGET_FSM` | the address supplies the place: each layer's centre walks to the site its island index labels (`i % EL, (i / EL) % EL, CENTER`), one cell per light frame, so the `ISLAND_SIZE` copies walk in lockstep; the "quantised islands" absorbing state is this rule's fixed point | `simulation.cpp` (621) | inert when OFF; separate binary `island_census_addr` |
| `HOMB_PRODUCER_FSM` | ports the directional producers that exist only in the archived CUDA kernel (`src/cuda/cuda_automaton.cu`, `dev_encounter4/6/7`) into the CPU `encounter()`, giving the homing flag `homB` -- and through it the relocation vector `c` and the `RELOC` stage -- its missing producer (WP8).  Deterministic: the direction comes from the in-phase/quadrature bits `pB`/`sB` and, for equal phases, from the immutable `W` address.  Requires live `pB`/`sB`, i.e. a build that also breaks the election fixed point (`POLAR_BOOTSTRAP_ADDRESS`), and a tube with `RMAX >= 4` | `interaction.cpp` (1261, 1924, 1935, 2053), `simulation.cpp` (432, 447, 753) | unchanged when OFF (verified in WP8) |
| `ISLAND_ALIGNED_W_ROTATION` | candidate: the cross-layer rotation is cyclic WITHIN each `ISLAND_SIZE` block, so a slot's partner always belongs to the same island (the reference rotates the whole partner lattice by one slice per frame, so at `ISLAND_SIZE = 3` two of every three pairings are cross-island) | `utils.cpp` (47) | inert when OFF; separate binaries `island_census_align` / `island_census_rot` |
| `PLACED_FAMILY_SEED` | prepared initial condition for visualisation: every family is placed at its own site on a flat `EL x EL` grid instead of all centres superposed | `initSim.cpp` (369) | not the reference seed; separate binary `island_census_placed` |
| `MULTIFREQ_RAY_FSM` | multi-frequency mechanics: per-tick ray detection sets the binary `freq_hit` per source and gates the `pB`/`sB` channels on it | `interaction.cpp` (121, 342, 559, 951, 1474, 1747), `simulation.h` (319) | inert when OFF |
| `PAIR_STACK_ABSORB_FSM` | multi-frequency piece: counts the free S+S formations that absorbed identical co-located stacks | `interaction.cpp` (114, 277, 1683), `simulation.h` (316) | inert when OFF |
| `HOMB_CONSUMER_TRANSPORT` | WP8 consumer: decodes the arrived `c[]` field into `reloc[]` -- the transport chain the ported producers feed | `simulation.cpp` (532) | inert when OFF |
| `POLAR_MAGNITUDE_FSM` | WP8 (iii): liveness by MAGNITUDE instead of by sign, i.e. `pB`/`sB` become `pol != 0` rather than `pol > 0` | `simulation.cpp` (366) | inert when OFF |
| `PAIR_WORD_LOG` | instrumentation only: prints the charge words of every pair formation (`[pairword] ...`) so a long run can be checked against the reachability table of `CHARGE_SPECTRUM.md` | `interaction.cpp` (1656) | stdout only; no dynamics change |
| `CASCADE_LOG` | instrumentation only (A2 probe): records every encounter that changes a source's role -- tick, both addresses, both kinds, charge word, parent before/after -- and prints the global tally plus one line per charge word with the tick window of that word's cascade and its LAST transition.  No rule, no lattice write; the ledger lives in `chief_transition.h` and is dumped by `experiments/cascade_probe.cpp` | `chief_transition.h` (88-163), `interaction.cpp` (1791) | stdout only; **with the macro OFF the preprocessor removes the block, so the reference build is byte-identical** (verified) |
| `PARENT_SELECTIVE_FSM` | candidate (2026-09-19) for the author's rule set: `D x D` stops promoting (`chief_transition.h`), so the cap of 2 is removed; same-island `D x D` becomes a no-op and cross-island `D x D` repels by one step at the frame edge, keyed on the EMERGENT parent (`islandChief`), never on `w/3` / `ISLAND_SIZE` / `L`.  Alone it collapses the membership structure (1 K, 241 unresolved) because T2 was the chief-birth cascade; needs `PARENT_NOMINATION_REPAIR` | `chief_transition.h` (17), `interaction.cpp` (140, 555, 589, 858, 1078, 1802) | inert when OFF; separate binaries `island_census_parent*`, `rb_cand` |
| `PARENT_FUSION_ABSORB` | completion of that rule set: on a `K x K` fusion the demoted chief's delegation is re-pointed to the survivor, so the fusion GROWS an island instead of orphaning a delegation | `interaction.cpp` (148, 557, 591, 891, 1080, 1791) | inert when OFF; measured inert in the fsm-only build (`K x K encounters = 0`), active in the censuses |
| `PARENT_NOMINATION_REPAIR` | completion of that rule set: at the frame edge any source NAMED as a parent by a delegate but not a chief is promoted -- the deterministic replacement for the global repair the T2 cascade used to perform | `interaction.cpp` (926, 1083) | inert when OFF; with it the build reaches 162 K / 0 unresolved |
| `PARENT_SHELL_RELEASE_FSM` | brake (iii), parameter-free: a delegate farther from its chief than the model contact range `2*RMAX` is outside the shell cohesion can claim and is released (a size-dependent loss on a TRANSITION; the displacement route is closed by `length<=1` in `inertia_fixture.h`) | `interaction.cpp` (810, 1092) | inert when OFF; verified to read EXTENT and not member count (`range` mode of `island_rate_probe`, `rb_shellonly`) |
| `INERTIA_REVALIDATION`, `PROPELLER_K_FLIGHT_EXPERIMENT`, `ORPHAN_DUMP_SRC` (harness only) | selectors/diagnostics of the harnesses: which output root the two arms write to, the propeller flight experiment, and the probe's source dump | `experiments/inertia_matched.cpp` (37), `inertia_revalidation.cpp`, `attic/closed_programmes/propeller_k.cpp`, `attic/alpha_campaign/alpha_probe.cpp (retired 19 Sep 2026)` | harness only -- cannot affect the reference binary |


### Prerequisite discovered by the WP8 audit: `RMAX >= 4` for the polarization sector

`simulation.cpp` reconstructs the transverse pair as
`reconstructPair(c.bstamp, RMAX - 2, pol_u, pol_v)`, and that function returns
`(0,0)` for `R <= 0`.  With `R = RMAX - 2`:

| short side | `RMAX` | `R` | consequence |
|---|---|---|---|
| 5 | 2 | 0 | `pol == (0,0)` identically: the whole polarization sector is structurally zero |
| 7 | 3 | 1 | `pv = 2R*isqrt(j(R-j)) == 0` for every admissible `j`: `sB` is dead, only `pB` can live |
| >= 9 | >= 4 | >= 2 | `pB` and `sB` can both be live |

Any measurement of `pB`, `sB`, polarization, homing or electroweak contact in a
tube with a short side of 5 or 7 is therefore degenerate, with or without
`POLAR_BOOTSTRAP_ADDRESS`; such runs must be repeated with a short side of at
least 9 before being quoted.

Current working-tree model fingerprint:
`bacade53e4d0c0a7ae1601992579d0066a603e4ccf9020051399f8cc510a5d82`
(re-measured 19 Sep 2026 with `experiments\model_fingerprint.ps1`, whose default `-Expected` carries this
value; re-checked by recomputing the per-file table with the script's own method).  It moved past the
previously recorded `7f5eb50e...` for reasons that do **not** touch the reference dynamics: the `D x D`
correction pass rewrote four comments in `interaction.cpp` and `chief_transition.h` (the false "D-D
cohesion" claim), and this fingerprint hashes source **text**, so a comment-only edit moves it.  The latest
move is the read-only **A2 instrumentation** (`CASCADE_LOG`): a ledger block appended at the END of
`chief_transition.h` (no anchor moved there) plus a 9-line hook after the `chiefContact` call in
`interaction.cpp` (which moved the six anchors below it by +9 -- the semantic checker caught them and they
were re-mapped).  The
earlier moves were the 21 `WINDING_GATED_FSM` lines (`interaction.cpp`, macro OFF -- preprocessor-identical
without it), two stale in-source comment anchors (`simulation.cpp`, `initSim.cpp`) and the macro-guarded
`PARENT_*` candidate blocks.  **Behaviour equivalence for a comment-only change is proven separately** by
preprocessing `interaction.cpp` with every candidate macro OFF and comparing with `HEAD` byte for byte
(the method used in the WORK_PLAN entries of 19 Sep 2026), not by this hash.

Nine files differ from the frozen per-file table above (`attractor.h`, `bridge.cpp`, `charges.cpp`,
`chief_transition.h`, `initSim.cpp`, `interaction.cpp`, `simulation.cpp`, `simulation.h`, `utils.cpp`;
re-derived 19 Sep 2026): the working tree has accumulated post-freeze work, and the frozen table is
deliberately left as the state at the `model-ref-v1` tag (its per-file hashes still combine to
`5b0944...`, re-verified 19 Sep 2026).

The WP8 candidate macros (`HOMB_PRODUCER_FSM`, `HOMB_CONSUMER_TRANSPORT`, `POLAR_MAGNITUDE_FSM`,
plus the earlier `EM_FORCE_PREREQ`, `DD_INTRA_ISLAND_FIX`, `EM_NOS2B_FSM`,
`POLAR_BOOTSTRAP_ADDRESS`, `POLAR_BROADCAST_WAVE` and the `ORPHAN_*` family) are rows of the table
above, which is the authoritative inventory.  Build variants:
`experiments/build_rest_shell_probe{_boot,_homb,_homb2,_homb3}.bat`.
The reference build (all candidate macros OFF) is behaviour-checked two ways: by the source fingerprint
(`experiments\model_fingerprint.ps1`) and, whenever a comment edit moves that hash, by preprocessing
`src\model\interaction.cpp` with every candidate macro OFF and comparing the result with the previous
revision byte for byte (the method used in the WORK_PLAN entries of 19 Sep 2026).  The coupling-constant
probe that used to provide the third check was retired that day -- see `attic/alpha_campaign/README.md`.

## Reference harness recipes

```bat
rem model source fingerprint (the reference drift check)
powershell -NoProfile -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1

rem production-path island census
experiments\build_island_census.bat
build\island_census\island_census.exe 64 16384 build\island_census\run64
```

## Frozen expected values (acceptance for a reproducer)

*(moved here on 19 Sep 2026 from `experiments/DATA_DEPOSIT.md`, which was retired with the rest of the
paper-facing documents -- see `attic/submission_docs/README.md`.)*

Deterministic reference build (all candidate macros OFF), canonical superposed Platonic seed.  A third
party should reproduce these rows; deviations mean a different kernel or a macro-enabled binary.

| command | expected |
|---|---|
| `powershell -File experiments\model_fingerprint.ps1` | `MATCH: model matches the reference fingerprint` |
| `island_census.exe 12 16384 out` | frame 2 on: `K=235 D=8 S=0 P=0`, groups 235, `max_population=2`, one centre, 0 events from frame 3 |
| `island_census.exe 12 64 out` | frame 2 on: `K=145 D=5 S=0 P=93`, groups 145, `max_population=2`, 0 events from frame 3 |
| `island_census.exe 4 16384 out 15` | frame 2 on: `K=667 D=8 S=0 P=0`, groups 667, `max_population=2` (**~2 h**) |
| `island_census_exc_fix.exe 12 16384 out` | 81 groups of population 3, `K=81 D=162`, unresolved 0 |
| `p7_concentration_ref.exe 10 co` | one promotion at frame 2, final `2 K + 1 D`, `max_pop = 2` |
| `p7_concentration_ref.exe 10 spread` | zero contacts and zero promotions, final `1 K + 2 D`, `max_pop = 3` |
| `p7_concentration_fix.exe 10 co` | same contacts as the reference, zero promotions, final `1 K + 2 D` |

Cost profile, so a run can be scheduled: the fingerprint check is sub-second, `run_all.bat` completes in
under a minute, the `L=9` census is ~1 min/frame (~12 min for 12 frames), the P7 pair and the analyzer are
sub-second, and the `L=15` check is ~31-36 min per frame.


Full CLI conventions and acceptance criteria live in the matching
`experiments\*.md` note.  The reference drift check is the source fingerprint
(`experiments\model_fingerprint.ps1`); the 200-frame coupling-probe measurement that used to be quoted here
was **retired on 19 Sep 2026** with the rest of the coupling-constant campaign, and its historical values
live in `attic/alpha_campaign/` (`README.md`, `RESULTS.md`).

## Gate G0 -- GPU decision (CPU-only reference)

**Decision: the reference path is CPU-only; the GPU path is unsupported.**
Rationale: the `Makefile` expects CUDA v13.2 while only v11.8 is installed
here, and the engine (`simulation.cpp`) refuses CUDA activation because the
legacy GPU kernels lack the current contact/DDA/inertia rules.  The CUDA
sources (`src/cuda/*.cu`) remain in the tree but are **not** built or
validated.  A dedicated CUDA bring-up would be a separate work package if the
runtime budget of WP2 makes it necessary.

## Known dead / experimental code paths (for the manuscript's Limitations)

- `M`/`Mbar` hook: present but disabled; not part of the ordinary dynamics.
- `color_fsm.inc` / `color_matching.inc`: only reachable behind
  `COLOR_*_FSM`; excluded from the reference build.
- The polarization broadcast does not self-bootstrap from the zero-polarization
  seed (`attic/closed_programmes/SEED_ASYMMETRY.md`); the bootstrap macros are experimental.
- Host-side arbitration (source arrays, contact lists, integer DDA) and the
  whole-layer `applyMomentum` are non-local; the implementation must not be
  described as a strictly local, integer-only transition.

