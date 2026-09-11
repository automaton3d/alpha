# Chief census and affinity occupancy

`attractor::begin()` now measures source constituents grouped by live chief:
one source (`r2 == 0`) per W, K identified by its W address, D by a parent
that currently names a K. Sources without a valid chief are excluded from
chief populations; dangling D references are counted as unresolved. S and P
are not island members. P counts in the census are source halves, not pairs.
No affinity, seed-family restriction, population quota, or dynamics is added.
Occupied centers are a spatial diagnostic, not an island count.

`begin(Observable::AffinityCells)` instead measures all lattice cells by their
exact affinity value. Its units and CSV headings are distinct. Neither those
populations nor their fitted regression zeros represent L/3 constituents.

Both modes initialize the previous snapshot from the actual seed, avoiding
artificial initial captures. Every boundary sample uses the same membership
for population and flows, including turnaround frames. For each address:
`N(frame) - N(previous) = captures - escapes`. A membership switch is an
escape plus a capture. Escape uses the previous charge class; capture uses
the new class. Charge-class changes within unchanged membership are recorded
separately in `conversionD_Orb/Umb`. Thus sector balance equals its initial
value plus cumulative net membership flux plus conversion. M/anti column
ordering is now consistent at recording and reporting, and netD/fr is divided
by the number of frames.

Regression is descriptive. A negative slope alone is not evidence of a
stable attractor; the nominal OLS errors do not model temporal correlation.
No burn-in is imposed by the observer. Empty previous populations are omitted
from the pooled fit; individual series retain zeros through births/deaths.
Statistics are available for short series even when fitting is unavailable.

Binary series format v4 stores the observable, topology, previous membership,
64-bit sector baseline, conversions and census. Legacy series are rejected
rather than silently reinterpreted. Load failure preserves existing history.
Resume requires a matching lattice checkpoint; resync rejects changes to the
saved membership or charge classes. Samples must be sequential.

`experiments/build_attractor_test.bat` validates identity across affinities,
equal-affinity distinct chiefs, unresolved parents after demotion, initial
baseline, all four sector classes, charge conversion, turnaround accounting,
short/constant series, resume continuity and incompatible-file rejection.

The production `island_census` runner compares its independent census against
this instrumentation each frame and additionally writes `constituents.csv`,
`sector_flux.csv` and `chief_census.csv`. Its final population/localization
counts now exclude disappeared chiefs. Use a new output directory to preserve
historical results. For example:

```
experiments\build_island_census.bat
mkdir build\census_corrected
build\island_census\island_census.exe 4 16384 build/census_corrected
```

These corrections validate measurement semantics, not spontaneous 9L islands,
local mediator dynamics, or an equilibrium population.

Validation: the production seed run in `build/census_corrected` completed four
frames with exit code zero (L=9, W=243, sieve=16384, no EXCLUSION macro).
Frame 1 remained all S; frames 2--4 had 235 K and 8 D, one occupied center,
zero unresolved parents and zero groups of three. The independent CSV audit
passed for every chief and frame:

```
python experiments/audit_chief_census.py build/census_corrected
```

This short run reproduces the aggregation failure; it is not a stability
measurement. Focused instrumentation tests and manuscript compilation passed.
