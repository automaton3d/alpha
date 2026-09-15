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
  `428ece873cddf82fb5fcdfeed14fc4c9e769ccf08ebd5c0fb792bb7edb4b2ca8` (the
  historical `model-ref-v1` value was `5b0944...`; it advanced when the
  macro-guarded spike macros `EM_FORCE_PREREQ` / `DD_INTRA_ISLAND_FIX` /
  `EM_NOS2B_FSM` were added -- the reference build behaviour is unchanged).
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
- The campaign notes are explicit about their own limits; read the
  `*_REVALIDATION` and `RESULTS.md` notes before reusing any table.

## Current scientific status (one paragraph)

The six pre-registered candidates for the fine-structure constant (alpha_A..F)
all fail at accessible lattice sizes; the superposed Platonic seed does not
self-assemble into `9L` islands of `L/3` constituents; the mediated
photon/graviton repulsion between equal-charge islands is at a documented
geometric impasse; and the island population is now *proved* to quantise to **two
constituents** -- never to the postulated `L/3` -- a statement verified at `L=9`
(multiplicity 3) and `L=15` (multiplicity 5), with controls that separate the
geometry from the transition.  The charge unit `L/3` is stated as an axiom of the
internal dimension.  Controlled positives exist (K-only island transport; a
family-preserving `81 x 3` exclusion control).  Full detail:
`experiments\RESULTS.md`, `experiments\DYNAMIC_QUANTIZATION_DERIVATION.md` and the
WORK_PLAN.

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
| `experiments\DYNAMIC_QUANTIZATION_DERIVATION.md` | Membership fixed-point theorem (population quantum 2), ledger, `P7` control, `L=15` check |
| `experiments\TURNOVER_ABLATION.md` (+ `turnover_ablation.cpp`, `build_turnover_ablation.bat`) | Item 2: the `EXCLUSION` on/off ablation on a configuration with turnover (the hard core isolates, it does not select) |
| `experiments\SPACING_PROBE.md` (+ `spacing_probe.cpp`, `build_spacing_probe.bat`) | Item 1: the spacing threshold (`d >= 2*RMAX+1`) and the test that no model channel provides spacing |
| `experiments\CLAIMS_LINT.md` (+ `check_claims.py`, `make check-claims`) | Item 5: the claims-vs-artifacts lint (52 STRONG / 2 WEAK / 12 NONE over 66 claim sentences) and the semantic findings the tool cannot see |
| `experiments\EMERGENCE_SEARCH.md` (+ `PHASE_DISTINCT_FSM`, modes `coPhase/five/fiveCo`) | Emergence search: identity by (charge word, breathing phase) holds `N* = m` at `m = 3, 5` with no `ISLAND_SIZE` read; the phase-spread seed route is closed; census test registered |
| `experiments\PREDICTED_NS_RULE.md` (+ `build_p7_concentration.bat`, modes `chain/gap2/mid/edge`) | Item 4: a rule with a predicted `N*` (capture by shell overlap vs escape by surface) -- prediction falsified at `gap = 3,4`; cause = contact duty cycle dropping 3/4 -> 1/4 of the 4-frame breathing period |
| `experiments\CHARGE_SPECTRUM.md` (+ `analyze_charge_spectrum.py`, `pair_channel_probe.cpp`, `build_pair_channel_probe.bat`) | Item 3: the charge words and pair channels the rules allow (3 of 6 algebraically closed), the seed's 41:40 inventory verified live, the Appendix-B census arithmetic recomputed, and the measured result that the pair channel is EMPTY (only R1/R2 reach the branch) |
| `experiments\SIEVE_SWEEP_PRODUCTION.md` | Sieve-modulus sweep on the production census (the sieve channel does not quantise) |
| `experiments\PHOTON_MEDIATION_FAR.md` | Distinct-family mediated-duo probe (no mediator-based separation) |
| `experiments\analyze_sieve_sweep.py` | Summarise every `build\island_census\sweep_S*` directory |
| `experiments\GEOMETRIC_QUANTUM.md` | Geometric-quantum hypothesis tested and falsified (contact, shell and ball capacities) |
| `experiments\BRIDGE_REVISION_IF_PROVEN.md` + `experiments\bridge_proven.tex` | Conditional revision of `sec:bridge` (as if the conjectures were proven): derivation text + the H1-H4 obligation map and swap procedure |
| `quantization\README.md`, `quantization\FINDINGS.md` | Flux harness for the attractor hypothesis (`Gamma_cap = Gamma_esc`) and its first results on real data |
| `quantization\flux_from_census.py`, `quantize_stdlib.py`, `make_synth_stdlib.py` | Adapter (`constituents.csv` -> flux CSV), stdlib port of the harness (with `--by-run`), stdlib synthetic controls |
| `experiments\analyze_geometric_quantum.py` | Reader for the above: capacities, contact-graph completeness, realised-population histogram |
| `experiments\p7_concentration.cpp` (+ `build_p7_concentration.bat`) | P7: co-located vs spread prepared island, reference vs `DD_INTRA_ISLAND_FIX` |
| `experiments\pbsb_two_wide.cpp` | WP4.3: two-cloud pB/sB probe (ordinary + `/D EM_FIRST_FSM`) |
| `experiments\POSTULATE_VS_EMERGENT.md` | Postulate-vs-emergent table (editor response, Limitations) |
| `experiments\RESPONSE_TO_REFEREES.md` | WP6: simulated referee reports + point-by-point responses + action list |
| `experiments\CANDIDATE_VERSION.md` | WP6: editor change summary for the candidate version |
| `experiments\COVER_LETTER.md` | WP7: submission cover letter (draft) |
| `experiments\DATA_DEPOSIT.md` | WP7: data/code deposit plan (what to deposit, table->script map) |
| `doc\manuscript.tex` | The manuscript (build: `doc\latexpdf.bat`) |

## Notes

- Generated artifacts (`build/`, `obj/`, `*.exe`, `*.dll`, `*.log`, `*.obj`,
  `attic/`) are gitignored.
- The `experiments\` folder is large: sources and CSVs are kept, binaries are
  ignored.
 
