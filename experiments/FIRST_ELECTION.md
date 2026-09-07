# First-election reemission hypothesis

This is an isolated conditional geometry test, not a time-evolved CA run.
It assumes each superposed layer has reached the same complete maximum
integer-radius shell. It uses the payload function shared with polarization.cpp
and the current first-election dial sequence. The proposed new center is
the original winning cell, with radius zero; normalized m is not inverted.
No actual local reemission transition, subsequent contraction or aggregation
is implemented by this probe. It cannot validate those mechanisms.

## Size and method

L=255 is the largest odd multiple of three for which L^3 fits in the
payload's 24-bit spatial code. Larger sizes would require changing that
encoding before claiming an unambiguous election. This is a representation
limit for this test, not a measured full-simulator memory limit.

W=3L^2=195075 layers share the same candidate shell. The score depends on
only 9L=2295 possible keys, so each key's winner is computed once and reused
exactly. This avoids allocating the three full 3D+W lattices. The shell is
constructed geometrically; its dynamical formation is an assumption here.

## Reproduction

```
experiments\build_first_election.bat
build\first_election\first_election.exe 255 build\first_election\L255.csv
build\first_election\first_election.exe 9 build\first_election\L9.csv
```

| L | Layers | Candidate sites | Distinct winners | Maximum occupancy | Destinations changed by reverse election order |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 9 | 243 | 234 | 65 | 9 | 240 |
| 255 | 195075 | 205074 | 713 | 1190 | 194600 |

At L=255 the mean center offset is (126.956863, 6.414815, 0.280610),
relative to the initial center; the maximum radius is 127. Thus the result
breaks superposition but concentrates strongly toward positive x.

The score has only eight bits. Many candidates attain the same top score;
the spatial-address tie-break prefers larger x, then y, then z. In addition,
g_seedDial increments globally per election, making layer destinations
depend on traversal order. These are algorithmic asymmetries, not evidence
of a spontaneously selected physical direction or an effect of r*sin(r).

An independent Python enumeration verified all 243 winners at L=9.
All 195075 proposed L=255 centers were checked against the shell predicate.
Both executable runs exited zero. The test supports diversity of winners,
but rejects the current election as an order-independent, isotropic basis
for the proposed initial spreading. The reemission hypothesis itself is
not disproved by these defects in the election rule.
