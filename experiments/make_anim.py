#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
make_anim.py - output animation plugin for inertia_probe.

Consumes the per-light-frame layer dump "<csv>.pos" written by inertia_probe
and renders a 2-D side view (x horizontal, y vertical) of every source:
  - core dot   = source centre (lcenters[w]), coloured by kind
  - faint ring = current breathing shell radius min(t, RMAX)
Colours: K red, S blue, D yellow, P cyan  (manuscript entity colours).
Writes PPM (P6) frames into an output directory and, if ffmpeg is on PATH,
assembles them into anim.gif.

Usage:
  python make_anim.py <file.pos> <outdir> [px_per_cell] [fps]
"""
import os
import re
import struct
import subprocess
import sys


def parse_pos(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        lines = fh.read().splitlines()
    header = lines[0].split()
    assert header[0] == "P", "bad header: %r" % (lines[0],)
    LX, LY, LZ, RMAX, W, n_i, n_pi = (int(x) for x in header[1:])
    frames = []
    i = 1
    while i < len(lines):
        toks = lines[i].split()
        if not toks:
            i += 1
            continue
        assert toks[0] == "F", "bad frame line: %r" % (lines[i],)
        fnum = int(toks[1])
        tick = int(toks[2]) if len(toks) > 2 else 0
        layers = []
        for w in range(W):
            t = lines[i + 1 + w].split()
            layers.append([int(x) for x in t])
        frames.append((fnum, tick, layers))
        i += 1 + W
    return LX, LY, LZ, RMAX, W, n_i, n_pi, frames


# entity colours (r,g,b)
KCOL = (255, 96, 84)
SCOL = (108, 150, 255)
DCOL = (255, 208, 64)
PCOL = (0, 235, 235)
BG = (10, 12, 18)


def render_frame(lx, ly, rmax, n_i, layers, scale, wpx, hpx, marg, fnum, nf):
    # background
    pix = bytearray(wpx * hpx * 3)
    for idx in range(0, len(pix), 3):
        pix[idx] = BG[0]
        pix[idx + 1] = BG[1]
        pix[idx + 2] = BG[2]

    def put(x, y, col, alpha):
        if x < 0 or y < 0 or x >= wpx or y >= hpx:
            return
        o = (y * wpx + x) * 3
        a = alpha / 255.0
        pix[o] = int(col[0] * a + pix[o] * (1 - a))
        pix[o + 1] = int(col[1] * a + pix[o + 1] * (1 - a))
        pix[o + 2] = int(col[2] * a + pix[o + 2] * (1 - a))

    def col_for(kind, w):
        if kind == 0:
            return KCOL
        if kind == 3:
            return PCOL
        if kind == 2:
            return DCOL
        # S: island layers (w < n_i) share a leader -> lighter; orphans -> blue
        return SCOL

    # faint shell rings (draw first)
    for w, cx, cy, cz, kind, m0, m1, m2, r0, r1, r2, tt, pb, sb, pair, a in layers:
        col = col_for(kind, w)
        R = min(tt, rmax)
        Rp = R * scale
        for dy in range(-int(Rp) - 2, int(Rp) + 3):
            for dx in range(-int(Rp) - 2, int(Rp) + 3):
                d2 = dx * dx + dy * dy
                d = int(d2 ** 0.5)
                if abs(d - Rp) <= 1.5:
                    put(marg + cx * scale + dx, marg + cy * scale + dy,
                        col, 46)
    # bright cores
    for w, cx, cy, cz, kind, m0, m1, m2, r0, r1, r2, tt, pb, sb, pair, a in layers:
        col = col_for(kind, w)
        cr = 2 if kind != 0 else 3
        for dy in range(-cr, cr + 1):
            for dx in range(-cr, cr + 1):
                if dx * dx + dy * dy <= cr * cr:
                    put(marg + cx * scale + dx, marg + cy * scale + dy, col, 255)

    # progress bar (frame/total)
    barw = wpx - 2 * marg
    prog = int(barw * (fnum + 1) / max(1, nf))
    for bx in range(barw):
        for by in range(2):
            put(marg + bx, hpx - marg - 1 + by,
                (110, 220, 140) if bx < prog else (60, 62, 70), 255)
    return bytes(pix)


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 1
    pos = sys.argv[1]
    outdir = sys.argv[2]
    scale = int(sys.argv[3]) if len(sys.argv) > 3 else 6
    fps = float(sys.argv[4]) if len(sys.argv) > 4 else 1.0
    os.makedirs(outdir, exist_ok=True)

    LX, LY, LZ, RMAX, W, n_i, n_pi, frames = parse_pos(pos)
    marg = 10
    wpx = LX * scale + 2 * marg
    hpx = LY * scale + 2 * marg + 4

    for fnum, tick, layers in frames:
        raw = render_frame(LX, LY, RMAX, n_i, layers, scale, wpx, hpx,
                           marg, fnum, len(frames))
        with open(os.path.join(outdir, "frame_%04d.ppm" % fnum), "wb") as fh:
            fh.write(b"P6\n%d %d\n255\n" % (wpx, hpx))
            fh.write(raw)
        print("wrote frame %d/%d (tick %d)" % (fnum + 1, len(frames), tick))

    # Assemble the animation with ffmpeg (if available).
    exe = "ffmpeg.exe" if os.name == "nt" else "ffmpeg"
    gif = os.path.join(outdir, "anim.gif")
    try:
        subprocess.run(
            [exe, "-y", "-framerate", "%.3f" % fps,
             "-i", os.path.join(outdir, "frame_%04d.ppm"),
             "-loop", "0", gif],
            check=True, capture_output=True)
        print("animation written: %s" % gif)
    except FileNotFoundError:
        print("ffmpeg not found: PPM frames are in %s "
              "(assemble the GIF manually)" % outdir)
    except subprocess.CalledProcessError as e:
        print("ffmpeg failed:\n%s" % e.stderr.decode(errors="replace"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
