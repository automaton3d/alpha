"""Audit unwrapped axial trajectories; a torus crossing must not hide separation."""
import csv
import json
from collections import defaultdict
from pathlib import Path

root = Path(__file__).resolve().parent.parent / "build" / "inertia_matched"
cases = ("rest", "right", "left", "island", "island_left", "foreign", "broken", "zero", "many")
all_paths = {}
report = {}
for name in cases:
    n = 1 if name in ("right", "left") else 3
    frames = defaultdict(dict)
    for row in csv.DictReader((root / (name + ".csv")).open()):
        r = {k: int(v) for k, v in row.items()}
        frames[r["frame"]][r["w"]] = r
    if sorted(frames) != list(range(1, 97)):
        raise ValueError(f"{name}: expected 96 complete frame samples")
    width = len(frames[1])
    body_diameter = propeller_gap = 0
    for frame, members in frames.items():
        if sorted(members) != list(range(width)):
            raise ValueError("missing W source")
        for w, r in members.items():
            old = frames[frame-1][w]["unwrapped_x"] if frame > 1 else 4
            if r["unwrapped_x"] != old + r["impulse_x"] or abs(r["impulse_x"]) > 1:
                raise ValueError("incorrect signed transport")
            if r["unwrapped_x"] % 9 != r["x"] or (r["y"], r["z"]) != (4, 4):
                raise ValueError("incorrect axial trajectory")
        body = [members[w]["unwrapped_x"] for w in range(n)]
        body_diameter = max(body_diameter, max(body)-min(body))
        for p in range(n, width, 2):
            x = members[p]["unwrapped_x"]
            if x != members[p+1]["unwrapped_x"]:
                raise ValueError("pair halves separated")
            propeller_gap = max(propeller_gap, min(abs(x-b) for b in body))
    displacement = sum(frames[96][w]["unwrapped_x"]-4 for w in range(n))/n
    if name in ("rest", "foreign", "broken", "zero") and displacement != 0:
        raise ValueError("negative control drift")
    report[name] = dict(displacement=displacement, mean_cells_per_frame=displacement/96,
                       max_unwrapped_body_diameter=body_diameter,
                       max_unwrapped_pair_to_body_distance=propeller_gap,
                       body_spans_torus=body_diameter >= 9)
    all_paths[name] = frames
for right, left in (("right", "left"), ("island", "island_left")):
    for frame, members in all_paths[right].items():
        for w, r in members.items():
            if r["unwrapped_x"] + all_paths[left][frame][w]["unwrapped_x"] != 8:
                raise ValueError("direction reversal is not an exact spatial reflection")
(root / "analysis.json").write_text(json.dumps(report, indent=2) + "\n")
for name, result in report.items():
    print(name, result)
print("PASS transport, pair integrity, controls, and exact direction reversal; localization remains an outcome.")
