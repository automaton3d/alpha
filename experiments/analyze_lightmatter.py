#!/usr/bin/env python3
"""analyze_lightmatter.py — stability/N* audit for a lightmatter_decouple run.

Usage: python experiments/analyze_lightmatter.py <rundir>

Reads trace.csv and groups.csv written by experiments/lightmatter_decouple.cpp
and prints: per-frame centres; per-chief population time series (mean, std,
min, max, frames present); a pooled OLS of dN vs N over consecutive frames of
the same chief (attractor N* = -intercept/slope when slope < 0); final group
census; and transient size-3 group occurrences.  Read-only audit.
"""
import csv
import os
import sys
from collections import defaultdict


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    d = sys.argv[1]
    if not all(os.path.isfile(os.path.join(d, f))
               for f in ("trace.csv", "groups.csv")):
        print(f"FAIL: missing trace.csv/groups.csv in {d}", file=sys.stderr)
        return 2

    frames = []
    with open(os.path.join(d, "trace.csv"), newline="") as f:
        for row in csv.DictReader(f):
            frames.append({k: int(row[k]) for k in row})

    series = defaultdict(list)   # chief -> list of (frame, population)
    with open(os.path.join(d, "groups.csv"), newline="") as f:
        for row in csv.DictReader(f):
            series[int(row["chief"])].append(
                (int(row["frame"]), int(row["population"])))

    print(f"run dir   : {d}")
    print(f"frames    : {len(frames) - 1} sampled journeys (+ baseline)")

    print("\nper-frame centres / chiefs / size3 / unresolved:")
    for r in frames:
        print(f"  f{r['frame']:>3}: centres={r['centers']:>3} "
              f"chiefs={r['chief_centers']:>3} K={r['K']:>3} D={r['D']:>3} "
              f"size3={r['groups_of_three']:>2} unresolved={r['unresolved']:>3}")

    # per-chief population statistics from frame 2 onward
    print("\nper-chief population (frames 2..end):")
    total = 0
    npoints = 0
    xs, ys = [], []      # (population, dN) pooled for the attractor OLS
    for chief in sorted(series):
        pts = [(fr, p) for fr, p in series[chief] if fr >= 2]
        pts.sort()
        if not pts:
            continue
        pops = [p for _, p in pts]
        mean = sum(pops) / len(pops)
        var = sum((p - mean) ** 2 for p in pops) / len(pops)
        print(f"  chief {chief:>4}: frames={len(pts):>2} "
              f"mean={mean:7.2f} std={var ** 0.5:6.2f} "
              f"min={min(pops):>3} max={max(pops):>3} last={pops[-1]:>3}")
        for (fa, pa), (fb, pb) in zip(pts, pts[1:]):
            if fb == fa + 1:
                ys.append(pb - pa)
                xs.append(pa)
                total += pa
                npoints += 1

    if npoints >= 3:
        n = len(xs)
        mx = sum(xs) / n
        my = sum(ys) / n
        sxx = sum((x - mx) ** 2 for x in xs)
        sxy = sum((x - mx) * (y - my) for x, y in zip(xs, ys))
        slope = sxy / sxx if sxx else 0.0
        intercept = my - slope * mx
        ssr = sum((y - (intercept + slope * x)) ** 2 for x, y in zip(xs, ys))
        sst = sum((y - my) ** 2 for y in ys) or 1.0
        nstar = (-intercept / slope) if slope < 0 else None
        print(f"\npooled attractor OLS: dN = {slope:+.4f}*N + {intercept:+.2f} "
              f"(r2={1 - ssr / sst:.3f}, points={n})")
        print(f"N* (=-intercept/slope) = "
              f"{nstar:.1f}" if nstar else "N*: no restoring slope (slope>=0)")

    size3_frames = [r["frame"] for r in frames if r["groups_of_three"] > 0]
    print(f"\nsize3 groups appeared at frames: "
          f"{size3_frames if size3_frames else 'never'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
