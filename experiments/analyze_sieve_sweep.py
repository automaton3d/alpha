#!/usr/bin/env python3
"""Summarise a sieve-modulus sweep of the production island census.

Reads build/island_census/sweep_S*/{census.csv,summary.txt} produced by

    build\\island_census\\island_census.exe [frames] [sieve] [outdir]

with the ORDINARY reference build (no candidate macro), so the only varied
input is the electroweak sieve modulus S.  Prints one line per S with the
frame-2 role census, the last frame, the maximum population reached, and the
harness report card (chiefs at end / groups at population L/3).

Usage:  python experiments/analyze_sieve_sweep.py [build/island_census]
"""

import os
import re
import sys

SKIP_FRAMES = (0, 1)   # seed + first zero-radius journey: no interaction


def read_rows(path):
    with open(path, "r") as fh:
        lines = [ln.strip() for ln in fh if ln.strip()]
    head = lines[0].split(",")
    rows = []
    for ln in lines[1:]:
        vals = ln.split(",")
        if len(vals) != len(head):
            continue
        rows.append(dict(zip(head, vals)))
    return rows


def read_summary(path):
    card = {}
    if not os.path.exists(path):
        return card
    with open(path, "r") as fh:
        for ln in fh:
            if ln.startswith("SUMMARY"):
                # keys may contain '=' and '/', e.g. "pop==L/3_at_end=0" or
                # "stable>=5frames=81": split the value off at the LAST '='.
                for tok in ln.split()[1:]:
                    if "=" in tok:
                        key, val = tok.rsplit("=", 1)
                        card[key] = val
            m = re.match(r"wall_seconds=(\S+)", ln.strip())
            if m:
                card["wall_seconds"] = m.group(1)
    return card


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "build/island_census"
    entries = []
    for d in os.listdir(root):
        m = re.match(r"sweep_S(\d+)(\w*)$", d)
        if m:
            entries.append((int(m.group(1)), m.group(2), d))
    entries.sort(key=lambda e: (-e[0], e[1]))
    if not entries:
        print("no sweep_S* directories under %s" % root)
        return 1

    hdr = ("S", "frames", "f2(K/D/g/unres/mixed/maxpop/cent)",
           "last(K/D/g/maxpop/cent)", "maxpop seen", "pop==L/3", "chiefs_end",
           "wall_s")
    print("%-8s %-6s %-28s %-24s %-10s %-9s %-10s %s" % hdr)
    for svalue, suffix, d in entries:
        s = "%d%s" % (svalue, suffix)
        cpath = os.path.join(root, d, "census.csv")
        if not os.path.exists(cpath):
            print("%-8s (no census yet)" % s)
            continue
        rows = read_rows(cpath)
        if not rows:
            print("%-8s (empty census)" % s)
            continue
        last = rows[-1]
        f2 = next((r for r in rows if int(r["frame"]) == 2), None)
        interacting = [r for r in rows if int(r["frame"]) not in SKIP_FRAMES]
        maxpop = max((int(r["max_population"]) for r in interacting), default=0)
        i = lambda r: (r["K"], r["D"], r["groups"], r["unresolved"],
                       r["mixed_charge"], r["max_population"],
                       r["distinct_centers"])
        f2s = "/".join(i(f2)) if f2 else "-"
        lasts = "/".join((last["K"], last["D"], last["groups"],
                          last["max_population"], last["distinct_centers"]))
        card = read_summary(os.path.join(root, d, "summary.txt"))
        print("%-8s %-6s %-28s %-24s %-10s %-9s %-10s %s" % (
            s, len(rows) - 1, f2s, lasts, maxpop,
            card.get("pop==L/3_at_end", "?"), card.get("chiefs_at_end", "?"),
            card.get("wall_seconds", "?")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
