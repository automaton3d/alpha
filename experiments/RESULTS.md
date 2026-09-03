# alpha probe - results

The model defines a SINGLE scenario (the full dynamics).  The two-bubble
("scatter") and canonical ("canon", Platonic seed) arrangements below are
initial-condition probes within that one scenario, not separate scenarios.

Harness: `alpha_probe.cpp` (headless, CPU, MSVC). Build: `build_probe.bat`.
Usage: `alpha_probe [EL] [SEP] [FRAMES] [SIEVE] [budget] [canon]`

## Validation (reference run of the "It from bit" manuscript)

    EL=7 SEP=4 FRAMES=200 SIEVE=16384
    conv_calls (active passes) = 6996      (manuscript: 6996)      MATCH
    conv_s2b   (s2B passes)    = 18        (manuscript: 18)        MATCH
    conv_pair                  = 0                                  MATCH
    18/6996 = 1/388.667                                              MATCH

The e:\alpha model sources are bit-identical to the reference: the harness
reproduces the manuscript's quantitative null result exactly.

## Candidate alpha_A = <P(u,S)> over active cells, frame-averaged

| L | frames | active passes | s2B passes | alpha_A        | 1/alpha_A |
|---|--------|---------------|-------------|----------------|-----------|
| 7 | 200    | 6996          | 18          | 0.004220105    | 236.96    |
| 9 | 200    | 9300          | 0           | 0.004265223    | 234.45    |
| 11| 200    | 11920         | 40          | 0.009991230    | 100.09    |
| 13| 100    | 7392          | 0           | 0.010428519    |  95.89    |

Notes:
- alpha_A is frame-averaged; conv counts are cumulative over the run.
- 1/alpha_A is NOT constant in L (236, 234, 100, 96): the candidate is
  strongly L-dependent and does NOT yet match 1/137.035999177.  By the
  pre-registered acceptance criterion of the paper this candidate is, at this
  stage, not a derivation of alpha.  The non-monotonicity in L mirrors the
  non-monotonicity in S already reported in the manuscript.
- Next: candidate alpha_D (<sB>/<pB>), overlap-window-restricted statistics,
  and the L-dependence at fixed geometry (SEP proportional to L).

## Candidate alpha_D = <sB>/<pB> (magnetic/electric populations)

| L | frames | pB cells (total) | sB cells (total) | sB/pB (total) | frame-avg ratio |
|---|--------|------------------|------------------|---------------|-----------------|
| 7 | 200    | 37392            | 0                | 0             | 0.000           |
| 9 | 200    | 21252            | 73140            | 3.442         | 0.385           |
| 11| 200    | 121678           | 67884            | 0.558         | 0.445           |
| 13| 100    | 63618            | 51398            | 0.808         | 0.230           |

Rejected as defined in the controlled two-bubble geometry:
- L=7: the transverse (magnetic) bit sB is NEVER set in the two-bubble
  configuration -> the polarization broadcast is not active there.
- L>=9: sB/pB is unstable and non-monotonic (3.44, 0.56, 0.81); per-frame
  ratios are dominated by frames with tiny denominators.
Conclusion: alpha_D is NOT observable in either geometry tried so far:
- controlled two-bubble (L=7): sB never set (magnetic channel inert);
- canonical Platonic seed (L=5, W=75, 400 frames): sB never set AND
  conv_calls = 0 -> all source centres stay superposed, so no separation and
  no convolution overlap in 400 frames of the headless default.
The ONLY runs with sB>0 are the two-bubble geometries at L=9,11, where the
ratio is unstable (0.56-3.44 total; 0.385-0.445 frame-averaged).
alpha_D observability requires separated sources, which the single-scenario
dynamics only produces from suitably separated initial conditions or after
far longer canonical evolution.

## Canonical (Platonic seed) run - alpha_A

L=5, W=75, 400 light frames (218 s wall):
  conv_calls = 0 (sources superposed, never separate)
  alpha_A = 0.003664175 -> 1/alpha_A = 272.9

## Option A - collision window and aggregate throughput (two-bubble, SEP=4)

| L | frames | realized (s2B/calls) | aggregate exp. 1/exp | alpha_A frame-avg 1/a |
|---|--------|----------------------|----------------------|-----------------------|
| 7 | 200    | 18/6996 = 1/388.7    | 1/342.2              | 1/237.0               |
| 9 | 200    | 0/9300               | 1/430.8              | 1/234.5               |
| 11| 200    | 40/11920 = 1/298.0   | 1/172.5              | 1/100.1               |
| 13| 120    | 0/9160               | 1/201.8              | 1/95.9                |
| 15| 150    | 45/6943 = 1/154.3    | 1/170.2              | -                     |

Notes:
- "aggregate expected throughput" = (sum P(u,S))/(#active-cell samples), i.e. the
  mean gate probability per active cell, ratio of sums (smoother than the
  frame average).  P(u,S) is the exact manuscript closed form.
- At SEP=4 the bubbles NEVER separate beyond ~3.5 cells after the pass
  (closest approach dmin ~1-2 near frame 0-18), so the "collision window"
  contains essentially the whole run; a true pre/post split needs larger SEP.
- The aggregate is still non-monotonic in L (342, 431, 172, 202, 170): no
  convergence to 1/137.036 by the pre-registered criterion.
- The realized throughput is dominated by discrete sieve resonance (zero at
  L=9,13); at L=15 it reaches 1/154.3, the closest value so far.

## Option A follow-up: SEP proportional to L with repulsive charges

Hypothesis: same-sign charges (S x S, equal field sign) should repel and give a
clean pre/post collision window.  Test at L=7, SEP=6, both words 0x00 (Q=0):

  closest approach 3.00 cells at frame 0; separation NEVER exceeds ~4.5 cells
  over 200 frames -> the window again contains the whole run.
  realized = 18/5414 = 1/300.8 ; aggregate expected = 1/341.9

Finding: under the single scenario the two sources do NOT separate after
contact, whether charges are complementary (attraction) or equal (nominally
repulsive).  The adiabatic (non-collapse) contact branch drifts the sources
toward each other whenever their active shells coincide, so isolated bubbles
form a sustained near-contact ("sticky") state at these parameters.  A clean
pre/post collision window is not realizable with the current probes; the
contact coupling must be measured as a sustained-proximity property (already
done: realized + aggregate expected tables above) or with different initial
conditions (third bubble, higher momentum) that force separation.

## Option A follow-up: higher initial momentum (mag = 2, 3)

Test: same two-bubble repel probe at L=7, SEP=4, initial |m| = 2 and 3.

  mag=2: realized 18/6996 = 1/388.667, closest approach 2.00 (max separation 2.00)
  mag=3: realized 18/6996 = 1/388.667, closest approach 2.00 (max separation 2.00)

Identical to mag=1: the initial momentum magnitude is INERT.  Sources advance
at one cell per tick regardless of |m|: the model's speed of light (the
"maximum information speed" v_max of the manuscript) cannot be exceeded by
construction.  Higher-momentum forcing is therefore not a viable control.

## Bound pair (S=64) - alpha_D and alpha_E; R-degeneracy discovery

KEY DISCOVERY: the polarization pair (pol_u, pol_v) is reconstructed with
R = RMAX - 2 = L/2 - 2.  At L=7, R=1 -> the circle u^2+v^2=R^4 degenerates and
pol_v is ALWAYS 0 (j=0 only).  L=7 is therefore degenerate for polarization:
this explains every earlier alpha_D = 0 null at L=7.  Non-degenerate
polarization requires L >= 9 (R >= 2).

L=7, S=64, SEP=4 (complement), 200 frames:
  10 pairs formed   (matches manuscript tab:sieve for S=64)
  realized 28019/28283 ~ 1/1.009 (gate open)
  closest approach 1.0 @ frame 28; max separation 4.69 (bound, oscillating)
  pol_v = 0 everywhere (R=1 degeneracy confirmed)

L=9, S=64, SEP=4 (complement), 200 frames:
  1 pair formed    (matches manuscript: L=9 fires at S=64, 1 pair)
  realized 1304/1758 = 1/1.35
  alpha_D frame-avg = 0.978  (sB ~ pB when channel open)
  alpha_E: <|pol_v|>/<|pol_u|> = 0.847162904 ; <pol_v^2>/<pol_u^2> = 0.847162904
    (R=2: pol states are pure |4| on one axis only; ratio = occupancy count ratio
     2777/3278)
  pol_v sign census: positive only (ascending branch j<R)

L=11, S=64, SEP=4 (complement), 200 frames:
  0 pairs formed
  alpha_D frame-avg = 1.881 (total 1.938)
  alpha_E: <|pol_v|>/<|pol_u|> = 2.000000000 ; <pol_v^2>/<pol_u^2> = 4.000000000
    (R=3: every sampled cell has |pol_v| = 2 |pol_u|, ascending branch only)

Interpretation: with the channel open the realized coupling saturates (~1) and
the "fine-structure" ratios alpha_E are small-integer or occupancy ratios of
the integer reconstruction (0.85, 2, 4) - nowhere near 1/137.036.  The ratio
set is quantized by R and the integer j sampled; the physically interesting
limit is large R (large L), where j fills the circle and the ratios become
geometric.  Simulation is capped at L<=31 (R<=13): compute the large-R limit
of <(pol_v/pol_u)^2> over uniform j analytically.

## Analytic alpha_E(R) survey (no simulation; alphaE_limit.cpp)

Definition: alpha_E(R) = sum_j pol_v(j)^2 / sum_j pol_u(j)^2 over the uniform
arrival phase j in [0, 2R), with the manuscript reconstruction formulas and
the integer square root.  Table stored in alphaE_table.txt (R = 1..1000).

  exact-sqrt (circle) continuous limit: alpha_E -> 2.0000 as R -> infinity
  integer-isqrt:                       alpha_E -> 1.995 at R = 1000 (-> 2)
  1/alpha_E -> 0.5012
  mean relative deficit <R^4-(u^2+v^2)>/R^4 ~ 1.6/R (fluctuating, not small)

CLEAN FALSIFICATION of candidate alpha_E as defined (ratio of sums over the
reconstruction circle): the pre-registered criterion requires convergence to
1/137.035999177; the measured object converges to 2.  The transverse-to-radial
energy ratio of the emergent polarization pair is a geometric factor 2, not
the fine-structure constant.  (The mean-of-ratios variant <(v/u)^2> is not
defined: u = 0 on the pure-transverse cells at even R, j = R/2, 3R/2.)

## Candidate alpha_C (Wyler-on-the-lattice + integer ratios; alphaC_limit.cpp)

Wyler formula fed with the lattice value of pi (integer sphere point counts):
as pi_hat -> pi the expression converges to Wyler's numerology
1/137.036082 - only ~6 significant digits, BELOW the pre-registered >=8 digit
criterion.  At R=30 (largest realistic lattice scale) 1/alpha_W = 136.98.

Curated structural-integer ratios {W/9L, W/phi_full, W/(2 RMAX), 9L/R, ...} at
L = 7..31: the closest any ratio comes to 137.036 is 135.0 at L=15
(~0.3 dex, factor ~1.5) - all fail by far.  CLEAN FALSIFICATION of alpha_C in
the available L range; the only lattice object approaching alpha is the Wyler
expression itself (6 digits), which fails the criterion.

## Candidate alpha_F (self-consistent sieve fixed point)

Fixed point S* of the map S -> H(S) = <g*ceil(u/g)> over the measured active
u-spectrum (exact u histogram).  Robust across seeds (16384, 2048, 2^20):

  L=7: S* = 48  -> alpha_F = U0/S* = 2048/48 = 42.67   (1/alpha_F = 0.0234)
  L=9: S* = 38  -> alpha_F = U0/S* = 2048/38 = 53.89   (1/alpha_F = 0.0186)

S* equals the run's mean active amplitude <u> (H(S>=<u>) ~ <u>), which is
geometry-dependent: alpha_F is L-dependent and ~1/50, far from 1/137.036.
CLEAN FALSIFICATION of alpha_F as defined.

## Report card after the analytic battery

  alpha_A (sieve throughput avg) : FAILS (L-dependent, 237..96)
  alpha_D (<sB>/<pB>)            : FAILS (0 weak coupling; ~1 open channel)
  alpha_E (<pol_v^2>/<pol_u^2>)  : FAILS (-> 2 exactly, analytic)
  alpha_C (Wyler-on-lattice)     : FAILS (-> 137.03608, 6 digits only)
  alpha_F (sieve fixed point)    : FAILS (S*=<u>, alpha_F ~ 1/50, L-dep)
  alpha_B (impulse/contact)      : NOT YET MEASURED (only remaining candidate)

Conclusion: no pre-registered *ratio-of-measured-quantities* candidate yields
1/137.035999177 at accessible L.  The emergent sieve/polarization algebra at
these scales produces O(1) or geometry-locked numbers.  The negative battery
is internally consistent and falsifiable; alpha_B is the last standing
measurement that requires new instrumentation.

## Candidate alpha_B (force branching per contact; read-only counters in
## interaction.cpp: conv_collapse / conv_adiah / conv_repel)

Read-only counters added at the force-branch decision points of convolute().
Dynamics untouched: L=7 reference re-validated bit-identical (6996/18/0).

  L=7 S=16384 (complement): s2B=18  -> collapse=18, adiah=0, repel=0
  L=7 S=64    (complement): s2B=28019 -> collapse=27885 (99.5%), adiah=88, repel=0
  L=7 S=64    (repel mode, both 0x00): IDENTICAL to complement run
      (charge word does not change conv/s2B counts nor the branching)

Structural facts:
  1. every gate-passing contact collapses (reissues): P(force | s2B) = 1.0 at
     the reference and ~0.995 open.  Repulsion (moveOneStepAway) is NEVER
     reached in these probes: the sources evolve out of the S x S / K x K
     kinds before such contacts occur.
  2. alpha_B therefore reduces to the gate throughput conv_s2b/conv_calls
     (= 1/388.7 at the reference) - no new number near 1/137.036.
  3. the "force" of the model is sieve-gated collapse, i.e. the electroweak
     channel is the only force; its rate is the realized throughput.

CLEAN FALSIFICATION of alpha_B as an independent candidate.

## S-scan: bypassing the S=16384 numerology?

Question: is the coupling hostage to the hand-set modulus S=16384 (= 2^14)?
Realized overlap throughput (s2B/calls) as a function of S, L=7, SEP=4:

  S=32    -> 1/1.005      S=384  -> 1/4.229    S=2730 -> 1/75.7
  S=64    -> 1/1.009      S=512  -> 1/7.125    S=4096 -> 1/108.7
  S=96    -> 1/1.129      S=768  -> 1/11.16    S=4800 -> 1/134.8   <- crosses 1/137!
  S=128   -> 1/1.820      S=1024 -> 1/21.64    S=5600 -> 1/437.3   (resonance drop)
  S=137   -> 1/1.345      S=1370 -> 1/38.62    S=16384-> 1/388.7
  S=160   -> 1/1.267      S=2048 -> 1/44.53
  S=256   -> 1/3.205

L-stability test of the 1/137 crossing (S=4800) at L=9:
  L=9 S=4096 -> 1/387.5
  L=9 S=4800 -> 1/56.0     (NOT stable: resonance is lattice-size specific)

CONCLUSION: choosing another S does NOT bypass the numerology.  The realized
throughput at any fixed S is a resonance comb set by gcd(u,S) and the phase
coincidence of the shells, so any single-S number (16384, 4800, or otherwise)
is numerology by construction.  The only values that could be physical would
come from (i) an S-averaged coupling, or (ii) a structural derivation of S
(e.g. S proportional to 137 x <u_overlap>); both remain open.  This closes
the "Eliminating the dials" question for the realized throughput: the dial
cannot be tuned away, only averaged or derived.

## S-averaged coupling (s_scan.sh; S = 128..16384 step 512, 32 points/L)

Statistic over the resonance comb, two-bubble SEP=4 (csv: sscan_L7/L9.csv):

                        L=7        L=9
  median r              1/182.1    1/196.8
  mean r (S>=2048)      1/164.7    1/146.5
  weighted (S>=2048)    1/184.0    1/153.4
  mean r (full grid)    1/36.5     1/29.9     (open-channel dominated)
  weighted (full grid)  1/73.1     1/75.9

FIRST DIAL-FREE NUMBER OF THE CAMPAIGN: the S-averaged statistics are
L-stable to ~10% (L=7 vs L=9), unlike every single-S value.  The median of
the comb is ~1/190 and the S>=2048 mean ~1/155, i.e. ~0.7-0.9 x 1/137.036:
the right neighbourhood, but NOT the fine-structure constant.  The residual
~20-40% gap is where a structural derivation of S (S proportional to
137 x <u_overlap>) would have to act.  For the first time the coupling is
measured without a hand-set dial.

## Extension to L=11, L=13 and the frame-count control

Extended scan (s_scan2.sh): L=11 (200 fr, S step 512, 32 pts), L=13 (100 fr,
S step 1024, 16 pts).  S-averaged statistics:

                        L=7     L=9     L=11    L=13
  median r              1/182   1/197   1/114   1/99
  mean r (S>=2048)      1/165   1/147   1/50    1/103

The L=7 vs L=9 "stability" does NOT extend: the median drifts toward ~1/100
at L=11/13.  BUT the frame-count control (L=13, same S, 100 vs 200 frames)
shows the realized ratio is strongly run-length dependent:

    S     100 fr   200 fr
    128   1/1.65   1/1.35
    5248  1/241    1/44     (5x)
    10368 1/128    1/78
    15488 1/282    1/119

The sieve passes are BURSTY (they fire when the shell phase coincides), so the
realized-overlap throughput is not stationary in time at large L.  Comparing
median r across L with different frame counts is invalid; even at fixed L the
S-averaged median is frame-length dependent.  CONCLUSION: the realized
count-ratio coupling, even S-averaged, is not a well-behaved observable at
accessible run lengths.  This closes the S-averaging route as a source of a
stable dial-free alpha; only the analytic expected-value observables
(<P(u,S)>) or far longer runs remain, and the campaign's negative battery
stands.



## FINAL REPORT CARD (all six pre-registered candidates measured)

  alpha_A (sieve throughput avg) : FAILS (L-dependent, 237..96)
  alpha_B (force/contact)        : FAILS (reduces to gate throughput, 1/388)
  alpha_C (Wyler-on-lattice)     : FAILS (-> 137.03608, 6 digits only)
  alpha_D (<sB>/<pB>)            : FAILS (0 weak coupling; ~1 open channel)
  alpha_E (<pol_v^2>/<pol_u^2>)  : FAILS (-> 2 exactly, analytic)
  alpha_F (sieve fixed point)    : FAILS (S*=<u>, alpha_F ~ 1/50, L-dep)

  NONE of the six pre-registered candidates converges to 1/137.035999177 at
  accessible L under the strict criterion.  The negative battery is complete,
  internally consistent, and falsifiable.  Side results of the campaign:
  L=7 (R=1) is degenerate for the polarization pair; the two-bubble contact
  is sticky (no pre/post window); v_max = 1 cannot be exceeded; the s2B gate
  is the only force and it is collapse-dominated.
