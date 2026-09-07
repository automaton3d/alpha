# Search for an initial physical asymmetry

Build with experiments/build_seed_asymmetry.bat and run
build/seed_asymmetry/seed_asymmetry.exe from the repository root.
The harness uses the common kernel, without ISLAND_SEED_EXPERIMENT,
with a superposed L=9, W=243 seed. It does not change positions or fields
after initialization. Output: build/seed_asymmetry/trace.csv.

Observed over ticks 0 through 243 (the first encounter window): zero
differences in (u,v,r2,active) between layers, zero differences under
inversion through the initial center or x/y exchange, zero polarized cells,
zero displaced sources and zero P sources. The executable exited zero.
This interval precedes the first frame boundary and does not test a full
expansion, an era, or long-term symmetry breaking. Only the named symmetry
operations and fields are checked, not every possible symmetry.

Code inspection explains the absence of a field difference: the seed has
identical u,v,r2,t and zero polarization in all layers. The wave equation
reads these fields and a symmetric six-neighbor sum, not charge or affinity.
Thus differing charge words are not yet a mechanism for differing u,v.
This conditional equality persists as long as interactions do not change
the geometric/clock inputs differently. It is not a claim that the entire
interaction kernel preserves every symmetry forever.

Polarization reconstruction requires nonzero broadcast stamps. The broadcast
requires an elected axis, so it cannot supply the first directional datum
under the current polarization-only election. The scalar radial cloud may
select radii but cannot distinguish opposite positions by itself. Cubic
discretization distinguishes some angular classes, not a unique signed
direction from an inversion-symmetric state.

Other code paths do introduce directions: free-pair release uses w modulo
three and address parity; relocation/contact arbitration uses ordered
addresses and scans. Those are explicit algorithmic choices, not evidence
that the radial cloud physically selects a direction. Coupling charge to
wave amplitude alone would distinguish layers but still need not break
each layer's spatial inversion symmetry. A directional internal state or
an explicit symmetry-breaking interaction would need its own model rule.
