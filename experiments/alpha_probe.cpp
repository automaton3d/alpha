/*
 * alpha_probe.cpp - headless validation + alpha probes (e:\alpha\experiments).
 *
 * Replicates the controlled two-bubble scattering setup of the "It from bit"
 * manuscript (scatter_main.cpp): EL=7, SEP=4, 200 light frames, sieve
 * modulus 16384, W=2 complementary charges.  Validation target from the
 * manuscript reference run:
 *
 *     conv_calls (active passes) = 6996
 *     conv_s2b   (s2B passes)    = 18     -> 18/6996 ~ 1/388.7
 *
 * Usage:
 *   alpha_probe [EL] [SEP] [FRAMES] [SIEVE] [budget] [canon] [repel] [mag]
 *              [pol] [axis] [ch]
 *
 *   An optional eleventh argument [ch] (decimal 0..63) sets the SAME charge
 *   word on both bubbles (same-charge duo, e.g. 0x01 = 1), overriding the
 *   complementary 0x00/0x3F (scatter) or the neutral 0x00/0x00 (repel)
 *   defaults.  Together with [mag] it places a controlled equal-charge
 *   contact that cannot form a pair (Rules R1-R6), so the electric/magnetic
 *   channel (collapse/adiabatic/repel) is the only possible response.
 *
 *   With a sixth argument equal to "canon", runs the full canonical
 *   configuration (Platonic seed): W = 3*EL^2 layers, all source centres
 *   superposed at the lattice centre, no two-bubble overwrite.  SEP is then
 *   ignored.  This is the configuration in which the polarization
 *   election/broadcast/reconstruction is active.
 *
 *   A ninth argument equal to "pol" enables the experimental polarization
 *   bootstrap (polarization::seedAxis, the pbsb_two pattern): the default
 *   self-election cannot start from the zero-polarisation seed, so phase_step
 *   never reconstructs pol_u/pol_v and pB/sB stay false (alpha_D = 0 in the
 *   two-bubble geometry).  seedAxis installs a topological initial axis per
 *   layer and starts the helical broadcast walker immediately.  The optional
 *   tenth argument selects the axis source: "m" follows each bubble's
 *   momentum (canonical fallback +z), "z" uses +z for every layer, "za"
 *   alternates +z/-z by layer parity.  Without "pol" the run is the
 *   bit-identical reference (validation numbers of RESULTS.md).
 *
 * Only files under e:\alpha are touched.  e:\automaton is read-only input.
 *
 * The model defines a SINGLE scenario (the full dynamics).  The two-bubble
 * and canonical ("canon") arrangements are initial-condition probes within
 * that one scenario, not separate scenarios.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cmath>
#include <vector>
#include <array>
#include <numeric>

#include "model/simulation.h"
#include "model/polarization.h"
#include "config.h"

// ---------------------------------------------------------------------------
// Link stubs (same role as in scatter_main.cpp).
// ---------------------------------------------------------------------------
std::vector<unsigned int> voxels;   // used by tryAllocate() (initSim.cpp)

Config gConfig;                     // extern declared in config.h

namespace automaton
{
  bool convol_delay  = false;       // declared in model/simulation.h
  bool diffuse_delay = false;
  bool reloc_delay   = false;

  // CPU-only stubs (bridge.cpp is not linked: it drags in GUI symbols).
  bool tryEnableCuda()    { return false; }
  void disableCuda()      {}
  bool isCudaEnabled()    { return false; }
  bool swap_lattices_gpu(){ return false; }

  // defined in simulation.cpp, not exposed in simulation.h
  void trackCenter(unsigned x, unsigned y, unsigned z, unsigned w);
}

// ---------------------------------------------------------------------------
// placeSource: reset one whole winding layer to "unreached" and plant the
// source centre with a prescribed momentum.  Mirrors scatter_main.cpp.
// ---------------------------------------------------------------------------
static void placeSource(unsigned w, unsigned x, unsigned y, unsigned z,
                        int mx, int my, int mz, unsigned char chWord)
{
  const int ELi = (int)automaton::EL;

  for (int a = 0; a < ELi; ++a)
    for (int b = 0; b < ELi; ++b)
      for (int c = 0; c < ELi; ++c)
      {
        automaton::Cell& cc = automaton::getCell(
            automaton::lattice_curr, a, b, c, (int)w);
        automaton::Cell& cd = automaton::getCell(
            automaton::lattice_draft, a, b, c, (int)w);
        automaton::Cell& cm = automaton::getCell(
            automaton::lattice_partner, a, b, c, (int)w);
        for (automaton::Cell* p : { &cc, &cd, &cm })
        {
          p->r2  = INF_R2;
          p->r   = -1;
          p->u   = 0;
          p->v   = 0;
          p->active = 0;
          p->pol_u = p->pol_v = 0;
          p->bstamp = 0;
          p->m[0] = p->m[1] = p->m[2] = 0;
          p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
          p->kind = automaton::SourceKind::S;
          p->pair_idx   = automaton::NO_PAIR;
          p->pair_count = 0;
          p->parent     = automaton::NO_PARENT;
          p->leader_w   = automaton::NO_LEADER_W;
          p->a          = automaton::W_USED;   // orphan: no pre-existing island
          p->t = 0; p->f = 0;
          p->pB = p->sB = p->s2B = false;
          p->kB = p->bB = p->homB = p->cB = false;
          p->gB = false;
          p->c[0] = p->c[1] = p->c[2] = 0;
          p->g[0] = p->g[1] = p->g[2] = 0;
          p->spin_target = 0;
        }
      }

  // Complementary Rule-1 pair: layer 0 neutral, layer 1 full complement.
  // In "repel" mode both carry the same neutral word so the S x S rule with
  // equal field sign repels them (clean pass-through, no binding).
  for (automaton::Cell* p :
       { &automaton::getCell(automaton::lattice_curr, (int)x, (int)y, (int)z, (int)w),
         &automaton::getCell(automaton::lattice_draft, (int)x, (int)y, (int)z, (int)w),
         &automaton::getCell(automaton::lattice_partner, (int)x, (int)y, (int)z, (int)w) })
  {
    p->r2   = 0;
    p->r    = 0;
    p->u    = 2048;                  // central wave source amplitude
    p->v    = 0;
    p->ch   = chWord;
    // m is the immutable momentum.  reloc starts null; inertia fills it
    // via encounter only (applyMomentum consumes reloc, does not rewrite m).
    p->m[0] = mx; p->m[1] = my; p->m[2] = mz;
    p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
    p->t = 0; p->f = 0;
  }

  automaton::lcenters[w][0] = x;
  automaton::lcenters[w][1] = y;
  automaton::lcenters[w][2] = z;
  automaton::trackCenter(x, y, z, w);
}

// ---------------------------------------------------------------------------
// Sieve gate probability for a fixed positive amplitude u (exact closed form
// of the manuscript): P = g*ceil(u/g)/S, g = gcd(u,S).
// ---------------------------------------------------------------------------
static unsigned long long gcdULL(unsigned long long a, unsigned long long b)
{
  while (b) { unsigned long long t = a % b; a = b; b = t; }
  return a;
}

static double gateProbability(long long u, long long S)
{
  if (u <= 0 || S <= 0) return 0.0;
  unsigned long long g = gcdULL((unsigned long long)u, (unsigned long long)S);
  unsigned long long num = g * ((unsigned long long)(u - 1) / g + 1); // ceil(u/g)
  return (double)num / (double)S;
}

int main(int argc, char** argv)
{
  unsigned EL_in  = (argc > 1) ? static_cast<unsigned>(atoi(argv[1])) : 7u;
  unsigned SEP    = (argc > 2) ? static_cast<unsigned>(atoi(argv[2])) : 4u;
  unsigned FRAMES = (argc > 3) ? static_cast<unsigned>(atoi(argv[3])) : 200u;
  int      SIEVE  = (argc > 4) ? atoi(argv[4]) : 16384;
  double   budget = (argc > 5) ? atof(argv[5]) : 900.0;
  const bool canonical = (argc > 6) && (strcmp(argv[6], "canon") == 0);
  const bool repel = (argc > 7) && (strcmp(argv[7], "repel") == 0);
  int  mag = (argc > 8) ? atoi(argv[8]) : 1;   // initial |momentum|
  if (mag < 1) mag = 1;

  // Experimental polarization bootstrap ("pol"); see the header comment.
  const bool pol = (argc > 9) && (strcmp(argv[9], "pol") == 0);
  const char* axisKind = (argc > 10) ? argv[10] : "m";

  // Optional equal charge word for both bubbles (same-charge duo, 0..63).
  int chArg = (argc > 11) ? atoi(argv[11]) : -1;

  if (EL_in < 3 || EL_in > 31 || (EL_in % 2) == 0)
  {
    fprintf(stderr, "EL must be an odd value in [3,31]\n");
    return 1;
  }
  if (SEP < 2) SEP = 2;
  if (SIEVE < 1) SIEVE = 1;
  if (chArg < 0) chArg = -1;
  else if (chArg > 63) chArg = 63;

  const unsigned W_in = canonical ? 3u * EL_in * EL_in : 2u;

  printf("=== alpha probe: %s ===\n",
         canonical ? "canonical (Platonic seed) configuration"
                   : "controlled two-bubble scattering");
  printf("EL=%u W_USED=%u RMAX=%u frames=%u sieve=%d mode=%s mag=%d\n",
         EL_in, W_in, EL_in / 2u, FRAMES, SIEVE,
         canonical ? "canon" : (repel ? "repel" : "scatter"), mag);
  if (chArg >= 0)
    printf("same-charge duo: ch = 0x%02X on both bubbles\n", (unsigned)chArg);
  if (pol)
    printf("polarization bootstrap: axis=%s (seedAxis)\n", axisKind);

  automaton::calculateParameters(EL_in, W_in);
  automaton::s2b_target = SIEVE;
  if (!automaton::tryAllocate((int)EL_in, (int)W_in))
  {
    fprintf(stderr, "allocation failed: %s\n",
            automaton::lastAllocationError.c_str());
    return 2;
  }

  // Canonical startup (Platonic seed).  In the two-bubble mode the two
  // layers are then overwritten with the controlled configuration.
  for (int step = 0; step <= 7; ++step)
    automaton::initSimulation(step);

  if (!canonical)
  {
    const unsigned C   = automaton::CENTER;
    const unsigned off = SEP / 2u;
    unsigned char chA, chB;
    if (chArg >= 0)
    {
      // Same-charge duo with an arbitrary word (e.g. 0x01): equal charge,
      // not neutral, cannot form a pair under R1-R6.
      chA = chB = (unsigned char)chArg;
    }
    else
    {
      chA = 0x00;                                   // layer 0 word (Rule-1 pair)
      chB = repel ? 0x00 : 0x3F;                    // repel / complementary
    }

    placeSource(0, C - off, C, C,  +mag, 0, 0, chA);  // left, moving right
    placeSource(1, C + off, C, C,  -mag, 0, 0, chB);  // right, moving left
  }

  // ------------------------------------------------------------------
  // Polarization bootstrap ("pol"): install a topological initial axis
  // on every layer and start the helical broadcast walker immediately, so
  // phase_step() reconstructs pol_u/pol_v (and thus pB/sB) on the shells.
  // The unseeded self-election cannot boot from zero polarisation, which
  // is why alpha_D reads 0 in the reference two-bubble geometry.  This is
  // the pbsb_two pattern; ordinary reference runs do not call it.
  // ------------------------------------------------------------------
  if (pol)
  {
    for (unsigned w = 0; w < W_in; ++w)
    {
      const automaton::Cell& s = automaton::getCell(
          automaton::lattice_curr,
          automaton::lcenters[w][0], automaton::lcenters[w][1],
          automaton::lcenters[w][2], w);
      int ax, ay, az;
      if (strcmp(axisKind, "z") == 0)
      {
        ax = 0; ay = 0; az = 1;
      }
      else if (strcmp(axisKind, "za") == 0)
      {
        ax = 0; ay = 0; az = (w & 1u) ? -1 : 1;
      }
      else
      {
        // "m": follow the source momentum; canonical seeds have m = 0,
        // so fall back to +z (installAxis normalises to |axis| = RMAX).
        ax = s.m[0]; ay = s.m[1]; az = s.m[2];
        if (ax == 0 && ay == 0 && az == 0) az = 1;
      }
      if (!automaton::polarization::seedAxis(w, ax, ay, az))
        fprintf(stderr, "warning: polarization::seedAxis(w=%u) failed\n", w);
    }
    // Propagate the fresh source stamps to draft/partner so the first
    // encounter window sees a coherent partner snapshot.
    automaton::replicate();
  }

  const auto t0 = std::chrono::steady_clock::now();
  unsigned frame = 0;
  unsigned long long ticks = 0;

  // ---- accumulators ------------------------------------------------------
  long double sumA = 0.0L;            // per-frame average of <P(u,S)>
  long double totExpP = 0.0L;         // sum of P(u,S) over active cells
  unsigned long long totActive = 0;   // sum of active-cell counts
  long double sumDB = 0.0L;           // per-frame <sB>/<pB>
  unsigned long long sumPB = 0, sumSB = 0;
  unsigned long long nFramesSampled = 0;

  // alpha_E accumulators: transverse/radial ratios of the polarization pair
  // (pol_u, pol_v) over active cells with a nonzero pair.
  long double sumPolA = 0.0L;         // sum |pol_u|
  long double sumPolB = 0.0L;         // sum |pol_v|
  long double sumPolA2 = 0.0L;        // sum pol_u^2
  long double sumPolB2 = 0.0L;        // sum pol_v^2
  unsigned long long nPol = 0;        // active cells with nonzero pair
  unsigned long long nPvPos = 0, nPvNeg = 0;

  // u-histogram over active cells: exact integer values (bucket width 1).
  const int NH = 2048;
  unsigned long long histU[2048] = {0};

  // Per-frame records for the collision window (two-bubble mode).
  struct FrameRec { double d; long long calls; long long s2b;
                    long double expP; unsigned long long act;
                    unsigned long long pB, sB; };
  std::vector<FrameRec> recs;
  if (!canonical) recs.reserve(FRAMES);

  long long prevCalls = automaton::conv_calls;
  long long prevS2b   = automaton::conv_s2b;

  while (frame < FRAMES)
  {
    const bool newFrame = automaton::simulation();
    ++ticks;
    if (newFrame)
    {
      ++frame;

      // conv deltas accumulated during this frame's ticks.
      const long long dCalls = automaton::conv_calls - prevCalls;
      const long long dS2b   = automaton::conv_s2b - prevS2b;
      prevCalls = automaton::conv_calls;
      prevS2b   = automaton::conv_s2b;

      // ---- per-frame census over lattice_curr ---------------------------
      long double frameSum = 0.0L;
      unsigned long long nActive = 0;
      unsigned long long nPB = 0, nSB = 0;
      for (size_t i = 0; i < automaton::BLOCK; ++i)
      {
        const automaton::Cell& c = automaton::lattice_curr[i];
        if (c.active && c.u > 0)
        {
          frameSum += gateProbability(c.u, (long long)SIEVE);
          ++nActive;
          int idx = (int)((unsigned)c.u - 1u);
          if (idx >= NH) idx = NH - 1;
          ++histU[idx];
        }
        if (c.pB) ++nPB;      // in-phase bit: electric channel
        if (c.sB) ++nSB;      // transverse bit: magnetic channel
        if (c.active && (c.pol_u != 0 || c.pol_v != 0))
        {
          const long long pu = c.pol_u, pv = c.pol_v;
          const long long au = pu < 0 ? -pu : pu;
          const long long av = pv < 0 ? -pv : pv;
          sumPolA += (long double)au;
          sumPolB += (long double)av;
          sumPolA2 += (long double)pu * (long double)pu;
          sumPolB2 += (long double)pv * (long double)pv;
          if (pv > 0) ++nPvPos; else if (pv < 0) ++nPvNeg;
          ++nPol;
        }
      }
      if (nActive > 0) sumA += frameSum / (long double)nActive;
#ifdef ORPHAN_GUIDANCE_FSM
      // Experimental P1: the orphan shell is the thin band just AHEAD of each
      // layer's active front (r == f + 1), derived rather than stored.  A
      // concentric thin shell shows up as a compact r-band, not a volume.
      {
        unsigned long long nShell = 0;
        unsigned long long perR[64] = {0};
        for (size_t i = 0; i < automaton::BLOCK; ++i)
        {
          const automaton::Cell& c = automaton::lattice_curr[i];
          if (!automaton::isOrphanShell(c)) continue;
          ++nShell;
          int rr = c.r; if (rr < 0) rr = 0; if (rr > 63) rr = 63;
          ++perR[rr];
        }
        printf("[orphan] frame=%u shell=%llu active=%llu r:", frame, nShell, nActive);
        for (int rr = 0; rr < 64; ++rr) if (perR[rr]) printf(" %d=%llu", rr, perR[rr]);
        printf("\n");
      }
#endif
      totExpP   += frameSum;
      totActive += nActive;
      sumPB += nPB;
      sumSB += nSB;
      if (nPB > 0) sumDB += (long double)nSB / (long double)nPB;
      ++nFramesSampled;

      // Bubble separation (two-bubble mode only).
      double dsep = -1.0;
      if (!canonical)
      {
        const auto& p0 = automaton::lcenters[0];
        const auto& p1 = automaton::lcenters[1];
        int dx = (int)p1[0] - (int)p0[0];
        int dy = (int)p1[1] - (int)p0[1];
        int dz = (int)p1[2] - (int)p0[2];
        const int h = (int)(EL_in / 2);
        if (dx >  h) dx -= (int)EL_in; else if (dx < -h) dx += (int)EL_in;
        if (dy >  h) dy -= (int)EL_in; else if (dy < -h) dy += (int)EL_in;
        if (dz >  h) dz -= (int)EL_in; else if (dz < -h) dz += (int)EL_in;
        dsep = std::sqrt((double)(dx*dx + dy*dy + dz*dz));
        recs.push_back({ dsep, dCalls, dS2b, frameSum, nActive, nPB, nSB });
      }

      const double secs = std::chrono::duration<double>(
          std::chrono::steady_clock::now() - t0).count();
      if (secs >= budget)
      {
        fprintf(stderr, "[alpha_probe] budget hit at frame %u/%u\n",
                frame, FRAMES);
        break;
      }
    }
  }

  const double wall = std::chrono::duration<double>(
      std::chrono::steady_clock::now() - t0).count();

  printf("--------------------------------------------------------------\n");
  printf("ran %u light frames in %.2fs (%llu ticks)\n",
         frame, wall, ticks);
  printf("convolve: sieve=%d  active-passes=%lld  s2B-passes=%lld"
         "  pairs-formed=%lld  self-guard=%lld\n"
         "          collapse=%lld  adiah=%lld  repel=%lld\n",
         automaton::s2b_target,
         (long long)automaton::conv_calls, (long long)automaton::conv_s2b,
         (long long)automaton::conv_pair, (long long)automaton::conv_self,
         (long long)automaton::conv_collapse,
         (long long)automaton::conv_adiah,
         (long long)automaton::conv_repel);
  printf("recruit_events=%lld (orphan-shell x free-photon; macro ORPHAN_GUIDANCE_FSM)\n",
         (long long)automaton::recruit_events);

  if (automaton::conv_calls > 0)
    printf("realized overlap throughput = %lld/%lld = 1/%.3f\n",
           (long long)automaton::conv_s2b, (long long)automaton::conv_calls,
           (double)automaton::conv_calls / (double)automaton::conv_s2b);

  // Aggregate expected throughput (ratio of sums, smoother than the
  // frame average): the mean gate probability per active-cell sample.
  if (totActive > 0)
  {
    const long double expT = totExpP / (long double)totActive;
    printf("expected throughput (aggregate) = %.9Lf over %llu active-cell"
           " samples  (1/exp = %.9Lf)\n",
           expT, totActive, 1.0L / expT);
  }

  // Collision window (two-bubble mode): the per-frame conv deltas in the
  // frames whose bubble separation is within 1.5 cells of closest approach.
  if (!canonical && !recs.empty())
  {
    double dmin = 1e30, dmax = -1.0;
    size_t iMin = 0;
    for (size_t i = 0; i < recs.size(); ++i)
    {
      if (recs[i].d < dmin) { dmin = recs[i].d; iMin = i; }
      if (recs[i].d > dmax) dmax = recs[i].d;
    }

    long long winCalls = 0, winS2b = 0;
    long double winExpP = 0.0L;
    unsigned long long winAct = 0, winFrames = 0;
    unsigned long long winPB = 0, winSB = 0;
    for (const FrameRec& r : recs)
      if (r.d <= dmin + 1.5)
      {
        winCalls += r.calls; winS2b += r.s2b;
        winExpP += r.expP; winAct += r.act;
        winPB += r.pB; winSB += r.sB; ++winFrames;
      }

    printf("closest approach: %.2f cells at frame %u of %u (max separation %.2f)\n",
           dmin, (unsigned)iMin, frame, dmax);
    printf("collision window (d <= %.2f, %llu frames): calls=%lld s2B=%lld",
           dmin + 1.5, winFrames, winCalls, winS2b);
    if (winCalls > 0)
      printf("  realized = 1/%.3f", (double)winCalls / (double)winS2b);
    printf("\n");
    if (winAct > 0)
      printf("window expected throughput = %.9Lf over %llu active-cell"
             " samples  (1/exp = %.9Lf)\n",
             winExpP / (long double)winAct, winAct,
             1.0L / (winExpP / (long double)winAct));
    if (winPB > 0)
      printf("window alpha_D = sB/pB (total) = %llu/%llu = %.9Lf"
             "  (1/alpha_D = %.9Lf)\n",
             winSB, winPB, (long double)winSB / (long double)winPB,
             (long double)winPB / (long double)winSB);
    else
      printf("window alpha_D: no pB cells inside the collision window"
             " (polarization broadcast inactive?)\n");
  }

  // u-histogram over active cells (exact values): print nonzero entries.
  {
    printf("u-histogram (active cells, exact):\n");
    for (int i = 0; i < NH; ++i)
      if (histU[i] > 0)
        printf("  u=%4d : %llu\n", i + 1, (unsigned long long)histU[i]);
  }

  if (nFramesSampled > 0)
  {
    const long double meanA = sumA / (long double)nFramesSampled;
    printf("alpha_A = <P(u,S)> over active cells, frame-averaged = %.9Lf"
           "  (1/alpha_A = %.9Lf)\n", meanA, 1.0L / meanA);

    const long double meanDB = sumDB / (long double)nFramesSampled;
    printf("alpha_D = <sB>/<pB> :  frame-averaged ratio = %.9Lf"
           "  (1/alpha_D = %.9Lf)\n", meanDB, 1.0L / meanDB);
    if (sumPB > 0)
    {
      const long double totD = (long double)sumSB / (long double)sumPB;
      printf("alpha_D (total counts) = %llu/%llu = %.9Lf  (1/alpha_D = %.9Lf)\n",
             sumSB, sumPB, totD, 1.0L / totD);
    }
  }

  // alpha_E: transverse/radial ratios of the polarization pair.
  if (nPol > 0)
  {
    const long double mA = sumPolA / (long double)nPol;
    const long double mB = sumPolB / (long double)nPol;
    const long double mB2 = sumPolB2 / sumPolA2;   // <pol_v^2>/<pol_u^2>
    printf("alpha_E: over %llu pol-active cell samples\n", nPol);
    printf("  <|pol_v|>/<|pol_u|> = %.9Lf   (inverse %.9Lf)\n",
           mB / mA, mA / mB);
    printf("  <pol_v^2>/<pol_u^2> = %.9Lf   (inverse %.9Lf)\n",
           mB2, 1.0L / mB2);
    printf("  pol_v sign census: positive=%llu negative=%llu  (sB>0 = %llu)\n",
           nPvPos, nPvNeg, sumSB);
  }

  // alpha_F: fixed point of the normalized throughput map S -> H(S), where
  //   H(S) = < g*ceil(u/g) >  over the measured active u-spectrum, g=gcd(u,S).
  // alpha_F = U0 / S*  (U0 = 2048 central seed amplitude).
  {
    unsigned long long totU = 0;
    for (int i = 0; i < NH; ++i) totU += histU[i];
    if (totU > 0 && SIEVE > 0)
    {
      auto H = [&](unsigned long long S) -> long double
      {
        long double acc = 0.0L;
        for (int i = 0; i < NH; ++i)
          if (histU[i] > 0)
          {
            unsigned long long u = (unsigned long long)(i + 1);
            unsigned long long g = gcdULL(u, S);
            unsigned long long ce = (u - 1) / g + 1;   // ceil(u/g)
            acc += (long double)(g * ce) * (long double)histU[i];
          }
        return acc / (long double)totU;
      };

      printf("alpha_F fixed-point search over the measured u-spectrum:\n");
      unsigned long long seeds[3] = { (unsigned long long)SIEVE, 2048u,
                                      1ull << 20 };
      for (int k = 0; k < 3; ++k)
      {
        unsigned long long Scur = seeds[k];
        bool done = false;
        for (int it = 0; it < 1000 && !done; ++it)
        {
          long double h = H(Scur);
          unsigned long long Snext = (unsigned long long)(h + 0.5L);
          if (Snext < 1) Snext = 1;
          if (it < 12)
            printf("  seed %2d it %3d: S=%llu  H(S)=%.6Lf\n",
                   k, it, Scur, h);
          if (Snext == Scur || (it > 200 && Snext == Scur))
          {
            printf("  seed %d -> fixed point S* = %llu ; alpha_F = U0/S* = "
                   "%.9Lf (1/alpha_F = %.9Lf)\n",
                   k, Scur, 2048.0L / (long double)Scur,
                   (long double)Scur / 2048.0L);
            done = true;
          }
          Scur = Snext;
        }
        if (!done)
          printf("  seed %d -> no fixed point found in 1000 iterations"
                 " (end S=%llu)\n", k, Scur);
      }
      printf("  sanity H(SIEVE=%d) = %.6Lf  (S*<P> = 16384/%.3Lf = %.3Lf)\n",
             SIEVE, H((unsigned long long)SIEVE),
             16384.0L, 16384.0L / H((unsigned long long)SIEVE));
    }
  }

  return 0;
}