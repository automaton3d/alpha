# B0 -- the winding triple is measurable (instrument validated; no physics claimed yet)

**Question (B0).** Does the torus supply a measurable integer triple per island -- the winding numbers
of its centre of mass around the three non-contractible cycles of `T^3` -- and is the instrument
trustworthy?  This note answers only that second question.  Nothing below is a statement about the
model's dynamics.

**Artifacts.** `experiments/winding_observatory.cpp` (read-only), `experiments/build_winding_observatory.bat`,
logs/CSVs under `build/winding_observatory/`.

**Design constraint -- the lesson from the parked probe.**  `experiments/winding_probe.cpp` (now in
`attic/retired_experiments/`) *planted* a global label `chief_W[3]` and asked whether a rule could read
it: near tautological, not local, uninformative.  B0 does the opposite.  It defines **no rule** and
**plants no global**; it computes the triple from the transport history that already exists --
`experiments/inertia_fixture.h` accumulates `unwrapped[w][k] += wrappedDelta(previous, current)` --
and reports.

**Quantity.**  With the body layers `w < BODY = 3` sharing a centre,
`D_k = (sum_{w<BODY} unwrapped[w][k] - start_k) / BODY` (the centre of mass, exact),
`n_k = round(D_k) mod edge_k` (the winding number), plus `member_spread`, the spread of the members'
own unwindings -- because an island winding is only well defined while the members stay together.

**Geometry.**  The transport axis is the long side (15) and the other two stay short (5), so
`RMAX = min(LY,LZ)/2 = 2` in **all three** arms: the arms differ by an exact axis relabelling, which is
what makes the permutation test meaningful.  Body `1K+2D`, one reciprocal drive pair, reference sieve
`16384`, 80 light frames per arm.

## Results

| arm | disp_final (cells) | `W_final` | roles_changed_total | accumulator |
|---|---|---|---|---|
| baseline (no driver) | (0,0,0) | (0,0,0) | 1 | OK |
| x+ | (+20,0,0) | (5,0,0) | 1 | OK |
| x- | (-20,0,0) | (10,0,0) | 1 | OK |
| y+ | (0,+20,0) | (0,5,0) | 1 | OK |
| y- | (0,-20,0) | (0,10,0) | 1 | OK |
| z+ | (0,0,+20) | (0,0,5) | 1 | OK |
| z- | (0,0,-20) | (0,0,10) | 1 | OK |

`accum_mismatch = 0` in every arm (see below).  Established, all of it about the **instrument**:

1. **The accumulator is trustworthy.**  Recomputing one step from the fixture's own stored `previous`
   position reproduces the stored `unwrapped` exactly: no frame is skipped and no delta is lost.  (The
   first version of this check compared against a *fresh* snapshot and reported 60 "mismatches" -- a
   bug in the check, not in the fixture; the corrected check is the one that counts.)
2. **The triple is axis-diagonal and sign-covariant.**  Each driver winds only in its own axis, the
   three axes give the same magnitude (20 cells), and reversing the driver negates the displacement
   (`W = 10 = -20 mod 15`).  Axis-permutation symmetry -- the falsifier registered for the programme --
   already holds for the instrument, so a violation found later is a property of a *rule*, not of the
   measurement.
3. **The baseline does not wind.**  Without a driver the body does not transport at all
   (`disp == 0`, `W == (0,0,0)` for 80 frames): the J0-style baseline, and the reason B2 (a generator)
   is the programme's precondition.
4. **Cross-validation of the transport itself:** 20 cells in 80 frames = **0.250000 cells per light
   frame**, the published value for one rightward drive pair (`INERTIA.md`, and the manuscript's
   inertia subsection).  B0 reproduces it from an independent harness.


## Two findings that B1 must handle

* **The body is `2K + 1D` after frame 1, not `1K + 2D`.**  The K/D counts go `1,2 -> 2,1` at frame 1 and
  never change again (79 frames of stability): a reference `D x D` promotion (T2) fires on the first
  contact and the promoted delegate stays co-located with the rest.  So "the island" is ambiguous at
  the *group* level, and the triple measured here is the **body's** centre of mass.  B1 must define the
  winding **per chief** (per group) and report both whenever they differ.
* **Member spread is 0 throughout** (`member_spread = 0`, `max_body_span = 0`): the three layers move in
  lockstep, so the body winding is well defined -- the transport is rigid at this size.  Whether that
  survives captures and escapes is exactly the B1 question.

## Next steps (registered, not run)

* **B1 -- conservation.**  With the instrument validated, ask whether the triple survives an event:
  prepare a body that is **not** co-located (the fixture's `spread` argument) so delegates meet and
  promote, then check whether any per-chief unwrapped displacement jumps without a corresponding lattice
  step.  Falsifier: a discontinuity in `unwrapped` not explained by `wrappedDelta`.
* **B2 -- the generator.**  The reference seed transports nothing (B0 baseline; J0 measured
  `J = 0` identically).  Before any coupling question can be asked, some rule must *make* an island move
  from the seed; this is the same missing piece the `J` programme hit.
* **B3 -- coupling with charge.**  Correlate the per-chief triple with the charge word over a census.
  Only a *computed* triple may enter a rule, and for this to be a quantization statement rather than
  numerology the seed must stop assigning the charge words itself.
* **B4 -- axis symmetry.**  Any candidate rule must give the same result with `x<->y<->z` relabelled.
  B0 shows the instrument already satisfies this.

## What B0 does NOT establish

* Nothing about selection: the triple takes the value the driver imposes.  A protected or imposed
  invariant is not a quantum (the S3 lesson: `J` protects a planted size and selects nothing).
* Nothing about the reference path: the seed has no transport, so its triple is trivially `(0,0,0)`.
* Nothing about conservation across events: no capture or escape occurs in these arms.

## Reproduce

    experiments\build_winding_observatory.bat
    build\winding_observatory\winding_observatory.exe baseline 80 build\winding_observatory\baseline.csv
    build\winding_observatory\winding_observatory.exe x+      80 build\winding_observatory\xp.csv
    build\winding_observatory\winding_observatory.exe z-      80 build\winding_observatory\zm.csv

Arms: `baseline | x+ x- | y+ y- | z+ z-`.  The CSV carries
`frame,tick,dx,dy,dz,nx,ny,nz,K,D,S,P,roles_changed_total,member_spread,accum_mismatch,max_body_span,max_pair_gap`
(`roles_changed_total` is cumulative -- `1` means exactly one role transition, at frame 1).

If the `vcvars64.bat` call inside the build script fails (no `vswhere.exe` on PATH), `cl.exe` is already
on PATH on such machines and the `cl` line can be run directly; that is how the 19 Sep 2026 runs were
made.  The model sources are untouched by this harness: it links the ordinary reference build
(all candidate macros OFF).
