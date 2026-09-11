"""Run all cases even on failure; preserve historical logs and executables."""
import json
import subprocess
import sys
from pathlib import Path

variant = sys.argv[1]
assert variant in ('production', 'matched')
root = Path('build/inertia_revalidation') / variant
cases = ['rest', 'right', 'left', 'island', 'island_left', 'many',
         'balanced', 'foreign', 'broken', 'zero', 'up', 'oblique', 'election', 'lifecycle']
if variant == 'production':
    cases += ['cohesion', 'large_one', 'large_many', 'saturated']
original = [('suite', [])] if variant == 'production' else [
    (case, [case, '96']) for case in ('rest', 'right', 'left', 'island', 'island_left', 'foreign', 'broken', 'zero', 'many')]
results = []
for case, args in original:
    with (root / ('original_' + case + '.log')).open('w') as log:
        rc = subprocess.run([str(root / 'original.exe'), *args], stdout=log, stderr=subprocess.STDOUT).returncode
    results.append({'suite': 'original', 'case': case, 'exit': rc})
    print(variant, 'original', case, rc, flush=True)
for case in cases:
    frames = 96 if variant == 'matched' or case == 'lifecycle' else (160 if case.startswith('large_') or case == 'saturated' else 48)
    with (root / (case + '.log')).open('w') as log:
        rc = subprocess.run([str(root / 'diagnostic.exe'), case, str(frames)], stdout=log, stderr=subprocess.STDOUT).returncode
    results.append({'suite': 'diagnostic', 'case': case, 'frames': frames, 'exit': rc})
    (root / 'execution.json').write_text(json.dumps(results, indent=2) + '\n')
    print(variant, 'diagnostic', case, rc, flush=True)
(root / 'execution.json').write_text(json.dumps(results, indent=2) + '\n')
sys.exit(1 if any(r['exit'] for r in results) else 0)
