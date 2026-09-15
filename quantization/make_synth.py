#!/usr/bin/env python3
"""Synthetic test data for quantize.py.

Generates two runs:
  attractor.csv  -- mean-reverting island populations around N*=30
  partition.csv  -- basic32_seed_1 failure mode: fixed partition, zero escapes
"""
import numpy as np
import pandas as pd

rng = np.random.default_rng(7)

# --- attractor: dN = 0.15*(30 - N) + noise, with turnover fluxes
rows = []
for isl in range(6):
    N = rng.integers(10, 60)
    for t in range(300):
        drift = 0.15 * (30 - N)
        lam_cap = max(0.1, 2.0 + drift / 2)
        lam_esc = max(0.1, 2.0 - drift / 2)
        cap, esc = rng.poisson(lam_cap), rng.poisson(lam_esc)
        rows.append((f"att", isl, t, N, cap, esc))
        N = max(1, N + cap - esc)
pd.DataFrame(rows, columns=["run", "island", "frame", "N", "captures",
                            "escapes"]).to_csv("analysis/attractor.csv",
                                               index=False)

# --- fixed partition: constant N, zero fluxes (the failure mode)
rows = []
for isl, pop in enumerate([33, 30, 28, 31, 29, 32]):
    for t in range(200):
        rows.append(("part", isl, t, pop, 0, 0))
pd.DataFrame(rows, columns=["run", "island", "frame", "N", "captures",
                            "escapes"]).to_csv("analysis/partition.csv",
                                               index=False)
print("wrote analysis/attractor.csv and analysis/partition.csv")
