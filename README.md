# alpha -- a deterministic cellular-automaton "toy universe"

`alpha` is a research codebase built around a fully specified deterministic
cellular automaton proposed as a concrete test of Wheeler's "it from bit"
idea.  Space is a periodic 3-torus plus a non-spatial internal dimension; each
cell stores a fixed-length binary word evolved by local rules built from
classical logic and elementary arithmetic.

The repository holds three layers that share the same model sources:

| Layer | What it is | Where |
|---|---|---|
| **Model (CA engine)** | The automaton: source kinds `K/S/D/P`, six charge bits, the light frame, polarization, `encounter()` | `src/model/`, `src/include/model/` |
| **GUI application** | OpenGL viewer (GLFW/GLAD/GLM/freetype), HUD, camera, recording/replay, tomography | `src/*.cpp`, `src/include/` |
| **Science campaign** | Headless harnesses (probes), Python analyses, result/design notes, the manuscript | `experiments/`, `doc/` |

## Repository layout

```
src/                GUI + core (C++20)
  model/            automaton engine (simulation, interaction, polarization, charges, ...)
  include/model/    engine headers
  include/          GUI/utility headers + vendored glm, zlib
  cuda/             optional CUDA kernels (see "Toolchain" -- currently not the reference path)
experiments/        headless harnesses (.cpp), build/run scripts (.bat/.sh/.py/.ps1),
doc/                manuscript.tex, manuscript.bib, figures, it_from_bit*.tex
glad/               OpenGL loader source (needed by the build; not committed)
lib/                prebuilt import libs (glfw3dll.lib, zlib.lib)
fonts/, logo*.png   runtime assets
build/              build outputs (gitignored)
attic/              pre-WP0 root artifacts parked here (gitignored)
```

## Toolchain

- **MSVC** (Visual Studio 18 / v14.50 was used for the reference build), C++20.
- **vcpkg** at `E:\vcpkg\installed\x64-windows` (override with `VCPKG_ROOT`).
  Provides glfw3, freetype, zlib, bz2, brotli, libpng.
- **Build system:** `nmake` (see `Makefile`).
- **CUDA is NOT the reference path.** The GUI/engine CPU build is the reference.
  The CUDA objects are optional (`nmake USE_CUDA=1`), the Makefile expects CUDA
  v13.2 while only v11.8 is installed here, and the app itself refuses GPU
  activation because the legacy GPU kernels lack the current contact/DDA/inertia
  rules. Treat the GPU path as unsupported until a CUDA bring-up task is scheduled.

## Build the GUI (CPU reference)

```bat
build_gui.bat
```

This calls `vcvars64.bat`, sets `VCPKG_ROOT`, and runs
`nmake build\automaton.exe`, producing `build\automaton.exe` plus the runtime
DLLs and assets. Run it with `nmake run` (executes from `build\`).

Before the full build, `build_gui.bat` first runs the **ODR/link gate**
(`nmake check-odr`): two tiny translation units (`experiments\odr_gate_tu1.cpp`,
`odr_gate_tu2.cpp`) include the shared model headers and are linked together,
so a header that *defines* a symbol with external linkage without `inline`
fails in seconds with LNK2005/LNK1169 instead of after the full 45-object
build. `run_all.bat` runs the same gate as step 0.

For a one-command reproducibility smoke test (GUI build + headless probe),
use the root `run_all.bat`:

```bat
run_all.bat            rem quick: GUI build + probe smoke, writes build\run_all\report.txt
set FULL=1 && run_all.bat   rem also runs the 200-frame reference probe
```

## Build and run the headless harnesses

Every experiment has its own self-contained build script under `experiments\`.
They compile the model sources directly and emit an `.exe` next to a per-run
output directory. Representative examples:

```bat
rem reference drift check (the coupling probe that used to head this list was
rem retired on 19 Sep 2026 -- see attic/alpha_campaign/README.md)
powershell -NoProfile -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1

rem production-path island census (chief aggregation)
experiments\build_island_census.bat
build\island_census\island_census.exe 64 16384 build\island_census\run64

rem island-inertia revalidation (production vs matched FSM)
experiments\build_inertia_revalidation.bat
python experiments\run_inertia_revalidation.py production

rem two-bubble gravity / rectangular tube probes
experiments\build_gravity.bat
```

Per-harness CLI conventions, acceptance criteria and results are documented in
the matching `experiments\*.md` note (see the documentation map below).

## Reference configuration and reproducibility

- The **frozen reference** is documented in `doc\REFERENCE_CONFIG.md`:
  **MODEL_VERSION** = commit `4501f25`, annotated tag `model-ref-v1`.  The
  current working-tree model fingerprint (SHA256) is
  `bacade53e4d0c0a7ae1601992579d0066a603e4ccf9020051399f8cc510a5d82` (the
  historical `model-ref-v1` value was `5b0944...`; it advances whenever a model
  source changes, comment-only edits included, so the current value is re-measured
  and documented in `doc/REFERENCE_CONFIG.md`; for a comment-only change, behaviour
  equivalence is proven by preprocessing the file with all candidate macros off).
  Verify drift with
  `powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1`
  (exit 0 = matches).
- The **reference build** is the CPU build with all candidate macros **off**
  (the ordinary `simulation()` path). Executables that enable experimental
  macros (`EXCLUSION_FSM`, `COLOR_*_FSM`, `ORPHAN_GUIDANCE_FSM`,
  `LIGHT_MATTER_FSM`, `INERTIA_MATCHED_FSM`, ...) are separate binaries and are
  **not** interchangeable measurements.
- Every quantitative number in the manuscript must be traceable to a harness
  run under the frozen reference configuration. Numbers produced before the
  inertia rework are labelled **historical** and are being regenerated
  (see `experiments\WORK_PLAN.md`, WP1-WP3).
- The harness notes are explicit about their own limits; read the limits section of
  a note before quoting its numbers.

## Current scientific status (one paragraph)

The superposed Platonic seed does not self-assemble into an island population set by
the seed's declared partition (that partition is an axiom of the internal dimension,
not a dynamical output); the mediated photon/graviton repulsion between equal-charge
islands is at a documented geometric impasse; and the island population is now
*proved* to quantise to **two constituents**, verified at `L=9` and `L=15` with
controls that separate the geometry from the transition.  (The search for a coupling
constant was set aside on 19 Sep 2026 as premature -- see
`attic/alpha_campaign/README.md`.)  Controlled positives exist (K-only island transport; a
family-preserving `81 x 3` exclusion control).  Full detail:
WORK_PLAN.

## Documentation map

| File | Contents |
|---|---|
| `experiments\WORK_PLAN.md` | The active plan to finish the project and make the manuscript peer-review ready |
| `doc\REFERENCE_CONFIG.md` | Frozen reference build: MODEL_VERSION (tag `model-ref-v1`), compiler flags, candidate-macro list, model fingerprint |
| `experiments\model_fingerprint.ps1` | Recompute/verify the model source fingerprint (drift check) |
| `experiments\INERTIA.md`, `INERTIA_REVALIDATION.md` | Island-inertia transport experiments |
| `experiments\ISLAND_CENSUS.md`, `ATTRACTOR_CENSUS.md` | Production-path aggregation census |
| `experiments\DYNAMIC_QUANTIZATION_DERIVATION.md` | Membership fixed-point theorem (population quantum 2), ledger, `P7` control, `L=15` check |
| `experiments\J1_SPIN.md` (+ `spin_probe.cpp`, `spin_ring_probe.cpp`, `build_spin_probe.bat`, `J0_spin_reader.py`, `SPIN_GATED_FSM`) | The spin programme, J0 and J1: `J = sum r x m` is structurally zero today (3206 groups, `nonzero_J = 0`, because only propeller layers are given `m`), and once a rule consults it (`SPIN_GATED_FSM` S1/S3, a leave-veto inside `SURFACE_ESCAPE_FSM`) a planted rotor `J = (0,0,10)` with zero net momentum holds its group where every control releases it (`escapes 2 -> 0`, `spin_vetoes 20`, final `1K+2D`); J2 sweeps `N = 1..6` and shows protection is FLAT in `N` and tracks `J` as a collective quantity -- a `cancel` arm carrying identical individual momenta but `J = 0` is released exactly like the `m = 0` arm |
| `experiments\CHARGE_SPECTRUM.md` (+ `analyze_charge_spectrum.py`, `pair_channel_probe.cpp`, `build_pair_channel_probe.bat`) | Item 3: the charge words and pair channels the rules allow (3 of 6 algebraically closed), the seed's 41:40 inventory verified live, the Appendix-B census arithmetic recomputed, and the measured result that the pair channel is EMPTY (only R1/R2 reach the branch) |
| `quantization\README.md`, `quantization\FINDINGS.md` | Flux harness for the attractor hypothesis (`Gamma_cap = Gamma_esc`) and its first results on real data |
| `quantization\flux_from_census.py`, `quantize_stdlib.py`, `make_synth_stdlib.py` | Adapter (`constituents.csv` -> flux CSV), stdlib port of the harness (with `--by-run`), stdlib synthetic controls |
| `doc\manuscript.tex` | The manuscript (build: `doc\latexpdf.bat`) |

*(Notes and harnesses of the **closed programmes** -- the EM/photon/orphan and gravity lines, the
colour/election/cohesion candidates, the falsified size-selection candidates and the paper lint -- were
retired on 19 Sep 2026 and live in `attic/closed_programmes/`; see its README for the manifest.  This map
lists only what is active.)*

## Notes

- Generated artifacts (`build/`, `obj/`, `*.exe`, `*.dll`, `*.log`, `*.obj`,
  `attic/`) are gitignored.
- The `experiments\` folder is large: sources and CSVs are kept, binaries are
  ignored.
 
