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

Usage: python analyze_island_spectrum.py [--frame N] <dir> [<dir> ...]

The report is printed in two blocks: the aggregate size spectrum (as before) and the
PER-WORD spectrum -- one row per charge word, with its c/q/w0/w1 bits, class and colour,
the number of seed families carrying it, its island count, sizes and the chief addresses of
its non-singleton islands, plus slices by each ch field.  The per-word block is what tests
whether the charge word itself organises the island population (the `ch`-based reading of
quantization), and the chief addresses double as a cross-check of PAIR_MECHANISM.md: a
size-2 island's chief is the LOWER of the top two addresses of its word's highest family
(offsets m-2 and m-1 in a family of size m), verified at m = 2, 3 and 5.
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


# --- ch bit structure (simulation.h:30-39) and the manuscript's class rule -------
# Same conventions as analyze_charge_spectrum.py, so the two notes speak one language.
COLOUR_NAMES = ["N", "R", "G", "Bbar", "B", "Gbar", "Rbar", "Nbar"]


def word_bits(ch):
    return {"c": ch & 0x07, "q": (ch >> 3) & 1, "w0": (ch >> 4) & 1, "w1": (ch >> 5) & 1}


def word_class(ch):
    """signature sig = c2+c1+c0: matter if sig < 2, antimatter otherwise, sig == 3 anti-neutral."""
    c = ch & 0x07
    sig = ((c >> 2) & 1) + ((c >> 1) & 1) + (c & 1)
    if sig == 3:
        return "anti-neutral"
    return "matter" if sig < 2 else "antimatter"


def families_per_word(L):
    """How many of the seed's 9L families carry each word (`island % 8`)."""
    out = defaultdict(int)
    for island in range(9 * L):
        out[seed_word(island)] += 1
    return out


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
            per_word[seed_word(w // mult)].append(
                (w, int(r["population"]), int(r["distinct_centers"])))
        sizes = Counter()
        for word, items in per_word.items():
            for _w, p, _c in items:
                sizes[p] += 1
        non_singleton = [p for items in per_word.values() for _w, p, _c in items if p >= 2]
        out["per_frame"][f] = {
            "groups": sum(len(v) for v in per_word.values()),
            "mixed": mixed,
            "sizes": dict(sorted(sizes.items())),
            "words": len(per_word),
            "words_with_non_singleton": sum(1 for v in per_word.values()
                                            if max(p for _w, p, _c in v) >= 2),
            "non_singleton": non_singleton,
            "distinct_centers": max(int(r["distinct_centers"]) for r in rows if int(r["frame"]) == f),
            "per_word": per_word,
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


def report_per_word(a, frame=None):
    """The per-word spectrum: does `ch` organise the island population?"""
    if not a or not a["per_frame"]:
        return
    f = frame if frame is not None else max(a["frames"])
    d = a["per_frame"][f]
    fam = families_per_word(a["L"])
    print("   == per-word spectrum (frame %d) ==  ch bits: c0..c2, q, w0, w1" % f)
    print("   %-6s %-9s %-12s %-7s %8s %8s %5s  %s"
          % ("word", "c q w0 w1", "class", "colour", "families", "islands", "ns", "sizes, non-singleton chiefs"))
    for word in sorted(d["per_word"]):
        items = d["per_word"][word]
        b = word_bits(word)
        sizes = Counter(p for _w, p, _c in items)
        ns = [(w, p) for w, p, _c in items if p >= 2]
        hist = " ".join("%d:%d" % (p, c) for p, c in sorted(sizes.items()))
        chiefs = " ".join("w=%d(%d)" % (w, p) for w, p in sorted(ns))
        print("   %-6s %d %d %d %d  %-12s %-7s %8d %8d %5d  %s  %s"
              % ("0x%02X" % word, b["c"], b["q"], b["w0"], b["w1"], word_class(word),
                 COLOUR_NAMES[b["c"]], fam.get(word, 0), len(items), len(ns), hist, chiefs))

    print("   -- slices by ch field (islands / non-singletons / max population):")
    for field in ("colour", "q", "w0w1", "class"):
        agg = defaultdict(lambda: [0, 0, 0])
        for word, items in d["per_word"].items():
            b = word_bits(word)
            key = {"colour": b["c"], "q": b["q"], "w0w1": "%d%d" % (b["w0"], b["w1"]),
                   "class": word_class(word)}[field]
            slot = agg[key]
            slot[0] += len(items)
            slot[1] += sum(1 for _w, p, _c in items if p >= 2)
            slot[2] = max(slot[2], max(p for _w, p, _c in items))
        print("      %-7s %s" % (field, "  ".join(
            "%s:%d/%d/%d" % (k, v[0], v[1], v[2])
            for k, v in sorted(agg.items(), key=lambda kv: str(kv[0])))))

    per_word_islands = Counter(len(v) for v in d["per_word"].values())
    print("   -- checks: islands per populated word = %s ; words with a non-singleton = %d of %d ; mixed = %d"
          % (dict(sorted(per_word_islands.items())), d["words_with_non_singleton"], d["words"], d["mixed"]))
    if d["non_singleton"]:
        ns = sorted(d["non_singleton"])
        print("   -- non-singleton sizes across words: %s   equal? %s"
              % (ns, "yes" if len(set(ns)) == 1 else "NO"))
    frames = a["frames"]
    if len(frames) > 1:
        grew = [fr for fr in frames if a["per_frame"][fr]["sizes"] != d["sizes"]]
        print("   -- frames whose spectrum differs from frame %d: %s"
              % (f, grew if grew else "none"))
    print()


def main():
    args = sys.argv[1:]
    frame = None
    if "--frame" in args:
        i = args.index("--frame")
        frame = int(args[i + 1])
        del args[i:i + 2]
    dirs = args
    if not dirs:
        print(__doc__)
        print("usage: python analyze_island_spectrum.py [--frame N] <census outdir> [...]")
        return 1
    for d in dirs:
        a = analyse(d)
        report(a)
        report_per_word(a, frame)
    return 0


if __name__ == "__main__":
    sys.exit(main())
