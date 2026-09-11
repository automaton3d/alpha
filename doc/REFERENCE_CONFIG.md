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
| `ORPHAN_GUIDANCE_FSM` | experimental P1/P2 photon-mediated impulse (orphan shell, recruit/relay) | `interaction.cpp` (61, 624, 823), `simulation.h` (382) |
| `ORPHAN_GATE_ONLY`, `ORPHAN_NO_SHELL`, `ORPHAN_NO_RELAY`, `ORPHAN_NO_ANNIH`, `ORPHAN_KICK_CAP`, `ORPHAN_KICK_PER_WINDOW`, `ORPHAN_MEDIATOR_PROPAGATES`, `ORPHAN_MEDIATOR_SUSTAIN`, `ORPHAN_RELAY_KEEPT`, `ORPHAN_RELAY_NOOP`, `ORPHAN_RELAY_SHUTTLE` | sub-options of the orphan-mediation schemes A-E | `interaction.cpp`, `simulation.h`, `alpha_probe.cpp` |
| `EXCLUSION_FSM` | Pauli-like identity hard core across seed families + push | `interaction.cpp` (442, 558, 988) |
| `EM_FIRST_FSM` | decide the electroweak channel before identity | `interaction.cpp` (~893) |
| `EM_FORCE_PREREQ` | (WP4.2 spike) force the `EM_FIRST_FSM` prerequisite s2B+pB at each contact | `interaction.cpp` (~884) |
| `EM_NOS2B_FSM` | (Route B) drop the probabilistic sieve requirement from the EM reorder | `interaction.cpp` (~906) |
| `DD_INTRA_ISLAND_FIX` | (WP3.3 candidate) intra-family election uses the family-minimum chief, so a 3-copy family stays 1 K + 2 D | `chief_transition.h` |
| `COLOR_ENCOUNTER_FSM` | colour FSM replaces `update_lattice_cpu` | `simulation.cpp` (647) |
| `COLOR_CHIEF_FSM`, `COLOR_COHESION_FSM`, `COLOR_MATCHED_FSM` | colour/election/cohesion/matched candidates | `color_fsm.inc`, `color_matching.inc` |
| `DD_PROMOTION_FSM`, `KK_CLASH_FSM` | role transitions (D x D promote, K x K clash) | `interaction.cpp` |
| `INERTIA_MATCHED_FSM` | matched-experiment inertia harness | `experiments/inertia_matched.cpp` |
| `LIGHT_MATTER_FSM` | provisional pB/sB decoupling stand-in | `color_fsm.inc`, `experiments/lightmatter_decouple.cpp` |
| `ISLAND_SEED_EXPERIMENT` | reduced seed branch (excluded by the census) | `initSim.cpp` |
| `COAGULATION_RANDOM_FSM` | coagulation random experiment | `experiments/coagulation_random.cpp` |
| `POLAR_BOOTSTRAP_ADDRESS`, `POLAR_BROADCAST_WAVE` | experimental polarization bootstrap/broadcast | `polarization.cpp` |
| `ORPHAN_PRINT_ONLY` | probe-only source diagnostic | `experiments/alpha_probe.cpp` |
| `USE_CUDA`, `CUDA_BRIDGE_CU` | GPU path (see G0) | `bridge.cu`, `cuda_automaton.cu`, `Makefile` |

Include guards (`ATTRACTOR_H_`, `GEOMETRY_H_`, `POLARIZATION_H_`,
`SIMULATION_H_`, `WAVEFRONT_H_`) are not feature macros and are ignored.

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

Since the `model-ref-v1` freeze, the working tree added one macro-guarded
experimental block to a model source:

| Macro | Effect | Where | Reference behaviour |
|---|---|---|---|
| `EM_FORCE_PREREQ` | forces the `EM_FIRST_FSM` prerequisite (s2B + pB) at each contact (WP4.2 spike) | `interaction.cpp` (~884) | unchanged when OFF |
| `DD_INTRA_ISLAND_FIX` | intra-family election uses the family-minimum chief (WP3.3); a 3-copy family stays 1 K + 2 D | `chief_transition.h` | unchanged when OFF |
| `EM_NOS2B_FSM` | drops the probabilistic `s2B` gate from the EM reorder (Route B); needs `EM_FIRST_FSM` | `interaction.cpp` (~906) | unchanged when OFF |

Current working-tree model fingerprint:
`428ece873cddf82fb5fcdfeed14fc4c9e769ccf08ebd5c0fb792bb7edb4b2ca8`
(`interaction.cpp` = `B8E3F329...`, `chief_transition.h` = `2EABE284...`).
The reference build (all candidate macros OFF) was re-verified
behaviour-identical: `alpha_probe 7 4 200 16384 256` reports active-passes =
6468, s2B = 0, pairs = 0, alpha_A = 0.003756878.

## Reference harness recipes

```bat
rem coupling / falsification probe -- the alpha_A..F campaign
experiments\build_probe.bat
experiments\alpha_probe.exe 7 4 200 16384 256      rem EL SEP FRAMES SIEVE budget

rem production-path island census
experiments\build_island_census.bat
build\island_census\island_census.exe 64 16384 build\island_census\run64
```

Full CLI conventions and acceptance criteria live in the matching
`experiments\*.md` note.  The 200-frame `alpha_probe` run is the reference
falsification measurement; its historical value is documented in
`experiments/RESULTS.md` and is being regenerated in WP2.

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
  seed (`SEED_ASYMMETRY.md`); the bootstrap macros are experimental.
- Host-side arbitration (source arrays, contact lists, integer DDA) and the
  whole-layer `applyMomentum` are non-local; the implementation must not be
  described as a strictly local, integer-only transition.

