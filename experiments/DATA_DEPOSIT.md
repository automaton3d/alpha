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
`doc/manuscript.pdf` (pdflatex + biber, zero warnings).

## Table -> script map

| Paper artefact | Harness | Build / command |
|---|---|---|
| `tab:campaign` (alpha_A..F) | `experiments/alpha_probe.cpp` | `experiments\build_probe.bat`; `alpha_probe.exe 7 4 200 16384 256` |
| `tab:sieve` (sieve sweep) | `experiments/alpha_probe.cpp` | `alpha_probe.exe 7 4 200 <S> 256` for `S in {16384..8}` |
| `tab:scaling` (wavefront metrics) | `lorentz_mm.cpp` (historical) | see the Reproducibility section |
| `tab:claim-evidence` | (map) | every row cites its own script |
| `tab:postulate-emergent` | (map) | `experiments/POSTULATE_VS_EMERGENT.md` |
| ablation `81 x 3` | `experiments/lightmatter_decouple.cpp` | `experiments\build_lightmatter_decouple.bat` |
| island census | `experiments/island_census.cpp` | `experiments\build_island_census.bat` |
| island inertia | `experiments/inertia_revalidation.cpp` | `experiments\build_inertia_revalidation.bat` |
| EM reorder (Route B) | `experiments/pbsb_two_wide.cpp` | `experiments\build_pbsb_two_wide.bat` |

## The reproducible-table set

`experiments/RESULTS_v2.md` is the canonical table set for the alpha campaign,
produced by `experiments/run_wp2_sweep.ps1` (L-sweep and S-scan) and summarised by
`experiments/analyze_wp2_scan.ps1` / `experiments/plot_wp2_scan.ps1`.

## Versioning and licence

- Cite the submission commit hash and the model fingerprint in the deposit.
- Licence to be set by the author; the deposit should match the repository's.
