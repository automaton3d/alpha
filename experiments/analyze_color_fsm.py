"""Read-only analysis of stage samples; standard library only."""
import csv
import json
import math
import argparse
from collections import Counter, defaultdict
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--directory", type=Path,
                    default=Path(__file__).resolve().parent.parent / "build" / "color_fsm")
root = parser.parse_args().directory
samples = defaultdict(list)
with (root / "trajectories.csv").open() as f:
    for row in csv.DictReader(f):
        r = {k: int(v) for k, v in row.items()}
        if r["k"] == 0:
            samples[r["frame"]].append(r)
contacts = defaultdict(lambda: [0, 0])
with (root / "contacts.csv").open() as f:
    for r in csv.DictReader(f):
        # Contacts in journey n are consumed at boundary n+1.
        c = contacts[int(r["frame"]) + 1]
        c[0] += 1
        c[1] += int(r["reciprocal"]) == 0
seen = {}
previous = None
results = []
for frame, rows in sorted(samples.items()):
    rows.sort(key=lambda r: r["w"])
    if len(rows) != 243 or [r["w"] for r in rows] != list(range(243)):
        raise ValueError(f"incomplete boundary sample {frame}")
    positions = [(r["x"], r["y"], r["z"]) for r in rows]
    # Fixed color direction confines completed displacements to a body
    # diagonal; intermediate axis-by-axis translation need not lie on it.
    for p, r in zip(positions, rows):
        color = (r["w"] // 3) % 8
        parameters = {(p[j]-4)*(1-2*((color >> j) & 1)) % 9 for j in range(3)}
        if len(parameters) != 1:
            raise ValueError(f"color diagonal invariant broken frame={frame} w={r['w']}")
    state = tuple((*p, r["t"]) for p, r in zip(positions, rows))
    first_seen = seen.setdefault(state, frame)
    count = Counter(positions)
    square_distance = 0
    for i, a in enumerate(positions):
        for b in positions[:i]:
            square_distance += sum(min(abs(x-y), 9-abs(x-y))**2 for x, y in zip(a, b))
    movers = 0
    if previous is not None:
        for old, new in zip(previous, rows):
            movers += any(old[a] != new[a] for a in ("x", "y", "z"))
            for axis, impulse in zip(("x", "y", "z"), ("dcx", "dcy", "dcz")):
                if new[axis] != (old[axis] + new[impulse]) % 9:
                    raise ValueError(f"displacement mismatch frame={frame} w={new['w']}")
    results.append(dict(frame=frame, distinct=len(count), moved_sources=movers,
        coincident_pairs=sum(n*(n-1)//2 for n in count.values()),
        rms_pair_distance=round(math.sqrt(square_distance/(243*242/2)), 6),
        reemitted=sum(r["reemitted"] for r in rows),
        phase_counts=dict(sorted(Counter(r["t"] for r in rows).items())),
        signed_sum=[sum(r[a] for r in rows) for a in ("dcx", "dcy", "dcz")],
        directed_contacts=contacts[frame][0], nonreciprocal=contacts[frame][1],
        repeated_position_phase_frame=None if first_seen == frame else first_seen))
    previous = rows
output = dict(scope="L=9; boundary positions/phases, not full-state recurrence or equilibrium",
              frames=results)
(root / "analysis.json").write_text(json.dumps(output, indent=2) + "\n")
for r in results:
    print(f"frame={r['frame']} centers={r['distinct']} movers={r['moved_sources']} "
          f"contacts={r['directed_contacts']} nonreciprocal={r['nonreciprocal']} "
          f"net={r['signed_sum']} repeat={r['repeated_position_phase_frame']}")
