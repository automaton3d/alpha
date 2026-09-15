#!/usr/bin/env python3
"""Charge algebra of the canonical seed: what the rule set actually allows.

Item 3 of "what to try next": the charge unit is an axiom, so what CAN be derived
is the *spectrum* -- the set of charge words the seed generates, which pair rules
are reachable within that set, and how the matter/antimatter classes split.  All
statements below come from the code, not from prose:

  seed word      initSim.cpp:73-80
                 island = w / ISLAND_SIZE
                 w0 = island % 2 ; w1 = (island >> 1) % 2 ; q = w0 ^ w1
                 ch = (island % 8) | (q << 3) | (w0 << 4) | (w1 << 5)
  bit layout     simulation.h:31-39  (C0 0x01, C1 0x02, C2 0x04, Q 0x08,
                 W0 0x10, W1 0x20)
  pair rules     interaction.cpp:147-173  (R1..R6, see pair_rules below)
  signature      sig = c2 + c1 + c0 ; matter if sig < 2, antimatter otherwise,
                 anti-neutral if sig == 3 (manuscript, Sect. "Charges")

Usage: python experiments/analyze_charge_spectrum.py
"""

import itertools
import sys

# ------------------------------------------------------------------ code facts

def seed_word(island):
    """The canonical seed's charge word for a W-island index (initSim.cpp:76-80)."""
    w0 = island % 2
    w1 = (island >> 1) % 2
    q = w0 ^ w1
    return (island % 8) | (q << 3) | (w0 << 4) | (w1 << 5)


def bits(ch):
    return {"q": (ch >> 3) & 1, "w0": (ch >> 4) & 1, "w1": (ch >> 5) & 1,
            "colour": ch & 0x07}


def signature(ch):
    return ((ch >> 2) & 1) + ((ch >> 1) & 1) + (ch & 1)


def word_class(ch):
    sig = signature(ch)
    if sig == 3:
        return "anti-neutral"
    return "matter" if sig < 2 else "antimatter"


COLOUR_NAMES = ["N", "R", "G", "Bbar", "B", "Gbar", "Rbar", "Nbar"]


def pair_rules(ca, cb):
    """The rule names satisfied by the pair (interaction.cpp:147-173)."""
    hit = []
    if ca == 0x00 and cb == 0x00:
        hit.append("R3 neutrino-type")
    if ca == 0x3F and cb == 0x3F:
        hit.append("R4 antineutrino-type")
    if (ca ^ cb) == 0x3F:
        hit.append("R1 graviton-type")
    qa, qb = (ca >> 3) & 1, (cb >> 3) & 1
    w0a, w0b = (ca >> 4) & 1, (cb >> 4) & 1
    w1a, w1b = (ca >> 5) & 1, (cb >> 5) & 1
    cola, colb = ca & 7, cb & 7
    if (qa ^ qb) and (w1a == w1b) and (w0a ^ w0b) and ((cola ^ colb) == 7):
        hit.append("R2 photon-type")
    if (not qa and not qb and not w1a and not w1b and w0a and w0b
            and cola == colb and cola not in (0x00, 0x07)):
        hit.append("R5 up-quark-type")
    if (qa and qb and w1a and w1b and not w0a and not w0b
            and cola == colb and cola not in (0x00, 0x07)):
        hit.append("R6 up-quark-type")
    return hit


def main():
    L = 9
    islands = 9 * L                      # 9L families
    seed_words = sorted({seed_word(i) for i in range(islands)})

    print("== 1. The seed's charge words (canonical L=9 seed, 9L = %d families) =="
          % islands)
    print("%-6s %-6s %-9s %s" % ("island", "word", "q w0 w1", "colour  class"))
    for i in range(8):                   # one period; the pattern repeats
        ch = seed_word(i)
        b = bits(ch)
        print("%-6d 0x%02X   %d %2d %2d  %-6s  %s"
              % (i, ch, b["q"], b["w0"], b["w1"],
                 COLOUR_NAMES[b["colour"]], word_class(ch)))
    print("distinct words generated: %d of 64 -> %s"
          % (len(seed_words), ", ".join("0x%02X" % c for c in seed_words)))
    counts = {}
    for i in range(islands):
        counts[seed_word(i)] = counts.get(seed_word(i), 0) + 1
    print("families per word: %s"
          % {("0x%02X" % k): v for k, v in sorted(counts.items())})
    print()

    print("== 2. Which pair rules are reachable from the seed's own words ==")
    reach = {}
    for ca, cb in itertools.combinations_with_replacement(seed_words, 2):
        for r in pair_rules(ca, cb):
            reach.setdefault(r, []).append((ca, cb))
    for r in ("R1 graviton-type", "R2 photon-type", "R3 neutrino-type",
              "R4 antineutrino-type", "R5 up-quark-type", "R6 up-quark-type"):
        hits = reach.get(r, [])
        if hits:
            print("%-22s reachable: %s%s"
                  % (r, ", ".join("0x%02X/0x%02X" % h for h in hits[:3]),
                     " ..." if len(hits) > 3 else ""))
        else:
            print("%-22s NOT reachable from seed words" % r)
    print()

    # The only writes to cell.ch are initSim.cpp:80 (the seed) and the gated
    # conjugation hook simulation.cpp:872 (ch ^= 0x1F, no-op unless mm_eps != 0).
    # Both live inside the manifold M: q ^ w0 == w1.
    manifold = [c for c in range(64) if (((c >> 3) & 1) ^ ((c >> 4) & 1)) == ((c >> 5) & 1)]
    diag = [c for c in manifold if (c & 1) == ((c >> 4) & 1) and ((c >> 1) & 1) == ((c >> 5) & 1)]
    conj = {c: c ^ 0x1F for c in manifold}
    print("== 2b. Exhaustive: the invariant manifold and each rule's domain ==")
    print("words with q ^ w0 == w1 (preserved by ch ^= 0x1F): %d of 64"
          % len(manifold))
    print("of those, the seed's diagonal (c0 = w0, c1 = w1): %d words = %s"
          % (len(diag), ", ".join("0x%02X" % c for c in diag)))
    for r in ("R1 graviton-type", "R2 photon-type", "R3 neutrino-type",
              "R4 antineutrino-type", "R5 up-quark-type", "R6 up-quark-type"):
        inM = [(a, b) for a, b in itertools.combinations_with_replacement(manifold, 2)
               if r in pair_rules(a, b)]
        print("%-22s pairs inside M: %4d%s" % (r, len(inM), "" if inM else
              "   <-- algebraically impossible for any valid word"))
    r2 = [(a, b) for a, b in itertools.combinations_with_replacement(manifold, 2)
          if "R2 photon-type" in pair_rules(a, b)]
    print("R2 pairs in M are exactly {w, w ^ 0x1F}: %s"
          % all(b == (a ^ 0x1F) for a, b in r2))
    print("conjugates of the seed's words (reachable only if mm_eps != 0): %s"
          % ", ".join("0x%02X" % conj[c] for c in diag))
    print()

    print("== 3. Matter / antimatter inventory of the seed's words ==")
    cls = {}
    for ch in seed_words:
        cls.setdefault(word_class(ch), []).append(ch)
    for k in ("matter", "antimatter", "anti-neutral"):
        print("%-13s %2d words: %s"
              % (k, len(cls.get(k, [])),
                 ", ".join("0x%02X" % c for c in cls.get(k, []))))
    print()

    print("== 4. Census arithmetic published in Appendix B (verification) ==")
    rows = [("Gluon-type", "Pair", 36760, 36760),
            ("Up-quark-type", "Pair", 42, 42),
            ("Down-quark-type", "Single", 10, 10),
            ("Electron-type", "Single", 31, 31),
            ("Photon-type", "Pair", 20, 20),
            ("Graviton-type", "Pair", 12, 12),
            ("Z-type", "Pair", 24592, 18416),
            ("W-type", "Pair", 6122, 12294),
            ("Neutrino-type", "Pair", 6082, 6082),
            ("Antiup-type", "Pair", 2, 2),
            ("Antielectron-type", "Single", 1, 0),
            ("Antiquark-type", "Single", 0, 0),
            ("Leftover", "Single", 24624, 24620)]
    orb = sum(r[2] for r in rows)
    umb = sum(r[3] for r in rows)
    print("all %d rows sum: Orbis=%d  Umbra=%d  total=%d" % (len(rows), orb, umb,
                                                             orb + umb))
    print("published total: 196,587 -> %s"
          % ("VERIFIED" if orb + umb == 196587 else "MISMATCH"))
    print("W+Z aggregate: Orbis=%d Umbra=%d  (published 30,714 vs 30,710)"
          % (24592 + 6122, 18416 + 12294))
    charged_m, charged_a = 42 + 10 + 31, 2 + 1 + 0
    print("charged matter:antimatter = %d:%d = %.1f:1 (published 83:3 = 27.7:1)"
          % (charged_m, charged_a, charged_m / max(1, charged_a)))
    print("NOTE: that table averages a hand-scripted recombination of all 64")
    print("words; sections 1-3 above are about the 8 words the seed generates.")
    print()

    print("== 5. The spectrum, and where the axiom sits ==")
    print("The electric charge is carried by the q bit and a bound state counts")
    print("fragments, so the model's spectrum is integral in units of ONE")
    print("fragment; a spectrum in thirds requires the multiplicity axiom")
    print("ISLAND_SIZE = L/3 (the seed's partition), not the algebra.")
    print("What the algebra does fix: the %d reachable words, their "
          "matter/antimatter" % len(seed_words))
    print("classes (section 3) and the reachable pair types (section 2).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
