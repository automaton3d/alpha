/*
 * inertia_probe.cpp - Rev 2 inertia pilot: a complete W-island (electron of
 * Orbis, provisional charge card) plus propeller pairs on the rectangular
 * tube.  Measures the centre-of-mass velocity of the island elements plus
 * the propellers frame by frame (manuscript, "Propeller" subsection).
 *
 * Notation (gravity_probe_DESIGN.md, section 13): pair (P) = SourceKind::P
 * neutral complementary pair (generic gauge fragment); propeller = the pair
 * role carrying a non-zero momentum vector m (|m| = RMAX = L/2 along the
 * push axis); layer = structural unit; the island = n_i layers (chief K + S).
 * reloc starts at zero and accumulates only via encounter (P×K / P×D);
 * applyMomentum consumes reloc without altering m.
 *
 * Usage:
 *   inertia_probe LX LY LZ n_i n_pi FRAMES SIEVE mode csv
 *   (edges odd >= 5, LX long; n_pi >= 3 propeller pairs)
 */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>

#include "model/simulation.h"
#include "config.h"

std::vector<unsigned int> voxels;   // used by tryAllocate() (initSim.cpp)
Config gConfig;                     // extern declared in config.h

namespace automaton
{
  bool convol_delay  = false;
  bool diffuse_delay = false;
  bool reloc_delay   = false;

  bool tryEnableCuda()    { return false; }
  void disableCuda()      {}
  bool isCudaEnabled()    { return false; }
  bool swap_lattices_gpu(){ return false; }

  void trackCenter(unsigned x, unsigned y, unsigned z, unsigned w);
}

// Deterministic LCG for the pseudo-random seed geometry.
static unsigned long g_rng = 2463534242u;
static unsigned rnd(unsigned m)
{
  g_rng = g_rng * 1664525u + 1013904223u;
  return (unsigned)((g_rng >> 8) % (m ? m : 1u));
}

// ---------------------------------------------------------------------------
// Layer helpers: clearLayer resets one full W-layer to "unreached";
// plantBubble plants one source centre with the requested charge word.
// ---------------------------------------------------------------------------
static void clearLayer(unsigned w)
{
  const int ELXi = (int)automaton::ELX;
  const int ELYi = (int)automaton::ELY;
  const int ELZi = (int)automaton::ELZ;

  for (int a = 0; a < ELXi; ++a)
    for (int b = 0; b < ELYi; ++b)
      for (int c = 0; c < ELZi; ++c)
      {
        automaton::Cell* ps[3] = {
          &automaton::getCell(automaton::lattice_curr, a, b, c, (int)w),
          &automaton::getCell(automaton::lattice_draft, a, b, c, (int)w),
          &automaton::getCell(automaton::lattice_partner, a, b, c, (int)w)
        };
        for (automaton::Cell* p : ps)
        {
          p->r2  = INF_R2;
          p->r   = -1;
          p->u   = 0;
          p->v   = 0;
          p->ch  = 0;             // neutral matter default; sources set theirs
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
          p->a          = automaton::W_USED;   // orphan
          p->t = 0; p->f = 0;
          p->pB = p->sB = p->s2B = false;
          p->kB = p->bB = p->hB = p->cB = false;
          p->gB = false;
          p->c[0] = p->c[1] = p->c[2] = 0;
          p->g[0] = p->g[1] = p->g[2] = 0;
          p->spin_target = 0;
        }
      }
}

static void plantBubble(unsigned w, int x, int y, int z,
                        unsigned char chWord, int mx, int my, int mz,
                        automaton::SourceKind kind, bool island)
{
  automaton::Cell* ps[3] = {
    &automaton::getCell(automaton::lattice_curr, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_draft, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_partner, x, y, z, (int)w)
  };
  for (automaton::Cell* p : ps)
  {
    p->r2   = 0;
    p->r    = 0;
    p->u    = 2048;
    p->v    = 0;
    p->ch   = chWord;
    // m is the immutable momentum (harness may pre-load |m| = RMAX on
    // propellers).  reloc starts null; inertia fills it via encounter.
    p->m[0] = mx; p->m[1] = my; p->m[2] = mz;
    p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
    p->t = 0; p->f = 0;
    p->kind = kind;
    p->a = island ? 0u : automaton::W_USED;
    p->leader_w = island ? 0u : automaton::NO_LEADER_W;
  }

  automaton::lcenters[w][0] = (unsigned)x;
  automaton::lcenters[w][1] = (unsigned)y;
  automaton::lcenters[w][2] = (unsigned)z;
  automaton::trackCenter((unsigned)x, (unsigned)y, (unsigned)z, w);
}

// ---------------------------------------------------------------------------
// Provisional Orbis-electron charge card (DESIGN section 13):
//   -L -> ch 0x08 (fragment 100000, Orbis); -Lbar -> ch 0x17;
//   complement of -L -> 0x37 (propeller partner / nu_e half).
// ---------------------------------------------------------------------------
static unsigned char chElectron()  { return 0x08; }
static unsigned char chNeutrinoA() { return 0x08; }
static unsigned char chNeutrinoB() { return 0x37; }
static unsigned char chPropeller(int p)
{
  return (p & 1) ? chNeutrinoA() : chNeutrinoB();
}

// ---------------------------------------------------------------------------
// main (part 2)
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
  if (argc < 7)
  {
    fprintf(stderr,
            "usage: inertia_probe LX LY LZ n_i n_pi FRAMES [SIEVE] [mode] [csv]\n");
    return 1;
  }

  unsigned LX    = (unsigned)atoi(argv[1]);
  unsigned LY    = (unsigned)atoi(argv[2]);
  unsigned LZ    = (unsigned)atoi(argv[3]);
  unsigned n_i   = (unsigned)atoi(argv[4]);
  unsigned n_pi  = (unsigned)atoi(argv[5]);
  unsigned FRAMES= (unsigned)atoi(argv[6]);
  int      SIEVE = (argc > 7) ? atoi(argv[7]) : 16384;
  const bool repel = (argc > 8) && (strcmp(argv[8], "repel") == 0);
  const char* csvPath = (argc > 9) ? argv[9] : "inertia_run.csv";

  if (LX < 5 || LY < 5 || LZ < 5 || (LX % 2) == 0 ||
      (LY % 2) == 0 || (LZ % 2) == 0)
  {
    fprintf(stderr, "edges must be odd and >= 5 (got %u x %u x %u)\n",
            LX, LY, LZ);
    return 1;
  }
  if (n_i < 2 || n_pi > 8)
  {
    fprintf(stderr, "need n_i >= 2 and 0 <= n_pi <= 8 (got %u, %u)\n",
            n_i, n_pi);
    return 1;
  }
  if (SIEVE < 1) SIEVE = 1;

  // Island charge composition: "electron" (default) = -L singletons plus nu_e
  // dressing pairs [-L : +L]  (complement 0x37, the word that actually
  // satisfies canFormPair Rule 1 with -L = 0x08).  "same" = legacy all -L.
  const bool electron = (argc <= 10) || (strcmp(argv[10], "same") != 0);

  const unsigned W = n_i + 2 * n_pi;
  const size_t estBytes = (size_t)LX * LY * LZ * W * 64u;
  if (estBytes > (size_t)8ull * 1024 * 1024 * 1024)
  {
    fprintf(stderr, "geometry needs ~%.1f GB; reduce edges/W.\n",
            (double)estBytes / (1024.0 * 1024.0 * 1024.0));
    return 1;
  }

  printf("=== inertia probe (Rev 2): island + propeller pairs on the tube ===\n");
  printf("tube LX=%u LY=%u LZ=%u | n_i=%u n_pi=%u W=%u | frames=%u sieve=%d mode=%s\n",
         LX, LY, LZ, n_i, n_pi, W, FRAMES, SIEVE, repel ? "repel" : "attract");

  automaton::s2b_target = SIEVE;
  if (!automaton::tryAllocateTube(LX, LY, LZ, W))
  {
    fprintf(stderr, "allocation failed: %s\n",
            automaton::lastAllocationError.c_str());
    return 2;
  }

  // One complete island: n_i layers with common affinity (leader 0).
  automaton::ISLAND_SIZE  = n_i;
  automaton::ISLAND_COUNT = 1;

  for (int step = 0; step <= 7; ++step)
    automaton::initSimulation(step);

  // Geometry: everything in the first eighth of the long axis.  Canonical
  // seed (initSim "Platonic premise"): all source centres are born at a
  // single point -- the island layers are CO-LOCATED (same centre, common
  // leader/affinity).  In the canonical model the island is a stack of W
  // layers sharing one centre; the layers separate only through
  // interaction-driven relocation, never at birth.  The earlier "random ball
  // of radius 1" seed was non-canonical and caused the observed dispersion.
  const unsigned XQ   = LX / 8u;
  const unsigned cy   = (automaton::ELY - 1u) / 2u;
  const unsigned cz   = (automaton::ELZ - 1u) / 2u;
  const int RMAXi     = (int)automaton::RMAX;
  const int xI        = (int)(XQ * 3u / 4u);
  const int yI        = (int)cy, zI = (int)cz;

  if (xI < 2)
  {
    fprintf(stderr,
            "first eighth too small for a co-located island plus propellers\n"
            "on its left: increase LX (need LX/8 >= 3).\n");
    return 3;
  }

  // Island: n_i layers co-located at (xI, cy, cz), one complete W-island
  // sharing the centre and the leader/affinity (canonical "Platonic premise").
  // Composition (Orbis electron card, DESIGN 13):
  //   * layer 0 = chief (K), charge -L  (0x08);
  //   * 'singles' -L S-layers (0x08);
  //   * 'pairs' nu_e dressings: 2 layers each, [ -L (0x08) : +L (0x37) ],
  //     the complementary pair that canFormPair Rule 1 actually binds.
  // The binding is EMERGENT (dressing pairs share the island leader); nothing
  // is pinned.  (all -L / all 0x08 is the degenerate "same" control.)
  const unsigned pairs   = electron ? n_i / 3u : 0u;
  const unsigned singles = n_i - 2u * pairs;          // >= 1 (chief)
  fprintf(stderr,
          "island composition: %s  (-L singles=%u incl. chief, nu_e pairs=%u)\n",
          electron ? "electron" : "same-charge", singles, pairs);
  for (unsigned i = 0; i < n_i; ++i)
  {
    unsigned char chW;
    if (i < singles)
      chW = chElectron();                       // -L = 0x08
    else
      chW = ((i - singles) & 1u) ? chNeutrinoB() // +L = 0x37
                                 : chElectron(); // -L = 0x08
    clearLayer(i);
    const bool chief = (i == 0);
    plantBubble(i, xI, yI, zI, chW, 0, 0, 0,
                chief ? automaton::SourceKind::K : automaton::SourceKind::S,
                true);
  }

  // Propellers: n_pi complementary pairs on the left of the island, spaced so
  // their expanding pulses (max radius RMAX) can reach the island shells
  // (distance D = xI - xp <= 2*RMAX).  Each half carries m = (+RMAX, 0, 0)
  // — modulus L/2 along +x — matching the dynamic-election scale used by
  // polarization::installAxis.  reloc stays zero until encounter transfers.
  const int mProp = RMAXi > 0 ? RMAXi : 1;
  for (unsigned p = 0; p < n_pi; ++p)
  {
    const int span = xI - 1;                  // xp in [1, xI-1]
    const int xp = 1 + (int)(((unsigned)span * p) / n_pi);
    int yp = (int)cy, zp = (int)cz;
    if (RMAXi > 1)
    {
      yp += (int)rnd((unsigned)(2 * RMAXi + 1)) - RMAXi;
      zp += (int)rnd((unsigned)(2 * RMAXi + 1)) - RMAXi;
    }
    yp = std::max(0, std::min((int)(automaton::ELY - 1), yp));
    zp = std::max(0, std::min((int)(automaton::ELZ - 1), zp));

    const unsigned wa = n_i + 2 * p;
    const unsigned wb = n_i + 2 * p + 1;
    clearLayer(wa);
    clearLayer(wb);
    const unsigned char chA = chPropeller((int)p);
    const unsigned char chB =
        repel ? chA : (unsigned char)(~chA & 0x3F);
    // Both halves: propeller push toward +x with |m| = RMAX.
    plantBubble(wa, xp, yp, zp, chA, mProp, 0, 0,
                automaton::SourceKind::S, false);
    plantBubble(wb, xp, yp, zp, chB, mProp, 0, 0,
                automaton::SourceKind::S, false);
  }

  const auto t0 = std::chrono::steady_clock::now();
  unsigned long long ticks = 0;
  unsigned frame = 0;

  // Element convention (manuscript "Propeller"): N = n_i + n_pi elements,
  // each propeller pair counted ONCE (centre of the two complementary
  // layers).  prevX[..] holds the previous light-frame centre per element so
  // per-frame kicks n and the island displacement sum can be counted.
  const unsigned N = n_i + n_pi;
  std::vector<double> prevX(N, -1e30);   // island layers first, then pairs

  FILE* f = fopen(csvPath, "w");
  if (f)
    fprintf(f, "frame,tick,xcmN,vcmN,nIsland,dxIslandSum,nProp,dxPropSum,nSeam,xIsland,vIsland,chiefX,nBound,coreX\n");

  // Optional per-light-frame layer dump (".pos") consumed by the output
  // animation plugin (make_anim.py -> PPM frames -> ffmpeg GIF).  Header:
  //   P LX LY LZ RMAX W_USED n_i n_pi
  // then per frame:  F <frame> <tick>  and one line per layer w:
  //   w cx cy cz kind m0 m1 m2 reloc0 reloc1 reloc2 t pB sB pair a
  // (kind: 0=K 1=S 2=D 3=P; a = leader affinity or -1 when orphan)
  char posPath[512];
  snprintf(posPath, sizeof(posPath), "%s.pos", csvPath);
  FILE* fp = fopen(posPath, "w");
  if (fp)
    fprintf(fp, "P %u %u %u %u %u %u %u\n",
            LX, LY, LZ, (unsigned)automaton::RMAX, W, n_i, n_pi);

  // Dense dump: optional second trace "<csv>.dense" sampled every
  // INERTIA_DUMP_DIV ticks (default 0 = off).  Same .pos schema with a
  // monotonically increasing sample index; dynamics untouched.
  long denseDiv = 0;
  if (getenv("INERTIA_DUMP_DIV"))
    denseDiv = atol(getenv("INERTIA_DUMP_DIV"));
  char densePath[512];
  snprintf(densePath, sizeof(densePath), "%s.dense", csvPath);
  FILE* fd = (denseDiv > 0) ? fopen(densePath, "w") : nullptr;
  unsigned long long denseIdx = 0;
  if (fd)
    fprintf(fd, "P %u %u %u %u %u %u %u\n",
            LX, LY, LZ, (unsigned)automaton::RMAX, W, n_i, n_pi);

  // Per-frame records for the end-of-run inertia summary (scatter_main
  // style: CoM velocity windows split at the first kick).
  std::vector<double> vcmA, dxIA;
  std::vector<unsigned> nIA;

  while (frame < FRAMES)
  {
    const bool newFrame = automaton::simulation();
    ++ticks;

    // Dense sample (mid light-frame): same layer dump as the frame trace.
    if (fd && (ticks % (unsigned long long)denseDiv) == 0)
    {
      ++denseIdx;
      fprintf(fd, "F %llu\n", denseIdx);
      for (unsigned w = 0; w < W; ++w)
      {
        const auto& p = automaton::lcenters[w];
        const automaton::Cell& sc = automaton::getCell(
            automaton::lattice_curr, (int)p[0], (int)p[1], (int)p[2], (int)w);
        fprintf(fd,
                "%u %u %u %u %d %d %d %d %d %d %d %u %d %d %d %d\n",
                w, p[0], p[1], p[2], (int)sc.kind,
                sc.m[0], sc.m[1], sc.m[2],
                sc.reloc[0], sc.reloc[1], sc.reloc[2],
                sc.t,
                sc.pB ? 1 : 0, sc.sB ? 1 : 0,
                (sc.pair_idx != automaton::NO_PAIR) ? 1 : 0,
                sc.a == automaton::W_USED ? -1 : (int)sc.a);
      }
    }

    if (!newFrame) continue;
    ++frame;

    // Element centres.  NOTE: per-element displacement is RAW (no toroidal
    // wrap): in this model a bubble can relocate to the contact site far away
    // in one light frame (legitimate large dx).  Applying a wrap correction
    // per element corrupts those moves (|d| > ELX/2 happens for teleports,
    // not only for seam crossings).  nSeam flags elements whose raw dx is so
    // large that the seam-vs-teleport ambiguity matters.
    double islandXsum = 0.0;
    double dxIslandSum = 0.0;
    unsigned nIsland = 0;
    unsigned nSeam = 0;
    for (unsigned i = 0; i < n_i; ++i)
    {
      const double x = (double)automaton::lcenters[i][0];
      islandXsum += x;
      if (prevX[i] > -1e29)
      {
        const double d = x - prevX[i];
        if (d > 0.5 || d < -0.5) ++nIsland;
        if (d > (double)automaton::ELX / 2.0 ||
            d < -(double)automaton::ELX / 2.0) ++nSeam;
        dxIslandSum += d;
      }
      prevX[i] = x;
    }

    double pairXsum = 0.0;
    double dxPropSum = 0.0;
    unsigned nProp = 0;
    for (unsigned p = 0; p < n_pi; ++p)
    {
      const unsigned wa = n_i + 2 * p;
      const unsigned wb = n_i + 2 * p + 1;
      const double cx = 0.5 * ((double)automaton::lcenters[wa][0] +
                               (double)automaton::lcenters[wb][0]);
      pairXsum += cx;
      const unsigned e = n_i + p;
      if (prevX[e] > -1e29)
      {
        const double d = cx - prevX[e];
        if (d > 0.5 || d < -0.5) ++nProp;
        if (d > (double)automaton::ELX / 2.0 ||
            d < -(double)automaton::ELX / 2.0) ++nSeam;
        dxPropSum += d;
      }
      prevX[e] = cx;
    }

    const double xcmN = (islandXsum + pairXsum) / (double)N;
    const double dxTotal = dxIslandSum + dxPropSum;
    // Mean centre advance (manuscript: dxTotal/N = measured xcmN drift, now
    // consistent because both use raw element displacement).
    const double vcmN = dxTotal / (double)N;

    const double xImean = islandXsum / (double)n_i;
    static double prevIsland = -1e30;
    const double vIsland =
        (prevIsland > -1e29) ? (xImean - prevIsland) : 0.0;
    prevIsland = xImean;

    // Core bound check: chief layer centre and how many island layers stay
    // within a few shell radii of it (coherent particle vs dispersed layers).
    const double chiefX = (double)automaton::lcenters[0][0];
    const double coreWin = 2.0 * (double)RMAXi + 1.0;
    double coreXsum = 0.0;
    unsigned nBound = 0;
    for (unsigned i = 0; i < n_i; ++i)
    {
      const double x = (double)automaton::lcenters[i][0];
      if (x >= chiefX - coreWin && x <= chiefX + coreWin)
      {
        coreXsum += x;
        ++nBound;
      }
    }
    const double coreX =
        (nBound > 0) ? (coreXsum / (double)nBound) : chiefX;

    if (fp)
    {
      fprintf(fp, "F %u %llu\n", frame, ticks);
      for (unsigned w = 0; w < W; ++w)
      {
        const auto& p = automaton::lcenters[w];
        const automaton::Cell& sc = automaton::getCell(
            automaton::lattice_curr, (int)p[0], (int)p[1], (int)p[2], (int)w);
        fprintf(fp,
                "%u %u %u %u %d %d %d %d %d %d %d %u %d %d %d %d\n",
                w, p[0], p[1], p[2], (int)sc.kind,
                sc.m[0], sc.m[1], sc.m[2],
                sc.reloc[0], sc.reloc[1], sc.reloc[2],
                sc.t,
                sc.pB ? 1 : 0, sc.sB ? 1 : 0,
                (sc.pair_idx != automaton::NO_PAIR) ? 1 : 0,
                sc.a == automaton::W_USED ? -1 : (int)sc.a);
      }
    }

    if (getenv("INERTIA_DEBUG") && frame <= 6)
    {
      fprintf(stderr, "[dbg f%u t=%llu]", frame, ticks);
      for (unsigned w = 0; w < W; ++w)
        fprintf(stderr, " w%u=%u", w, automaton::lcenters[w][0]);
      fprintf(stderr, " | prevX:");
      for (unsigned e = 0; e < N; ++e)
        fprintf(stderr, " e%u=%.0f", e, prevX[e]);
      fprintf(stderr, "\n");
    }

    if (f)
      fprintf(f, "%u,%llu,%.6f,%.6f,%u,%.6f,%u,%.6f,%u,%.6f,%.6f,%.6f,%u,%.6f\n",
              frame, ticks, xcmN, vcmN,
              nIsland, dxIslandSum, nProp, dxPropSum, nSeam,
              xImean, vIsland, chiefX, nBound, coreX);

    vcmA.push_back(vcmN);
    dxIA.push_back(dxIslandSum);
    nIA.push_back(nIsland);
  }

  const double wall = std::chrono::duration<double>(
      std::chrono::steady_clock::now() - t0).count();
  if (f) fclose(f);
  if (fp) fclose(fp);
  if (fd) fclose(fd);

  // ---------------------------------------------------------------------
  // Inertia summary (mirrors scatter_main's centre-of-mass response).
  // "incoming" window: frames before the first island kick (rest baseline);
  // "outgoing" window: frames from first kick to the end.
  // ---------------------------------------------------------------------
  {
    size_t firstKick = vcmA.size();
    for (size_t i = 0; i < nIA.size(); ++i)
      if (nIA[i] > 0) { firstKick = i; break; }

    const size_t nb = vcmA.size();
    double inSum = 0.0; size_t inN = 0;
    for (size_t i = 0; i < firstKick; ++i) { inSum += vcmA[i]; ++inN; }
    double outSum = 0.0, outDX = 0.0; size_t outN = 0, outKicks = 0;
    for (size_t i = firstKick; i < nb; ++i)
    {
      outSum += vcmA[i];
      outDX += dxIA[i];
      if (nIA[i] > 0) ++outKicks;
      ++outN;
    }
    const double vIn  = (inN  > 0) ? inSum  / (double)inN  : 0.0;
    const double vOut = (outN > 0) ? outSum / (double)outN : 0.0;
    unsigned sumKicks = 0;
    for (size_t i = 0; i < nIA.size(); ++i) sumKicks += nIA[i];

    printf("\n== INERTIA SUMMARY (N = %u = %u island + %u propellers) ==\n",
           N, n_i, n_pi);
    if (firstKick >= nb)
    {
      printf("no island kick observed in %zu frames (gate closed?): "
             "CoM stays at rest.\n", nb);
    }
    else
    {
      printf("first island kick at frame %zu; %zu kick frames after that.\n",
             firstKick + 1, outKicks);
      printf("incoming mean v_CoM : %+.4f cells/frame (rest baseline)\n", vIn);
      printf("outgoing mean v_CoM : %+.4f cells/frame\n", vOut);
      printf("|delta v_CoM|       : %.4f cells/frame\n", vOut - vIn);
      printf("total island kicks  : %u elements moved (sum over frames)\n",
             sumKicks);
      printf("island net dx       : %+.3f cells over the run\n", outDX);
      printf("predicted dx if each kick moves 1 cell : %u cells\n", sumKicks);
      printf("per-kick island COM  : %+.3f cells/kick-frame (dxIslandSum)\n",
             outKicks ? outDX / (double)outKicks : 0.0);
    }
  }

  fprintf(stderr,
          "encounters: calls=%lld s2B-pass=%lld pair=%lld self=%lld "
          "collapse=%lld adiah=%lld repel=%lld\n",
          (long long)automaton::enc_calls, (long long)automaton::enc_s2b,
          (long long)automaton::enc_pair, (long long)automaton::enc_self,
          (long long)automaton::enc_collapse,
          (long long)automaton::enc_adiah, (long long)automaton::enc_repel);
  printf("ran %u light frames in %.2fs (%llu ticks)\n", frame, wall, ticks);
  printf("CSV written: %s\n", csvPath);
  return 0;
}