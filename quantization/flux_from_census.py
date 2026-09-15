#!/usr/bin/env python3
"""Build the flux CSV that quantize.py expects, from a census run's per-chief log.

The census runner already writes the flux data in exactly the right shape:
`constituents.csv` has one row per (frame, chief) with the number of delegates
attached to that chief and the EXACT captures/escapes measured from consecutive
membership snapshots (not derived from dN, so the "last resort" caveat of
quantize.py does not apply).

    constituents.csv:  frame,chief_w,constituents,captures,escapes
    quantize.py needs: run,island,frame,N,captures,escapes

with N = components of the group = constituents + 1 (the chief itself is not
counted in the `constituents` column).

Usage:
    python flux_from_census.py build/island_census_dir/run20 > analysis/run20.csv
    python flux_from_census.py run64=build/island_census/run64 prod=build/island_census/prod_exc_fix
    python flux_from_census.py <dir> --out analysis/flux.csv

Stdlib only (no numpy/pandas), so it runs in this environment as-is.
"""

import argparse
import csv
import os
import sys


def read_run(path, run_name):
    cpath = os.path.join(path, "constituents.csv")
    if not os.path.isfile(cpath):
        sys.exit("missing %s" % cpath)
    rows = []
    with open(cpath, newline="") as fh:
        for r in csv.DictReader(fh):
            try:
                rows.append((
                    run_name,
                    int(r["chief_w"]),
                    int(r["frame"]),
                    int(r["constituents"]) + 1,   # N: chief + delegates
                    int(r["captures"]),
                    int(r["escapes"]),
                ))
            except (KeyError, ValueError):
                continue
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("runs", nargs="+",
                    help="directories with constituents.csv, as <name>=<dir> or <dir>")
    ap.add_argument("--out", help="write the CSV here instead of stdout")
    args = ap.parse_args()

    all_rows = []
    for spec in args.runs:
        if "=" in spec:
            name, path = spec.split("=", 1)
        else:
            path, name = spec, os.path.basename(os.path.normpath(spec))
        rows = read_run(path, name)
        if not rows:
            sys.exit("%s: no usable rows" % path)
        with_turnover = sum(1 for r in rows if r[4] + r[5] > 0)
        print("# %s: %d rows, %d island-frames with any flux"
              % (name, len(rows), with_turnover), file=sys.stderr)
        all_rows += rows

    out = open(args.out, "w", newline="") if args.out else sys.stdout
    w = csv.writer(out)
    w.writerow(["run", "island", "frame", "N", "captures", "escapes"])
    for row in sorted(all_rows):
        w.writerow(row)
    if args.out:
        out.close()
        print("wrote %s (%d rows)" % (args.out, len(all_rows)), file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
