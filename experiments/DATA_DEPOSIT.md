# Data and code deposit plan (WP7)

This note lists what to deposit with the manuscript (Zenodo/OSF) and how a reader
or referee reproduces every quantitative claim.  It is a plan, not a submission:
the author performs the actual deposit.

## What to deposit

1. The **repository at the submission commit**, with a DOI.  Record the commit
   hash and the model **fingerprint** (`doc/REFERENCE_CONFIG.md`) in the deposit
   metadata.  The tag `model-ref-v1` marks the *model source* freeze; the
   manuscript revision is a later commit on the same branch.
2. The **reproducible bundle**: sources, build scripts, and the result tables and
   logs under `experiments/` and `build/` (the CSVs are cheap and useful; the
   binaries are not essential).
3. The repository `README.md` and this deposit note.

Do **not** deposit scratch binaries, the `attic/` folder, or editor backups.

## Reproduce in three steps

```bat
run_all.bat
powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1
doc\latexpdf.bat
```

The first builds the reference GUI and the headless probe; the second must print
`MATCH` (the model source state matches the frozen reference); the third rebuilds
`doc/manuscript.pdf` (pdflatex + biber; no undefined references).

## Table -> script map

| Paper artefact | Harness | Build / command |
|---|---|---|
| `tab:campaign` (alpha_A..F) | `experiments/alpha_probe.cpp` | `experiments\build_probe.bat`; `alpha_probe.exe 7 4 200 16384 256` |
| `tab:sieve` (sieve sweep) | `experiments/alpha_probe.cpp` | `alpha_probe.exe 7 4 200 <S> 256` for `S in {16384..8}` |
| `tab:scaling` (wavefront metrics) | `lorentz_mm.cpp` (historical) | see the Reproducibility section |
| `tab:claim-evidence` | (map) | every row cites its own script |
| `tab:postulate-emergent` | (map) | `experiments/POSTULATE_VS_EMERGENT.md` |
| ablation `81 x 3` | `experiments/lightmatter_decouple.cpp` | `experiments\build_lightmatter_decouple.bat` |
| island census | `experiments/island_census.cpp` | `experiments\build_island_census.bat`; `island_census.exe <frames> <sieve> <outdir> [EL]` |
| membership ledger `tab:ledger` | `experiments/island_census.cpp` | reference `235 K + 8 D`; `S=64` `145 K + 5 D`; candidate `81 K + 162 D` |
| `subsec:fixed-point` theorem | (proof + this table) | `experiments/DYNAMIC_QUANTIZATION_DERIVATION.md` |
| membership ledger, open gate | `experiments/island_census.cpp` | `island_census.exe 12 64 build\island_census\sweep_S64` (~11 min) |
| sieve sweep (item A) | `experiments/island_census.cpp` + `analyze_sieve_sweep.py` | `python experiments\analyze_sieve_sweep.py build\island_census` |
| lattice-side falsifier `L=15` | `experiments/island_census.cpp` | `island_census.exe 4 16384 build\island_census\L15 15` (**~2 h**) |
| P7 co-located vs spread | `experiments/p7_concentration.cpp` | `experiments\build_p7_concentration.bat`; `<1 s` per run |
| distinct-family mediation (item C) | `experiments/alpha_probe.cpp` (orphan + EM) | `experiments\build_probe_em_orphan.bat`; see `PHOTON_MEDIATION_FAR.md` |
| geometric-quantum test | `experiments/analyze_geometric_quantum.py` | `python experiments\analyze_geometric_quantum.py build` (seconds) |
| island inertia | `experiments/inertia_revalidation.cpp` | `experiments\build_inertia_revalidation.bat` |
| EM reorder (Route B) | `experiments/pbsb_two_wide.cpp` | `experiments\build_pbsb_two_wide.bat` |

## The reproducible-table set

`experiments/RESULTS_v2.md` is the canonical table set for the alpha campaign,
produced by `experiments/run_wp2_sweep.ps1` (L-sweep and S-scan) and summarised by
`experiments/analyze_wp2_scan.ps1` / `experiments/plot_wp2_scan.ps1`.

## Frozen expected values (acceptance for a reproducer)

Deterministic reference build (all candidate macros OFF), canonical superposed
Platonic seed.  A third party should reproduce these rows; deviations mean a
different kernel or a macro-enabled binary.

| command | expected |
|---|---|
| `alpha_probe.exe 7 4 200 16384 256` | `active-passes = 6468`, `s2B = 0`, `pairs = 0`, `alpha_A = 0.003756878` |
| `island_census.exe 12 16384 out` | frame 2 on: `K=235 D=8 S=0 P=0`, groups 235, `max_population=2`, one centre, 0 events from frame 3 |
| `island_census.exe 12 64 out` | frame 2 on: `K=145 D=5 S=0 P=93`, groups 145, `max_population=2`, 0 events from frame 3 |
| `island_census.exe 4 16384 out 15` | frame 2 on: `K=667 D=8 S=0 P=0`, groups 667, `max_population=2`, `pop==L/3_at_end=0` (**~2 h**) |
| `island_census_exc_fix.exe 12 16384 out` | 81 groups of population 3 (`= L/3`), `K=81 D=162`, unresolved 0 |
| `p7_concentration_ref.exe 10 co` | one promotion at frame 2, final `2 K + 1 D`, `max_pop = 2` |
| `p7_concentration_ref.exe 10 spread` | zero contacts and zero promotions, final `1 K + 2 D`, `max_pop = 3` |
| `p7_concentration_fix.exe 10 co` | same contacts as the reference, zero promotions, final `1 K + 2 D` |
| `alpha_probe_orphan.exe 11 4 20 16384 120 no no 0 no m 8 photon 0 far` | merges (`closest approach 0.00`, `recruit_repel = 0`) -- the item-C negative |
| `python analyze_sieve_sweep.py build\island_census` | one line per `sweep_S*` directory, `maxpop seen = 2` in every regime |
| `python analyze_geometric_quantum.py build` | `complete` for every `L = 7..15`; capacities `26, 66, 158, ...` / `27, 93, 251, ...`; realised populations `1..24, 26, 27, 28, 30, 33` |

Cost profile, so a referee can schedule the checks: the `alpha_probe` reference run
takes **2.87 s** (re-run at this working tree during the WP6/WP7 pass, reproducing
`6468/0/0` and `alpha_A = 0.003756878` exactly), `run_all.bat` completes in under
a minute, the `L=9` census is ~1 min/frame (~12 min for 12 frames), the P7 pair
and the analyzer are sub-second, and the `L=15` check is ~31-36 min per frame.

Reproduction status at the WP6/WP7 pass: `run_all.bat` reported
`gui_build OK / probe_build OK / probe_smoke OK` (log `build/run_all_deposit.log`,
report `build/run_all/report.txt`), and the 200-frame reference probe was run
separately (`build/run_all_deposit_ref.log`).

## Versioning and licence

- Cite the submission commit hash and the model fingerprint in the deposit.
- Licence to be set by the author; the deposit should match the repository's.
