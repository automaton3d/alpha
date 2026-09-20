#!/usr/bin/env python3
"""
analyze_island_spectrum.py -- the size spectrum of spontaneously formed islands.

DEFINITION MEASURED HERE (the concrete one, not an invariant): quantization is the
spontaneous formation of islands whose bubbles carry the SAME charge and have
APPROXIMATELY EQUAL size -- a geometric, structural, emergent phenomenon.  So the
observable is a size DISTRIBUTION per charge word, not a value and not an invariant:

  * same charge   -> every group must be single-word (`charge_words == 1`);
  * equal size    -> the spectrum over words must concentrate on a common size
                     (how concentrated is what this script reports: mode share,
                     spread of the non-singleton sizes, words with a non-singleton);
  * spontaneous   -> measured on the REFERENCE build only (no candidate macros), and
                     the answer is compared across lattice sizes, because a partition
                     that is declared in the seed must not be scale-dependent while an
                     emergent one may be.

Input: groups.csv as written by the census harness
       (frame,tick,chief,population,seed_families,distinct_centers,
        max_distance_to_chief,charge_words,valid_chief)
       plus its sibling census.csv, used only to read W (frame 0, column S) and hence
       L = sqrt(W/3) and the seed multiplicity ISLAND_SIZE = W/(9L) that labels the
       charge word of a chief (the seed's own address map, initSim.cpp:76-80).

Usage: python analyze_island_spectrum.py <dir> [<dir> ...]
"""
import os
import sys
from collections import Counter, defaultdict


def seed_word(island):
    """Charge word the seed assigns to a W-island (initSim.cpp:76-80)."""
    w0 = island % 2
    w1 = (island >> 1) % 2
    q = w0 ^ w1
    return (island % 8) | (q << 3) | (w0 << 4) | (w1 << 5)


def read_census_w(path):
    with open(path) as fh:
        head = fh.readline().strip().split(",")
        for ln in fh:
            row = dict(zip(head, ln.strip().split(",")))
            if row["frame"] == "0":
                return int(row["S"])
    return None


def read_groups(path):
    rows = []
    with open(path) as fh:
        head = fh.readline().strip().split(",")
        for ln in fh:
            parts = ln.strip().split(",")
            if len(parts) != len(head):
                continue
            rows.append(dict(zip(head, parts)))
    return rows


def analyse(d):
    gw = read_census_w(os.path.join(d, "census.csv"))
    if not gw:
        return None
    L = int(round((gw / 3.0) ** 0.5))
    mult = gw // (9 * L)                      # ISLAND_SIZE = W/(9L)
    rows = read_groups(os.path.join(d, "groups.csv"))
    frames = sorted({int(r["frame"]) for r in rows})
    out = {"dir": d, "W": gw, "L": L, "mult": mult, "frames": frames, "per_frame": {}}
    for f in frames:
        per_word = defaultdict(list)
        mixed = 0
        for r in rows:
            if int(r["frame"]) != f or int(r["valid_chief"]) != 1:
                continue
            if int(r["charge_words"]) != 1:
                mixed += 1
            w = int(r["chief"])
            per_word[seed_word(w // mult)].append(int(r["population"]))
        sizes = Counter()
        for word, pops in per_word.items():
            for p in pops:
                sizes[p] += 1
        non_singleton = [p for pops in per_word.values() for p in pops if p >= 2]
        out["per_frame"][f] = {
            "groups": sum(len(v) for v in per_word.values()),
            "mixed": mixed,
            "sizes": dict(sorted(sizes.items())),
            "words": len(per_word),
            "words_with_non_singleton": sum(1 for v in per_word.values() if max(v) >= 2),
            "non_singleton": non_singleton,
            "distinct_centers": max(int(r["distinct_centers"]) for r in rows if int(r["frame"]) == f),
        }
    return out


def report(a):
    if not a:
        print("  (no census.csv/groups.csv)")
        return
    print("== %s" % a["dir"])
    print("   W=%d  L=%d  seed multiplicity (W-island size)=%d  frames=%s"
          % (a["W"], a["L"], a["mult"], a["frames"]))
    print("   %-6s %-7s %-6s %-34s %-8s %-6s %s"
          % ("frame", "groups", "mixed", "size histogram (population:count)",
             "non-sing", "words", "centres"))
    for f, d in a["per_frame"].items():
        hist = " ".join("%d:%d" % (p, c) for p, c in d["sizes"].items())
        print("   %-6d %-7d %-6d %-34s %-8d %-6d %d"
              % (f, d["groups"], d["mixed"], hist,
                 d["words_with_non_singleton"], d["words"], d["distinct_centers"]))
    print("   REFERENCE fact to check: every charge word carries the same spectrum?")
    last = a["per_frame"][max(a["frames"])]
    if last["non_singleton"]:
        ns = last["non_singleton"]
        print("   non-singleton islands at the last frame: n=%d sizes=%s  spread(max-min)=%d"
              % (len(ns), sorted(ns), max(ns) - min(ns)))
        print("   words with a non-singleton island: %d of %d reachable words"
              % (last["words_with_non_singleton"], last["words"]))
    else:
        print("   no non-singleton island at the last frame (pure singletons)")
    span = max(v["groups"] for v in a["per_frame"].values()) - \
        min(v["groups"] for v in a["per_frame"].values() if v["groups"] > 0)
    print("   group-count stability across frames: max-min = %d" % span)
    print()


def main():
    dirs = sys.argv[1:]
    if not dirs:
        print(__doc__)
        print("usage: python analyze_island_spectrum.py <census outdir> [...]")
        return 1
    for d in dirs:
        report(analyse(d))
    return 0


if __name__ == "__main__":
    sys.exit(main())
