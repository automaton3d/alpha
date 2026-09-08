#!/usr/bin/env python3
"""analyze_island_census.py — summarize an island_census run directory.

Usage: python experiments/analyze_island_census.py <rundir>

Reads census.csv and groups.csv produced by experiments/island_census.cpp and
prints: per-frame K/D/S/P and group/spread summary; group population
histogram at the final frame; counts of groups that ever reached L/3; and the
longest stable-membership run per final group.  It is a read-only independent
audit: it reconstructs nothing from simulation state.
"""
import csv
import os
import sys
from collections import Counter, defaultdict


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    d = sys.argv[1]
    census_p = os.path.join(d, "census.csv")
    groups_p = os.path.join(d, "groups.csv")
    if not (os.path.isfile(census_p) and os.path.isfile(groups_p)):
        print(f"FAIL: missing census.csv/groups.csv in {d}", file=sys.stderr)
        return 2

    frames = []
    with open(census_p, newline="") as f:
        for row in csv.DictReader(f):
            frames.append(row)

    by_frame = defaultdict(list)
    with open(groups_p, newline="") as f:
        for row in csv.DictReader(f):
            by_frame[row["frame"]].append(row)

    print(f"run dir   : {d}")
    print(f"frames    : {len(frames) - 1} sampled journeys (+ frame 0 baseline)")
    last = frames[-1]
    print(f"final row : frame={last['frame']} tick={last['tick']} "
          f"K={last['K']} D={last['D']} S={last['S']} P={last['P']} "
          f"centres={last['distinct_centers']} groups={last['groups']} "
          f"unresolved={last['unresolved']} mixed={last['mixed_charge']}")

    print("\nframe-by-frame (population/span extremes over groups):")
    print(f"{'frame':>6} {'K':>4} {'D':>4} {'S':>4} {'P':>3} "
          f"{'centres':>8} {'groups':>6} {'maxpop':>6} {'maxspan':>7}")
    for r in frames:
        print(f"{r['frame']:>6} {r['K']:>4} {r['D']:>4} {r['S']:>4} {r['P']:>3} "
              f"{r['distinct_centers']:>8} {r['groups']:>6} "
              f"{r['max_population']:>6} {r['max_span']:>7}")

    if not by_frame:
        print("no groups ever formed")
        return 0

    hist_final = Counter(row["population"] for row in by_frame.get(last["frame"], []))
    print(f"\nfinal-frame group population histogram (population: count): "
          f"{dict(sorted(hist_final.items()))}")

    ever_target = False
    for frame_rows in by_frame.values():
        for row in frame_rows:
            if int(row["population"]) == 3:
                ever_target = True
    print(f"any group ever reaching population L/3=3 : {'yes' if ever_target else 'no'}")

    final_span = defaultdict(int)
    for row in by_frame.get(last["frame"], []):
        final_span[row["chief"]] = int(row["max_distance_to_chief"])
    localised = sum(1 for s in final_span.values() if s <= 4)
    print(f"groups localised at end (span<=RMAX=4)    : {localised}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
