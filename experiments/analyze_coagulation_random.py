"""Independent audit of the temporary forcing, its timing, and actual relocation."""
import argparse
import csv
import json
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("seed", type=int, nargs="?", default=1)
parser.add_argument("--directory", type=Path)
args = parser.parse_args()
seed = args.seed
root = args.directory or (Path(__file__).resolve().parent.parent / "build" / "coagulation_random" / f"seed_{seed}")

def rows(name):
    with (root / name).open() as f:
        return [{k: int(v) for k, v in r.items()} for r in csv.DictReader(f)]

members = {}
for r in rows("members.csv"):
    members.setdefault(r["frame"], {})[r["w"]] = r
if sorted(members) != list(range(13)) or any(sorted(m) != list(range(243)) for m in members.values()):
    raise ValueError("expected 12 complete journeys and an initial sample")
events = rows("interventions.csv")
recorded = {(r["journey"], r["w"]): r for r in events}
if len(recorded) != len(events):
    raise ValueError("duplicate intervention")
mask = (1 << 64) - 1
state = seed & mask

def draw(bound):
    global state
    threshold = ((1 << 64) - bound) % bound
    while True:
        state = (state + 0x9e3779b97f4a7c15) & mask
        z = state
        z = ((z ^ (z >> 30)) * 0xbf58476d1ce4e5b9) & mask
        z = ((z ^ (z >> 27)) * 0x94d049bb133111eb) & mask
        z ^= z >> 31
        if z >= threshold:
            return z % bound

witness = [243] * 243
streak = [0] * 243
expected_events = set()
for frame in range(12):
    m = members[frame]
    positions = [tuple(m[w][axis] for axis in ("x", "y", "z")) for w in range(243)]
    for w in range(243):
        peer = next((q for q in range(243) if q != w and positions[q] == positions[w]), 243)
        streak[w] = (streak[w] + 1 if witness[w] == peer else 1) if peer < 243 else 0
        witness[w] = peer
        if not (4 <= frame < 8 and streak[w] >= 3):
            continue
        key = frame + 1, w
        expected_events.add(key)
        r = recorded[key]
        force = [0, 0, 0]
        while not any(force):
            force = [draw(9) for _ in range(3)]
        if r["witness"] != peer or r["streak"] != streak[w]:
            raise ValueError("invalid persistent-overlap selection")
        if force != [r[k] for k in ("forced_cx", "forced_cy", "forced_cz")]:
            raise ValueError("random sequence is not reproducible")
        for axis, delta in zip(("x", "y", "z"), force):
            if members[frame+1][w][axis] != (m[w][axis] + delta) % 9:
                raise ValueError("reemission destination disagrees with forced c")
        for field in ("charge", "affinity"):
            if members[frame+1][w][field] != m[w][field]:
                raise ValueError("forcing changed source identity")
        for field in ("kind", "parent"):
            if members[frame+1][w][field] != r[field]:
                raise ValueError("forcing changed the post-election role")
if expected_events != set(recorded):
    raise ValueError("unexpected intervention timing or target")
trace = rows("trace.csv")
for r in trace:
    actual = sum(e["journey"] == r["frame"] for e in events)
    if actual != r["interventions"]:
        raise ValueError("intervention count mismatch")
result = dict(seed=seed, interventions=len(events), expected_islands=81,
              final=trace[-1], frames=trace,
              interpretation="Random reemission changes positions, not chief identities.")
(root / "analysis.json").write_text(json.dumps(result, indent=2) + "\n")
for r in trace:
    print(r)
print("PASS timing, persistence selection, reproducible randomness, actual displacement, and preserved identities.")
