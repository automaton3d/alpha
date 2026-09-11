"""Cross-check production census, constituent populations, and flow closure."""
import csv
import sys
from pathlib import Path

root = Path(sys.argv[1])
def rows(name):
    with (root / name).open() as f:
        return [{k: int(v) for k, v in r.items()} for r in csv.DictReader(f)]

reference = {r['frame']: r for r in rows('census.csv')}
chiefs = {r['frame']: r for r in rows('chief_census.csv')}
groups = {}
for r in rows('groups.csv'):
    if r['valid_chief']:
        groups.setdefault(r['frame'], {})[r['chief']] = r['population']
previous = groups.get(0, {})
constituents = {}
for r in rows('constituents.csv'):
    constituents.setdefault(r['frame'], {})[r['chief_w']] = r
flux = {r['frame']: r for r in rows('sector_flux.csv')}
expected_frames = set(range(1, max(reference) + 1))
assert set(chiefs) == set(constituents) == set(flux) == expected_frames
for frame in sorted(expected_frames):
    ref, new = reference[frame], chiefs[frame]
    for key in ('K', 'D', 'S', 'unresolved'):
        assert ref[key] == new[key], (frame, key)
    assert ref['P'] == new['P_halves']
    assert ref['distinct_centers'] == new['occupied_centers']
    populations = groups.get(frame, {})
    assert len(populations) == new['K']
    captures = escapes = 0
    for w, r in constituents[frame].items():
        assert r['constituents'] == populations.get(w, 0), (frame, w)
        assert r['constituents'] - previous.get(w, 0) == r['captures'] - r['escapes'], (frame, w)
        captures += r['captures']
        escapes += r['escapes']
    assert captures == ref['captures'] == sum(v for k, v in flux[frame].items() if k.startswith('cap'))
    assert escapes == ref['escapes'] == sum(v for k, v in flux[frame].items() if k.startswith('esc'))
    previous = populations
print(f'PASS {len(expected_frames)} frames: live-chief populations, census agreement, per-chief and aggregate flow closure.')
