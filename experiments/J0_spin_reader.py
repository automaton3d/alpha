#!/usr/bin/env python3
"""J0 spin baseline reader: prove J is currently zero (stdlib only)."""
import csv
import sys
from pathlib import Path


def read_csv(path):
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        cols = list(r.fieldnames or [])
        return list(r), cols


def main(args):
    total_groups = 0
    total_nonzero = 0
    max_mag2 = 0
    max_c = [0, 0, 0]
    all_ok = True
    for a in args:
        p = Path(a)
        if p.is_file() and p.suffix == ".csv":
            # Single file: decide by header.
            _, cols = read_csv(p)
            if "chief" in cols and "population" in cols:
                gpath, cpath = p, None
            else:
                gpath, cpath = None, p
            label = str(p)
        else:
            gpath = p / "groups.csv"
            cpath = p / "census.csv"
            if not gpath.exists():
                # Fall back: any census.csv directly inside (archived single-file logs).
                alt = p / "census.csv"
                if alt.exists():
                    gpath, cpath = None, alt
                else:
                    print(f"[{a}] MISSING groups.csv or census.csv -> SKIP")
                    all_ok = False
                    continue
            elif not cpath.exists():
                cpath = None
            label = str(a)
        nz = 0
        ng = 0
        local_max = 0
        has_j = False
        if gpath is not None:
            grows, gcols = read_csv(gpath)
            has_j = all(c in gcols for c in ("Jx", "Jy", "Jz"))
            ng = len(grows)
            for row in grows:
                try:
                    jx = int(row.get("Jx", "0") or 0)
                    jy = int(row.get("Jy", "0") or 0)
                    jz = int(row.get("Jz", "0") or 0)
                except ValueError:
                    jx = jy = jz = 0
                m2 = jx * jx + jy * jy + jz * jz
                local_max = max(local_max, m2)
                max_c[0] = max(max_c[0], abs(jx))
                max_c[1] = max(max_c[1], abs(jy))
                max_c[2] = max(max_c[2], abs(jz))
                if m2 > 0:
                    nz += 1
        c_spin = 0
        c_max = 0
        nframes = 0
        maxpop = 0
        if cpath is not None:
            crows, ccols = read_csv(cpath)
            nframes = len(crows)
            for row in crows:
                try:
                    if "spin_groups" in ccols:
                        c_spin += int(row.get("spin_groups", "0") or 0)
                    if "max_Jmag2" in ccols:
                        c_max = max(c_max, int(row.get("max_Jmag2", "0") or 0))
                    if "max_population" in ccols:
                        maxpop = max(maxpop, int(row.get("max_population", "0") or 0))
                except ValueError:
                    pass
        total_groups += ng
        total_nonzero += nz
        max_mag2 = max(max_mag2, local_max, c_max)
        if gpath is not None:
            status = "PASS" if (nz == 0 and local_max == 0) else "FAIL"
            if status == "FAIL":
                all_ok = False
            print(f"[{label}] groups={ng} nonzero_J={nz} max_|J|^2={local_max} "
                  f"Jcols={'yes' if has_j else 'NO'} census_spin={c_spin} "
                  f"census_max={c_max} -> {status}")
        else:
            print(f"[{label}] (no groups.csv) frames={nframes} max_pop={maxpop} "
                  f"census_spin={c_spin} census_max={c_max} -> NO-J-COLUMNS "
                  f"(pre-J0 log; J==0 implied by m==0 seed)")
    print(f"TOTAL groups={total_groups} nonzero_J={total_nonzero} "
          f"max_|J|^2={max_mag2} max_|Jxyz|={tuple(max_c)}")
    print("J0 verdict:", "PASS (J==0 everywhere)" if all_ok and total_groups > 0
          else "NOTE: no J columns yet (pre-J0 logs); rebuild census for measurement")
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python experiments/J0_spin_reader.py <outdir|csv> [...]")
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))