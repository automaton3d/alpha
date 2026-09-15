#!/usr/bin/env python3
"""Test the geometric-quantum hypothesis of DYNAMIC_QUANTIZATION_DERIVATION.md.

The membership theorem fixes the frozen K/D population to 2, so the only place a
*quantum* could still hide is geometry: a shell capacity, a ball capacity, or a
contact capacity.  This script computes those quantities exactly on the lattice
(toroidal wrapping, the same band convention as the model: a cell is on shell r
when r^2 <= d^2 < (r+1)^2) for L = 7..15, checks whether the contact graph at the
turnaround is complete, and then scans every groups.csv under build/ to list the
populations the dynamics has actually realised.

Usage:  python experiments/analyze_geometric_quantum.py [build]
"""

import csv
import glob
import math
import os
import sys


def wrapped(coord, L):
    """Signed shortest toroidal difference for one axis."""
    c = coord % L
    if c > (L - 1) // 2:
        c -= L
    return c


def squared_distances(LX, LY, LZ):
    """All squared toroidal distances from the origin of the given box."""
    out = {}
    for x in range(LX):
        dx = wrapped(x, LX)
        for y in range(LY):
            dy = wrapped(y, LY)
            for z in range(LZ):
                dz = wrapped(z, LZ)
                n = dx * dx + dy * dy + dz * dz
                out[n] = out.get(n, 0) + 1
    return out


def shell_counts(LX, LY, LZ, rmax):
    """N_shell(r) and |B(r)| with the model's band convention (toroidal axes)."""
    hist = squared_distances(LX, LY, LZ)
    shells, balls, cum = [], [], 0
    for r in range(0, rmax + 1):
        lo, hi = r * r, (r + 1) * (r + 1)
        n_shell = sum(c for n, c in hist.items() if lo <= n < hi)
        cum += n_shell
        shells.append(n_shell)
        balls.append(cum)
    return shells, balls


def shell_counts_flat(rmax):
    """N_shell(r) on a free sphere: the convention of the model's own instrument
    (wavefront.cpp `shellPoints`: a local box [-(r+1), r+1]^3, no wrapping, no
    cavity cut).  This is the sequence quoted in the manuscript (6, 26, 98)."""
    shells, balls, cum = [], [], 0
    for r in range(0, rmax + 1):
        n_shell = 0
        for x in range(-(r + 1), r + 2):
            for y in range(-(r + 1), r + 2):
                for z in range(-(r + 1), r + 2):
                    d2 = x * x + y * y + z * z
                    if r * r <= d2 < (r + 1) * (r + 1):
                        n_shell += 1
        cum += n_shell
        shells.append(n_shell)
        balls.append(cum)
    return shells, balls


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "build"

    print("== 1. Shell and ball capacities (model band convention) ==")
    print("torus = wrapped axes over the whole cube; flat = the free-sphere count")
    print("of the model's own instrument (wavefront.cpp shellPoints; 6, 26, 98)")
    print()
    print("%-4s %-5s %-8s %-6s %-30s %-30s" %
          ("L", "RMAX", "d_max", "conv", "N_shell(r), r=0..RMAX", "|B(r)|, r=0..RMAX"))
    for L in (7, 9, 11, 13, 15):
        rmax = L // 2
        shells, balls = shell_counts(L, L, L, rmax)
        dmax = math.sqrt(3.0) * ((L - 1) / 2.0)
        print("%-4d %-5d %-8.2f %-6s %-30s %-30s %s" %
              (L, rmax, dmax, "torus", ",".join(str(s) for s in shells),
               ",".join(str(b) for b in balls),
               ("complete (d_max <= 2*RMAX)" if dmax <= 2 * rmax else "NOT complete")))
        fs, fb = shell_counts_flat(rmax)
        print("%-4s %-5s %-8s %-6s %-30s %-30s" %
              ("", "", "", "flat", ",".join(str(s) for s in fs),
               ",".join(str(b) for b in fb)))

    print()
    print("== 2. Contact capacity of the P7 tube (15x5x5, sources co-located) ==")
    shells, balls = shell_counts(15, 5, 5, 2)
    print("RMAX = 2;  N_shell(r) = %s ;  |B(r)| = %s" %
          (shells, balls))
    print("per pair-tick capacity at r=RMAX: %d cells (all shell cells are shared"
          " when the sources coincide)" % shells[-1])
    print("P7 measured contacts at frame 2 (W=3 layers, ENCOUNTER=3 ticks): 156")

    print()
    print("== 3. Populations the dynamics has actually realised ==")
    hist = {}
    files = sorted(glob.glob(os.path.join(root, "**", "groups.csv"),
                             recursive=True))
    for path in files:
        try:
            with open(path) as fh:
                for row in csv.DictReader(fh):
                    p = row.get("population")
                    if p is None:
                        continue
                    hist[p] = hist.get(p, 0) + 1
        except Exception:
            continue
    if not hist:
        print("(no groups.csv found under %s)" % root)
    else:
        print("%d groups.csv files scanned" % len(files))
        for p in sorted(hist, key=lambda v: int(v)):
            print("  population %-3s : %d group-frames" % (p, hist[p]))
        geometric = set()
        for L in (7, 9, 11, 13, 15):
            rmax = L // 2
            shells, balls = shell_counts(L, L, L, rmax)
            geometric.update(shells)
            geometric.update(balls)
        fs, fb = shell_counts_flat(7)
        geometric_flat = set(fs) | set(fb)
        realised = {int(p) for p in hist if int(p) > 1}
        print("  realised populations > 1 : %s" % sorted(realised))
        print("  torus shell/ball caps    : %s" % sorted(geometric))
        print("  flat  shell/ball caps    : %s" % sorted(geometric_flat))
        print("  realised & torus caps    : %s" % sorted(realised & geometric))
        print("  realised & flat  caps    : %s" % sorted(realised & geometric_flat))
    return 0


if __name__ == "__main__":
    sys.exit(main())
