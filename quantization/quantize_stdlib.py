#!/usr/bin/env python3
"""quantize_stdlib.py -- stdlib port of quantize.py (same method, no numpy/pandas).

The author's harness `quantize.py` needs numpy+pandas, which are not installed in
this environment.  This file reimplements its five sections on the standard
library so that the same verdicts can be produced here; `quantize.py` remains the
reference implementation, and the two must agree when numpy/pandas are available
(they use the same estimators: OLS drift, percentile bootstrap over islands,
AR(1) residual check, within-island permutation surrogates).

Input: one row per (island, frame) with columns `run,island,frame,N,captures,escapes`
(or the minimum `island,frame,N` with --derive-fluxes).

Usage:
    python quantize_stdlib.py runs/*.csv --bins 12 --surrogates 200 --out report.txt
    python quantize_stdlib.py run.csv --derive-fluxes
"""

import argparse
import csv
import math
import os
import random
import sys


# ------------------------------------------------------------------ loading

def load(paths, derive):
    """Returns a list of records: dict(run, island, frame, N, cap, esc, dN)."""
    recs = []
    for path in paths:
        with open(path, newline="") as fh:
            rows = list(csv.DictReader(fh))
        if not rows:
            sys.exit("%s: empty" % path)
        cols = {c.lower(): c for c in rows[0].keys()}
        for need in ("island", "frame", "n"):
            if need not in cols:
                sys.exit("%s: missing column '%s' (have %s)"
                         % (path, need, list(rows[0].keys())))
        run_default = os.path.basename(os.path.dirname(os.path.abspath(path))) \
            or os.path.basename(path)
        for r in rows:
            island = r[cols["island"]]
            try:
                rec = {"run": r[cols["run"]] if "run" in cols else run_default,
                       "island": island,
                       "frame": int(float(r[cols["frame"]])),
                       "N": int(float(r[cols["n"]]))}
            except ValueError:
                continue
            has_flux = "captures" in cols and "escapes" in cols and not derive
            if has_flux:
                try:
                    rec["cap"] = int(float(r[cols["captures"]]))
                    rec["esc"] = int(float(r[cols["escapes"]]))
                except ValueError:
                    has_flux = False
            rec["_has_flux"] = has_flux
            recs.append(rec)
    recs.sort(key=lambda r: (r["run"], r["island"], r["frame"]))
    # first pass: dN either from the logged fluxes or from consecutive N
    for rec in recs:
        rec["dN"] = None
    prev = {}
    for rec in recs:
        key = (rec["run"], rec["island"])
        if key in prev and prev[key]["frame"] < rec["frame"]:
            rec["dN_from_N"] = rec["N"] - prev[key]["N"]
        else:
            rec["dN_from_N"] = None
        prev[key] = rec
    for rec in recs:
        if rec["_has_flux"]:
            rec["dN"] = rec["cap"] - rec["esc"]
        else:
            rec["dN"] = rec["dN_from_N"]
            rec["cap"] = max(rec["dN"], 0) if rec["dN"] is not None else 0
            rec["esc"] = max(-rec["dN"], 0) if rec["dN"] is not None else 0
    return recs


def islands(recs):
    out = {}
    for rec in recs:
        out.setdefault((rec["run"], rec["island"]), []).append(rec)
    return out


# ------------------------------------------------------------------ estimators

def ols(xs, ys):
    """Slope and intercept of y = a + b x (standard, no numpy)."""
    n = len(xs)
    if n < 3:
        return None
    mx, my = sum(xs) / n, sum(ys) / n
    sxx = sum((x - mx) ** 2 for x in xs)
    if sxx == 0:
        return None
    sxy = sum((x - mx) * (y - my) for x, y in zip(xs, ys))
    b = sxy / sxx
    return {"a": my - b * mx, "b": b, "n": n}


def fit_drift(recs):
    pts = [(r["N"], r["dN"]) for r in recs if r["dN"] is not None]
    if len(pts) < 3:
        return None
    f = ols([p[0] for p in pts], [p[1] for p in pts])
    if f is None:
        return None
    f["nstar"] = (-f["a"] / f["b"]) if f["b"] < 0 else float("nan")
    return f


def ar1(recs):
    """Pooled lag-1 correlation of the residuals, computed per island."""
    xs, ys = [], []
    for _, rows in islands(recs).items():
        rows = [r for r in rows if r["dN"] is not None]
        if len(rows) < 3:
            continue
        f = fit_drift(rows)
        if f is None:
            continue
        e = [r["dN"] - (f["a"] + f["b"] * r["N"]) for r in rows]
        for i in range(len(e) - 1):
            xs.append(e[i])
            ys.append(e[i + 1])
    if len(xs) < 3:
        return float("nan")
    mx, my = sum(xs) / len(xs), sum(ys) / len(ys)
    sxx = sum((x - mx) ** 2 for x in xs)
    syy = sum((y - my) ** 2 for y in ys)
    if sxx == 0 or syy == 0:
        return float("nan")
    return sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / math.sqrt(sxx * syy)


def percentile(vals, q):
    if not vals:
        return float("nan")
    s = sorted(vals)
    pos = (len(s) - 1) * q
    lo, hi = int(math.floor(pos)), int(math.ceil(pos))
    if lo == hi:
        return s[lo]
    return s[lo] + (s[hi] - s[lo]) * (pos - lo)


# ------------------------------------------------------------------ 1. sanity

def sec_sanity(recs):
    L = ["== 1. Data sanity =="]
    runs = {}
    for rec in recs:
        runs.setdefault(rec["run"], []).append(rec)
    for run, rows in runs.items():
        per_isl = islands(rows)
        counts = [len(v) for v in per_isl.values()]
        cap = sum(r["cap"] for r in rows)
        esc = sum(r["esc"] for r in rows)
        zero_esc = sum(1 for v in per_isl.values()
                       if sum(r["esc"] for r in v) == 0)
        L.append("run %-26s islands=%d  frames/island=%d..%d  sum(cap)=%d "
                 "sum(esc)=%d  islands with zero escapes=%d"
                 % (run, len(per_isl), min(counts), max(counts), cap, esc,
                    zero_esc))
        if esc == 0:
            L.append("  WARNING: zero escapes in this run -- affiliation is a "
                     "fixed partition here, so dN = 0 and no drift can be "
                     "estimated (the basic32_seed_1 failure mode).")
        if not rows[0]["_has_flux"]:
            L.append("  NOTE: fluxes derived from consecutive N (turnover is "
                     "not distinguishable from stasis).")
    return L


# ------------------------------------------------------------------ 2. bins

def sec_binned(recs, nbins):
    L = ["== 2. Binned fluxes =="]
    pts = [r for r in recs if r["dN"] is not None]
    if not pts:
        return L + ["no usable rows"]
    nmin = min(r["N"] for r in pts)
    nmax = max(r["N"] for r in pts)
    width = (nmax - nmin) / nbins if nmax > nmin else 1.0
    L.append("N range %d..%d in %d bins of width %.2f"
             % (nmin, nmax, nbins, width))
    L.append("%-14s %-7s %-8s %-8s %-8s" % ("bin", "count", "meanN", "cap", "esc"))
    for i in range(nbins):
        lo, hi = nmin + i * width, nmin + (i + 1) * width
        sel = [r for r in pts if (lo <= r["N"] < hi) or
               (i == nbins - 1 and r["N"] == hi)]
        if not sel:
            continue
        L.append("%-14s %-7d %-8.2f %-8.3f %-8.3f"
                 % ("[%.1f,%.1f)" % (lo, hi), len(sel),
                    sum(r["N"] for r in sel) / len(sel),
                    sum(r["cap"] for r in sel) / len(sel),
                    sum(r["esc"] for r in sel) / len(sel)))
    curve = [(b, sum(r["dN"] for r in pts if r["N"] == b) /
              max(1, sum(1 for r in pts if r["N"] == b)))
             for b in sorted({r["N"] for r in pts})]
    L.append("mean dN by population: " +
             " ".join("N=%d:%.2f" % (b, d) for b, d in curve[:15]))
    L.append("an attractor requires this curve to cross zero with the drift "
             "going + -> - (b < 0).")
    return L


# ------------------------------------------------------------------ 3. fit

def sec_fit(recs, nboot, seed):
    L = ["== 3. Drift fit dN ~ a + b N =="]
    base = fit_drift(recs)
    if base is None:
        return L + ["not enough usable rows"], None
    L.append("a=%.5f  b=%.5f  over %d island-frames"
             % (base["a"], base["b"], base["n"]))
    if base["b"] < 0:
        L.append("N* = -a/b = %.2f  (attractor iff b < 0: OK)" % base["nstar"])
    else:
        L.append("b >= 0: no restoring drift, so no attractor N*.")
    r = ar1(recs)
    L.append("AR(1) of residuals = %.3f%s"
             % (r, "  (|r| > 0.3: autocorrelated, use the CI with care)"
                if abs(r) > 0.3 else ""))
    if base["b"] < 0 and nboot > 0:
        rng = random.Random(seed)
        per_isl = islands(recs)
        keys = list(per_isl.keys())
        stars = []
        for _ in range(nboot):
            pool = []
            for _k in keys:
                pool += per_isl[rng.choice(keys)]
            f = fit_drift(pool)
            if f is not None and f["b"] < 0:
                stars.append(f["nstar"])
        if stars:
            L.append("bootstrap over islands: N* 95%% CI [%.2f, %.2f] "
                     "(%d/%d usable resamples)"
                     % (percentile(stars, 0.025), percentile(stars, 0.975),
                        len(stars), nboot))
        else:
            L.append("bootstrap produced no usable N* (b >= 0 in resamples)")
    return L, base


# ------------------------------------------------------------------ 4. surrogates

def sec_surrogate(recs, base, nsurr, seed):
    L = ["== 4. Surrogate test (permute dN within island) =="]
    if base is None or base["b"] >= 0:
        return L + ["skipped (no attractor in the real data)"], float("nan")
    rng = random.Random(seed)
    obs = abs(base["b"])
    per_isl = islands(recs)
    count = 0
    for _ in range(nsurr):
        pool = []
        for r_rows in per_isl.values():
            vals = [r["dN"] for r in r_rows if r["dN"] is not None]
            perm = list(vals)
            rng.shuffle(perm)
            j = 0
            for r in r_rows:
                if r["dN"] is None:
                    continue
                pool.append({"run": r["run"], "island": r["island"],
                             "frame": r["frame"], "N": r["N"], "dN": perm[j]})
                j += 1
        f = fit_drift(pool)
        if f is not None and abs(f["b"]) >= obs:
            count += 1
    p = (count + 1) / (nsurr + 1)
    L.append("p(|b| surrogate >= observed) = %.4f over %d shuffles"
             % (p, nsurr))
    L.append("small p -> the drift is real dynamics, not a regression-to-the-"
             "mean artifact of the N distribution.")
    return L, p


# ------------------------------------------------------------------ 5. verdict

def verdict(recs, base, p):
    L = ["== 5. Verdict =="]
    if sum(r["esc"] for r in recs) == 0:
        L.append("INCONCLUSIVE: zero escapes -- affiliation is a fixed "
                 "partition in these runs. Re-run with escape-capable dynamics "
                 "(and per-event flux logging) before any attractor claim.")
    elif base is None or base["b"] >= 0:
        L.append("FAIL: no restoring drift (b >= 0). The measured fluxes do not "
                 "support a population attractor N*.")
    elif not math.isnan(p) and p >= 0.05:
        L.append("INCONCLUSIVE: b < 0 but surrogate p=%.3f; the apparent "
                 "restoring force is consistent with a sampling artifact. "
                 "More frames per island needed." % p)
    else:
        L.append("PASS: restoring drift b=%.4g < 0, N*=%.2f, surrogate p=%.4g. "
                 "Consistent with dynamic charge quantization; next step is the "
                 "EXCLUSION on/off ablation on the same harness."
                 % (base["b"], base["nstar"], p))
    return L


# ------------------------------------------------------------------ per run

def sec_by_run(recs):
    """N* per run: the direct link to the quantisation question (N* vs L)."""
    L = ["== 6. Per-run fits (N* vs lattice size) =="]
    L.append("%-26s %-8s %-9s %-10s %s" % ("run", "islands", "rows", "b", "N*"))
    runs = {}
    for rec in recs:
        runs.setdefault(rec["run"], []).append(rec)
    for run, rows in runs.items():
        f = fit_drift(rows)
        per_isl = islands(rows)
        if f is None:
            L.append("%-26s %-8d %-9d %-10s %s"
                     % (run, len(per_isl), len(rows), "-", "not enough rows"))
            continue
        nstar = ("%.2f" % f["nstar"]) if f["b"] < 0 else "none (b >= 0)"
        L.append("%-26s %-8d %-9d %-10.4f %s"
                 % (run, len(per_isl), len(rows), f["b"], nstar))
    L.append("A quantisation claim of the form N* = L/3 or N* = c*L^2 requires "
             "these N* values to track the lattice side, which is why the "
             "manuscript's runs at two sizes (L=9, L=15) are both listed.")
    return L


def main():
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("csv", nargs="+", help="flux CSV files")
    ap.add_argument("--bins", type=int, default=12)
    ap.add_argument("--derive-fluxes", action="store_true",
                    help="infer cap/esc from dN (last resort)")
    ap.add_argument("--surrogates", type=int, default=200)
    ap.add_argument("--boot", type=int, default=500)
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--by-run", action="store_true",
                    help="also fit each run separately (N* vs L)")
    ap.add_argument("--out", help="also write the report to this file")
    args = ap.parse_args()

    recs = load(args.csv, args.derive_fluxes)
    lines = []
    lines += sec_sanity(recs) + [""]
    lines += sec_binned(recs, args.bins) + [""]
    fit_sec, base = sec_fit(recs, args.boot, args.seed)
    lines += fit_sec + [""]
    surr_sec, p = sec_surrogate(recs, base, args.surrogates, args.seed)
    lines += surr_sec + [""]
    lines += verdict(recs, base, p)
    if args.by_run:
        lines += [""] + sec_by_run(recs)
    text = "\n".join(lines)
    print(text)
    if args.out:
        with open(args.out, "w") as fh:
            fh.write(text + "\n")


if __name__ == "__main__":
    main()

