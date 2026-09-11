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
                    CSVs and the campaign notes (RESULTS.md, DESIGN docs, WORK_PLAN.md)
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
rem coupling / falsification probe (the alpha_A..F campaign)
experiments\build_probe.bat
experiments\alpha_probe.exe 7 4 200 16384 256

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
  `937449ff97712e7c0095eb7992137eed1529c59fdd1985a5ed517753b9f714bf` (the
  historical `model-ref-v1` value was `5b0944...`; it advanced when the
  macro-guarded WP4.2/WP3.3 spike macros `EM_FORCE_PREREQ` / `DD_INTRA_ISLAND_FIX`
  were added -- the reference build behaviour is unchanged).  Verify drift with
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
- The campaign notes are explicit about their own limits; read the
  `*_REVALIDATION` and `RESULTS.md` notes before reusing any table.

## Current scientific status (one paragraph)

The six pre-registered candidates for the fine-structure constant (alpha_A..F)
all fail at accessible lattice sizes; the superposed Platonic seed does not
self-assemble into `9L` islands of `L/3` constituents; and the mediated
photon/graviton repulsion between equal-charge islands is at a documented
geometric impasse. Controlled positives exist (K-only island transport; a
family-preserving `81 x 3` exclusion control). Full detail:
`experiments\RESULTS.md` and the WORK_PLAN.

## Documentation map

| File | Contents |
|---|---|
| `experiments\WORK_PLAN.md` | The active plan to finish the project and make the manuscript peer-review ready |
| `doc\REFERENCE_CONFIG.md` | Frozen reference build: MODEL_VERSION (tag `model-ref-v1`), compiler flags, candidate-macro list, model fingerprint |
| `experiments\model_fingerprint.ps1` | Recompute/verify the model source fingerprint (drift check) |
| `experiments\RESULTS_v2.md` | WP2: reproduced reference measurements (alpha_A L-sweep, determinism, S-scan) |
| `experiments\run_wp2_sweep.ps1` | WP2 driver: alpha_A L-sweep (+ batched S-scan) -> `build\wp2\` |
| `experiments\analyze_wp2_scan.ps1` | WP2: summarize an S-scan CSV (open/closed split, median/mean) |
| `experiments\plot_wp2_scan.ps1` | WP2: dependency-free SVG plot of an S-scan CSV |
| `experiments\RESULTS.md` | The alpha_A..F falsification campaign, S-scan, gravity probes |
| `experiments\REVISION_PLAN.md` | Editor-response blueprint for restructuring `manuscript.tex` |
| `experiments\PHOTON_MEDIATION_DESIGN.md` | Photon/graviton-mediated interaction design (schemes A-E) |
| `experiments\INERTIA.md`, `INERTIA_REVALIDATION.md` | Island-inertia transport experiments |
| `experiments\ISLAND_CENSUS.md`, `ATTRACTOR_CENSUS.md` | Production-path aggregation census |
| `experiments\ABLATION_81x3.md`, `LIGHTMATTER_DECOUPLE.md`, `PBSB_ISLANDS.md` | Candidate aggregation/EM mechanisms |
| `experiments\POSTULATE_VS_EMERGENT.md` | Postulate-vs-emergent table (editor response, Limitations) |
| `doc\manuscript.tex` | The manuscript (build: `doc\latexpdf.bat`) |

## Notes

- Generated artifacts (`build/`, `obj/`, `*.exe`, `*.dll`, `*.log`, `*.obj`,
  `attic/`) are gitignored.
- The `experiments\` folder is large: sources and CSVs are kept, binaries are
  ignored.
 
