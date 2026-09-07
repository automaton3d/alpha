"""Read-only matching audit for the canonical L=9 run; standard library only."""
import csv
import json
from collections import defaultdict
from pathlib import Path

root = Path(__file__).resolve().parent.parent / "build" / "color_matched"
contacts = defaultdict(dict)
for row in csv.DictReader((root / "contacts.csv").open()):
    r = {k: int(v) for k, v in row.items()}
    contacts[r["frame"] + 1][r["a"], r["b"]] = tuple(r[k] for k in ("dx", "dy", "dz"))
groups = defaultdict(list)
for row in csv.DictReader((root / "groups.csv").open()):
    r = {k: int(v) for k, v in row.items()}
    groups[r["frame"]].append(r)
paths = defaultdict(dict)
for row in csv.DictReader((root / "trajectories.csv").open()):
    r = {k: int(v) for k, v in row.items()}
    if r["k"] == 0:
        paths[r["frame"]][r["w"]] = tuple(r[k] for k in ("x", "y", "z"))

results = []
for frame, positions in sorted(paths.items()):
    if len(positions) != 243:
        raise ValueError("incomplete frame")
    partners = defaultdict(set)
    net = [0, 0, 0]
    for (a, b), step in contacts[frame].items():
        if any(step):
            partners[a].add(b)
        if contacts[frame].get((b, a)) != tuple(-v for v in step):
            raise ValueError("nonreciprocal contact impulse")
        if any(abs(v) > 1 for v in step):
            raise ValueError("unbounded impulse")
        net = [x + y for x, y in zip(net, step)]
    if any(len(p) > 1 for p in partners.values()) or any(net):
        raise ValueError("invalid matching")
    chiefs = groups[frame]
    result = dict(frame=frame, centers=len(set(positions.values())),
                  chief_centers=len({positions[g["chief"]] for g in chiefs}),
                  chiefs=len(chiefs), moving_sources=len(partners),
                  max_distance_to_chief=max((g["max_distance_to_chief"] for g in chiefs), default=0),
                  max_group_sites=max((g["distinct_centers"] for g in chiefs), default=0),
                  unresolved_chief_groups=sum(not g["valid_chief"] for g in chiefs), net=net)
    results.append(result)
    print(result)
(root / "matching_analysis.json").write_text(json.dumps(results, indent=2) + "\n")
