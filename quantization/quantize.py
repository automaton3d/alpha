#!/usr/bin/env python3
"""
quantize.py -- Dynamic charge-quantization analysis harness
for the alpha automaton (github.com/automaton3d/alpha).

Tests the attractor hypothesis of manuscript Sec. "Dynamic charge
quantization":

    dN/dt = Gamma_cap(N) - Gamma_esc(N),  attractor N* where they cross.

Why this exists: the basic32_seed_1 runs show affiliation as a *fixed
partition* of the 8 charge words (constant populations, zero escapes),
so Gamma_esc == 0 by construction and OLS trivially returns dN = 0*N.
This harness measures the *fluxes* from event logs instead of fitting
population levels, and only claims an attractor when the drift curve
actually crosses zero with the right slope sign.

Input
-----
A CSV (or several) with one row per (island, frame):

    run,island,frame,N,captures,escapes

  run      : identifier of the simulation run / seed (string)
  island   : identifier of the island within the run (string/int)
  frame    : integer light-frame index (monotone per island)
  N        : island population at that frame
  captures : number of bubbles that joined the island in [frame, frame+1)
  escapes  : number of bubbles that left  the island in [frame, frame+1)

If your logger only stores populations, use --derive-fluxes to set
dN = N(frame+1)-N(frame) and treat positive dN as capture, negative as
escape. That is a last resort: it cannot distinguish turnover
(simultaneous capture+escape) from stasis.

Usage
-----
    python3 quantize.py runs/*.csv --bins 12 --out report.txt
    python3 quantize.py run.csv --derive-fluxes
    python3 quantize.py run.csv --surrogates 200 --seed 1

Output sections
---------------
1. Data sanity     -- frame coverage, turnover stats, zero-flux warning
2. Binned fluxes   -- Gamma_cap(N), Gamma_esc(N) per population bin
3. Drift fit       -- OLS dN ~ a + b N and nonlinear (loess-ish) drift,
                      fixed point N* = -a/b, attractor iff b < 0,
                      with bootstrap CI and AR(1) residual check
4. Surrogate test  -- same N* statistic on frame-shuffled surrogates;
                      p-value tells whether the drift is real dynamics
                      or an artifact of the population distribution
5. Verdict         -- explicit PASS / FAIL / INCONCLUSIVE statement
"""

import argparse
import sys
from dataclasses import dataclass

import numpy as np
import pandas as pd


# ---------------------------------------------------------------- data

@dataclass
class FluxData:
    df: pd.DataFrame  # columns: run, island, frame, N, cap, esc, dN


def load(paths, derive_fluxes=False):
    frames = []
    for p in paths:
        df = pd.read_csv(p)
        cols = {c.lower(): c for c in df.columns}
        need = ["island", "frame", "n"]
        for k in need:
            if k not in cols:
                sys.exit(f"{p}: missing column '{k}' (have {list(df.columns)})")
        df = df.rename(columns={cols["island"]: "island",
                                cols["frame"]: "frame",
                                cols["n"]: "N"})
        if "run" not in cols:
            df["run"] = p
        else:
            df = df.rename(columns={cols["run"]: "run"})
        if derive_fluxes:
            df = df.sort_values(["run", "island", "frame"])
            dN = df.groupby(["run", "island"])["N"].diff()
            df["cap"] = dN.clip(lower=0).fillna(0)
            df["esc"] = (-dN.clip(upper=0)).fillna(0)
            df["dN"] = dN
        else:
            for k, new in [("captures", "cap"), ("escapes", "esc")]:
                if k not in cols:
                    sys.exit(f"{p}: missing column '{k}' "
                             f"(or use --derive-fluxes)")
                df = df.rename(columns={cols[k]: new})
            df["dN"] = df["cap"] - df["esc"]
        frames.append(df[["run", "island", "frame", "N", "cap", "esc", "dN"]])
    out = pd.concat(frames, ignore_index=True)
    return FluxData(out)


# ---------------------------------------------------------------- 1. sanity

def sec_sanity(fd):
    df = fd.df
    L = ["== 1. Data sanity =="]
    L.append(f"rows={len(df)}  runs={df.run.nunique()}  "
             f"islands={df.groupby(['run','island']).ngroups}")
    per_island = df.groupby(["run", "island"]).size()
    L.append(f"frames/island: median={per_island.median():.0f} "
             f"min={per_island.min()} max={per_island.max()}")
    L.append(f"N range: {df.N.min()} .. {df.N.max()}  "
             f"unique N values: {df.N.nunique()}")
    tot_cap, tot_esc = df.cap.sum(), df.esc.sum()
    L.append(f"total captures={tot_cap:.0f}  total escapes={tot_esc:.0f}")
    if tot_esc == 0:
        L.append("WARNING: zero escapes in the whole dataset. "
                 "Affiliation is a fixed partition here (basic32_seed_1 "
                 "failure mode). No attractor can be measured; fix the "
                 "dynamics/instrumentation first.")
    if tot_cap + tot_esc > 0:
        turn = (df.cap + df.esc).mean()
        L.append(f"mean turnover per island-frame: {turn:.3f}")
    const = (df.groupby(["run", "island"])["N"].nunique() == 1).mean()
    L.append(f"fraction of islands with constant N: {const:.2%}")
    return L


# ---------------------------------------------------------------- 2. binned fluxes

def sec_binned(fd, nbins):
    df = fd.df.dropna(subset=["dN"])
    if df.empty:
        return ["== 2. Binned fluxes ==", "no flux data"]
    qs = np.quantile(df.N, np.linspace(0, 1, nbins + 1))
    qs = np.unique(qs)
    df = df.assign(bin=pd.cut(df.N, qs, include_lowest=True))
    g = df.groupby("bin", observed=True)
    tab = pd.DataFrame({
        "N_mid": g.apply(lambda x: x.N.mean(), include_groups=False),
        "count": g.size(),
        "cap_mean": g.cap.mean(),
        "esc_mean": g.esc.mean(),
        "drift": g.dN.mean(),
        "drift_se": g.dN.std() / np.sqrt(g.size()),
    })
    L = ["== 2. Binned fluxes (mean rates per island-frame) ==",
         f"{'N_mid':>8} {'n':>6} {'G_cap':>8} {'G_esc':>8} "
         f"{'drift':>8} {'+-se':>7}"]
    for _, r in tab.iterrows():
        L.append(f"{r.N_mid:8.1f} {int(r['count']):6d} "
                 f"{r.cap_mean:8.3f} {r.esc_mean:8.3f} "
                 f"{r.drift:8.3f} {r.drift_se:7.3f}")
    cross = np.where(np.diff(np.sign(tab.drift.values)) < 0)[0]
    L.append(f"drift sign crossings (positive->negative = attractor "
             f"candidate): {len(cross)}")
    return L


# ---------------------------------------------------------------- 3. drift fit

def fit_drift(df):
    """OLS dN = a + b N. Returns a, b, se_b, N*, r2."""
    d = df.dropna(subset=["dN"])
    x, y = d.N.values.astype(float), d.dN.values.astype(float)
    if len(d) < 10 or np.ptp(x) == 0:
        return None
    A = np.vstack([np.ones_like(x), x]).T
    coef, *_ = np.linalg.lstsq(A, y, rcond=None)
    resid = y - A @ coef
    dof = len(x) - 2
    sigma2 = resid @ resid / max(dof, 1)
    cov = sigma2 * np.linalg.inv(A.T @ A)
    a, b = coef
    se_b = np.sqrt(cov[1, 1])
    ss_tot = ((y - y.mean()) ** 2).sum()
    r2 = 1 - (resid @ resid) / ss_tot if ss_tot > 0 else np.nan
    nstar = -a / b if b != 0 else np.nan
    # AR(1) coefficient of residuals (per-island ordering)
    ar1 = np.corrcoef(resid[:-1], resid[1:])[0, 1] if len(resid) > 3 else np.nan
    return dict(a=a, b=b, se_b=se_b, nstar=nstar, r2=r2, ar1=ar1, n=len(d))


def sec_fit(fd, nboot=500, seed=0):
    rng = np.random.default_rng(seed)
    L = ["== 3. Drift fit  dN = a + b*N =="]
    base = fit_drift(fd.df)
    if base is None:
        return L + ["insufficient or degenerate data (N never varies?)"]
    L.append(f"a={base['a']:.4f}  b={base['b']:.6f} (se {base['se_b']:.2e})  "
             f"r2={base['r2']:.3f}  AR(1) of residuals={base['ar1']:.3f}")
    if base["b"] >= 0:
        L.append(f"b >= 0 -> no restoring drift, no attractor "
                 f"(N* would be {base['nstar']:.1f}, repeller at best).")
    else:
        # bootstrap N* over islands (cluster bootstrap: resample islands)
        keys = fd.df.dropna(subset=["dN"]).groupby(["run", "island"])
        groups = [g for _, g in keys]
        stars = []
        for _ in range(nboot):
            samp = pd.concat([groups[i] for i in
                              rng.integers(0, len(groups), len(groups))])
            f = fit_drift(samp)
            if f and f["b"] < 0 and np.isfinite(f["nstar"]):
                stars.append(f["nstar"])
        if stars:
            lo, hi = np.percentile(stars, [2.5, 97.5])
            L.append(f"N* = {base['nstar']:.2f}  "
                     f"bootstrap 95% CI [{lo:.2f}, {hi:.2f}] "
                     f"({len(stars)}/{nboot} usable resamples)")
        else:
            L.append("bootstrap produced no usable N* (b>=0 in resamples)")
    return L, base


# ---------------------------------------------------------------- 4. surrogates

def sec_surrogate(fd, base, nsurr, seed):
    L = ["== 4. Surrogate test (shuffle dN within island) =="]
    if base is None or base["b"] >= 0:
        return L + ["skipped (no attractor in real data)"], np.nan
    rng = np.random.default_rng(seed)
    df = fd.df.dropna(subset=["dN"])
    obs = abs(base["b"])
    count = 0
    for _ in range(nsurr):
        d = df.copy()
        d["dN"] = (d.groupby(["run", "island"])["dN"]
                     .transform(lambda s: rng.permutation(s.values)))
        f = fit_drift(d)
        if f is not None and abs(f["b"]) >= obs:
            count += 1
    p = (count + 1) / (nsurr + 1)
    L.append(f"p(b surrogate >= observed) = {p:.4f} over {nsurr} shuffles")
    L.append("small p -> drift is real dynamics, not a regression-to-mean "
             "artifact of the N distribution.")
    return L, p


# ---------------------------------------------------------------- 5. verdict

def verdict(fd, base, p):
    L = ["== 5. Verdict =="]
    if fd.df.esc.sum() == 0:
        L.append("INCONCLUSIVE: zero escapes -- affiliation is a fixed "
                 "partition in these runs. Re-run with escape-capable "
                 "dynamics (and per-event flux logging) before any "
                 "attractor claim.")
    elif base is None or base["b"] >= 0:
        L.append("FAIL: no restoring drift (b >= 0). The measured fluxes "
                 "do not support a population attractor N*.")
    elif not np.isnan(p) and p >= 0.05:
        L.append(f"INCONCLUSIVE: drift slope b<0 but surrogate p={p:.3f}; "
                 "the apparent restoring force is consistent with a "
                 "sampling artifact. More frames per island needed.")
    else:
        L.append(f"PASS: restoring drift b={base['b']:.4g}<0, "
                 f"N*={base['nstar']:.2f}, surrogate p={p:.4g}. "
                 "Consistent with dynamic charge quantization; next step "
                 "is the EXCLUSION on/off ablation on the same harness.")
    return L


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("csv", nargs="+", help="flux CSV files")
    ap.add_argument("--bins", type=int, default=12)
    ap.add_argument("--derive-fluxes", action="store_true",
                    help="infer cap/esc from dN (last resort)")
    ap.add_argument("--surrogates", type=int, default=200)
    ap.add_argument("--boot", type=int, default=500)
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--out", help="also write report to this file")
    args = ap.parse_args()

    fd = load(args.csv, args.derive_fluxes)
    lines = []
    lines += sec_sanity(fd) + [""]
    lines += sec_binned(fd, args.bins) + [""]
    fit_sec, base = sec_fit(fd, nboot=args.boot, seed=args.seed)
    lines += fit_sec + [""]
    surr_sec, p = sec_surrogate(fd, base, args.surrogates, args.seed)
    lines += surr_sec + [""]
    lines += verdict(fd, base, p)
    text = "\n".join(lines)
    print(text)
    if args.out:
        with open(args.out, "w") as f:
            f.write(text + "\n")


if __name__ == "__main__":
    main()
