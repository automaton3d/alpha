#!/usr/bin/env python3
"""Stdlib twin of make_synth.py: the same two synthetic scenarios, no numpy/pandas.

  analysis/attractor.csv  -- mean-reverting islands around N* = 30, with turnover
  analysis/partition.csv  -- the basic32_seed_1 failure mode: constant N, zero fluxes

The first is the POSITIVE control: a working harness must recover N* ~ 30 with a
small surrogate p (PASS).  The second is the NEGATIVE control: the harness must
report INCONCLUSIVE (zero escapes) rather than a spurious attractor.

Usage: python make_synth_stdlib.py [--outdir analysis]
"""

import argparse
import csv
import os
import random


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--outdir", default="analysis")
    ap.add_argument("--seed", type=int, default=7)
    args = ap.parse_args()
    os.makedirs(args.outdir, exist_ok=True)
    rng = random.Random(args.seed)

    # --- attractor: dN = 0.15*(30 - N) + noise, integer fluxes (Poisson-ish)
    rows = []
    for isl in range(6):
        N = rng.randint(10, 60)
        for t in range(300):
            drift = 0.15 * (30 - N)
            lam_cap = max(0.1, 2.0 + drift / 2.0)
            lam_esc = max(0.1, 2.0 - drift / 2.0)
            cap = poisson(rng, lam_cap)
            esc = poisson(rng, lam_esc)
            rows.append(("att", isl, t, N, cap, esc))
            N = max(1, N + cap - esc)
    write(os.path.join(args.outdir, "attractor.csv"), rows)

    # --- fixed partition: constant N, zero fluxes
    rows = []
    for isl, pop in enumerate([33, 30, 28, 31, 29, 32]):
        for t in range(200):
            rows.append(("part", isl, t, pop, 0, 0))
    write(os.path.join(args.outdir, "partition.csv"), rows)
    print("wrote %s/{attractor,partition}.csv" % args.outdir)


def poisson(rng, lam):
    """Knuth's method; lam is small here."""
    import math
    L = math.exp(-lam)
    k, p = 0, 1.0
    while True:
        p *= rng.random()
        if p <= L:
            return k
        k += 1


def write(path, rows):
    with open(path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["run", "island", "frame", "N", "captures", "escapes"])
        w.writerows(rows)


if __name__ == "__main__":
    main()
