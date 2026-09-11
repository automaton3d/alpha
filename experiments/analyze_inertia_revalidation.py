"""Audit diagnostic traces and report physical failures without hiding them."""
import csv
import json
from pathlib import Path

base = Path('build/inertia_revalidation')
reports = []
def distance(a, b):
    return sum(abs(a[k] - b[k]) for k in ('ux', 'uy', 'uz'))

for variant in ('production', 'matched'):
    root = base / variant
    execution = json.loads((root / 'execution.json').read_text())
    for run in execution:
        if run['suite'] != 'diagnostic':
            continue
        case = run['case']
        with (root / (case + '.summary.csv')).open() as f:
            s = next(csv.DictReader(f))
        n, frames, burn, wcount = (int(s[k]) for k in ('N', 'frames', 'burn', 'W'))
        states = {}
        with (root / (case + '.csv')).open() as f:
            for row in csv.DictReader(f):
                r = {k: int(v) for k, v in row.items()}
                assert r['w'] not in states.setdefault(r['frame'], {})
                states[r['frame']][r['w']] = r
        assert sorted(states) == list(range(frames + 1)), (variant, case, 'incomplete trace')
        assert all(sorted(f) == list(range(wcount)) for f in states.values())
        velocities = [[sum(states[t][w][k] - states[t-1][w][k] for w in range(n)) / n
                       for k in ('ux', 'uy', 'uz')] for t in range(1, frames + 1)]
        mean = [sum(v[a] for v in velocities[burn:]) / (frames-burn) for a in range(3)]
        assert all(abs(mean[a] - float(s[k])) < 1e-9 for a, k in enumerate(('vx','vy','vz')))
        span = max(distance(f[a], f[b]) for t, f in states.items() if t for a in range(n) for b in range(n))
        gap = max((min(distance(f[p], f[b]) for b in range(n)) for t, f in states.items() if t
                   for p in range(n, wcount, 2)), default=0)
        assert span == int(s['max_span']) and gap == int(s['max_gap'])
        split = unresolved = changed = 0
        for t in range(burn+1, frames+1):
            f = states[t]
            identities = [w if f[w]['kind'] == 0 else f[w]['parent'] if f[w]['kind'] == 2 else wcount for w in range(n)]
            valid = [p for p in identities if p in f and f[p]['kind'] == 0]
            split += len(set(valid)) != 1
            unresolved += len(valid) != n
            changed += any((f[w]['kind'], f[w]['parent']) != (states[0][w]['kind'], states[0][w]['parent']) for w in range(n))
        assert (split, unresolved, changed) == tuple(int(s[k]) for k in ('split_frames','unresolved_frames','changed_role_frames'))
        problems = []
        if run['exit'] or int(s['errors']): problems.append('integrity')
        # Production promises one face-step; matched permits one per axis.
        # This diagnostic bounds their sum; the original matched harness also
        # checks each axis individually until its first identity failure.
        if int(s['max_steps']) > (1 if variant == 'production' else 3):
            problems.append('journey movement budget exceeded')
        if split or unresolved: problems.append('not one live-chief island')
        if span > 3: problems.append('body span exceeds criterion')
        # Same bounded-neighborhood acceptance used by the original tube fixture.
        radius = 4  # ordinary runs here have short edge 9 or 5; stricter below for tube
        if variant == 'production' and not case.startswith('large_'): radius = 2
        if gap > 2 * radius: problems.append('drive pair departs')
        if case in ('rest','cohesion','election','foreign','broken','zero','balanced'):
            if any(abs(v) > 1e-9 for v in mean): problems.append('control drifts')
        elif case == 'lifecycle':
            phases = [sum(v[0] for v in velocities[a:b]) for a,b in ((0,24),(24,72),(72,96))]
            if not (abs(phases[0]) < 1e-9 and phases[1] > 0 and abs(phases[2]) < 1e-9):
                problems.append('affiliation lifecycle')
        else:
            axis = 1 if case == 'up' else 0
            direction = -1 if case in ('left','island_left') else 1
            if direction * mean[axis] <= 0: problems.append('missing directed transport')
            midpoint = burn + (frames-burn)//2
            for a,b in ((burn,midpoint),(midpoint,frames)):
                if direction * sum(v[axis] for v in velocities[a:b]) <= 0: problems.append('transport not sustained in both windows'); break
            if case == 'oblique' and (mean[1] <= 0 or abs(mean[0]-2*mean[1]) > 1e-9):
                problems.append('oblique ratio')
        reports.append(dict(variant=variant,case=case,frames=frames,N=n,pairs=int(s['pairs']),W=wcount,
                            vx=mean[0],vy=mean[1],vz=mean[2],span=span,gap=gap,split_frames=split,
                            unresolved_frames=unresolved,changed_role_frames=changed,problems=problems))
    selected = {r['case']:r for r in reports if r['variant']==variant}
    for left, right in (('left','right'),('island_left','island')):
        a,b=selected[left],selected[right]
        if abs(a['vx']+b['vx'])>1e-9:
            a['problems'].append('reflection asymmetry');b['problems'].append('reflection asymmetry')
    if selected['many']['vx'] <= selected['island']['vx']:
        selected['many']['problems'].append('no population-speed increase')

(base/'audit.json').write_text(json.dumps(reports,indent=2)+'\n')
for r in reports:
    print(f"{r['variant']:10} {r['case']:12} v=({r['vx']:+.6f},{r['vy']:+.6f}) span={r['span']:3} gap={r['gap']:3} "
          + ('ACCEPT' if not r['problems'] else 'FAIL: '+', '.join(r['problems'])))
print('Trace consistency audited. Physical acceptance is limited to the tested geometry and duration.')
raise SystemExit(1 if any(r['problems'] for r in reports) else 0)
