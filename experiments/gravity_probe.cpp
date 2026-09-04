/*
 * gravity_probe.cpp - S0 pilot: two-aggregate free-fall measurement on the
 * existing cubic lattice (design: experiments/gravity_probe_DESIGN.md).
 *
 * Two complementary single-source bubbles are planted SEP cells apart on the
 * x axis at rest (mag = 0) and left to evolve.  Every completed light frame
 * we record, READ-ONLY over lattice_curr:
 *   - separation d(t) between the two source centres (shortest torus dist.);
 *   - centred second difference dd(t) as a raw acceleration proxy;
 *   - operational mass of each centre (cells with affinity a == w);
 *   - encounter diagnostics (enc_calls / enc_s2b / enc_pair) per frame.
 *
 * Usage:
 *   gravity_probe EL SEP FRAMES SIEVE mag mode csv
 *     EL    odd in [3..511]  (default 11)
 *     SEP   initial separation along x (default 5)
 *     FRAMES light frames   (default 120)
 *     SIEVE sieve modulus   (default 16384)
 *     mag   initial |momentum|, 0 = at rest (default 0)
 *     mode  "attract" (0x00 vs 0x3F) or "repel" (0x00 vs 0x00) (default attract)
 *     csv   output path (default gravity_run.csv)
 *
 *   gravity_probe tube LX LY LZ SEP FRAMES SIEVE mag mode csv
 *     rectangular tube (odd edges >= 5, unequal): bubbles planted SEP apart
 *     on the long axis, per-axis wrapping; goes through tryAllocateTube.
 *
 * Output: per-frame CSV + terminal summary (G0 pipeline report).
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>

#include "model/simulation.h"
#include "config.h"

// ---------------------------------------------------------------------------
// Link stubs (same role as alpha_probe.cpp).
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
// placeSource: reset one W-layer to "unreached" and plant a source centre.
// ---------------------------------------------------------------------------
static void placeSource(unsigned w, unsigned x, unsigned y, unsigned z,
                        int mx, int my, int mz, unsigned char chWord)
{
  const int ELXi = (int)automaton::ELX;
  const int ELYi = (int)automaton::ELY;
  const int ELZi = (int)automaton::ELZ;

  for (int a = 0; a < ELXi; ++a)
    for (int b = 0; b < ELYi; ++b)
      for (int c = 0; c < ELZi; ++c)
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
          p->kB = p->bB = p->hB = p->cB = false;
          p->gB = false;
          p->c[0] = p->c[1] = p->c[2] = 0;
          p->g[0] = p->g[1] = p->g[2] = 0;
          p->spin_target = 0;
        }
      }

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
    p->m[0] = mx; p->m[1] = my; p->m[2] = mz;
    p->reloc[0] = mx; p->reloc[1] = my; p->reloc[2] = mz;
    p->t = 0; p->f = 0;
  }

  automaton::lcenters[w][0] = x;
  automaton::lcenters[w][1] = y;
  automaton::lcenters[w][2] = z;
  automaton::trackCenter(x, y, z, w);
}

// ---------------------------------------------------------------------------
// Per-aggregate operational mass: cells whose affinity labels island w.
// ---------------------------------------------------------------------------
static long long islandMass(unsigned w)
{
  long long m = 0;
  for (size_t i = 0; i < automaton::lattice_curr.size(); ++i)
  {
    const automaton::Cell& c = automaton::lattice_curr[i];
    if (c.a != automaton::W_USED && c.a == w) ++m;
  }
  return m;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Cube mode (legacy):
  //   gravity_probe EL SEP FRAMES SIEVE mag mode csv
  // Tube mode (unequal edges, per-axis geometry):
  //   gravity_probe tube LX LY LZ SEP FRAMES SIEVE mag mode csv
  const bool tubeMode = (argc > 1) && (strcmp(argv[1], "tube") == 0);

  setvbuf(stdout, nullptr, _IONBF, 0);   // crash-location tracing (probe only)

  unsigned EL_in  = 0u;
  unsigned LX     = 0u, LY = 0u, LZ = 0u;
  unsigned SEP    = 5u;
  unsigned FRAMES = 120u;
  int      SIEVE  = 16384;
  int      mag    = 0;
  bool     repel  = false;
  const char* csvPath = "gravity_run.csv";

  if (!tubeMode)
  {
    EL_in   = (argc > 1) ? (unsigned)atoi(argv[1]) : 11u;
    SEP     = (argc > 2) ? (unsigned)atoi(argv[2]) : 5u;
    FRAMES  = (argc > 3) ? (unsigned)atoi(argv[3]) : 120u;
    SIEVE   = (argc > 4) ? atoi(argv[4]) : 16384;
    mag     = (argc > 5) ? atoi(argv[5]) : 0;
    repel   = (argc > 6) && (strcmp(argv[6], "repel") == 0);
    csvPath = (argc > 7) ? argv[7] : "gravity_run.csv";
  }
  else
  {
    if (argc < 9)
    {
      fprintf(stderr,
              "tube usage: gravity_probe tube LX LY LZ SEP FRAMES SIEVE mag [mode] [csv]\n");
      return 1;
    }
    LX      = (unsigned)atoi(argv[2]);
    LY      = (unsigned)atoi(argv[3]);
    LZ      = (unsigned)atoi(argv[4]);
    SEP     = (unsigned)atoi(argv[5]);
    FRAMES  = (unsigned)atoi(argv[6]);
    SIEVE   = atoi(argv[7]);
    mag     = atoi(argv[8]);
    repel   = (argc > 9) && (strcmp(argv[9], "repel") == 0);
    csvPath = (argc > 10) ? argv[10] : "gravity_tube.csv";
  }

  if (!tubeMode && (EL_in < 3 || EL_in > 511 || (EL_in % 2) == 0))
  {
    fprintf(stderr, "EL must be an odd value in [3,511]\n");
    return 1;
  }
  if (SEP < 2) SEP = 2;
  if (SIEVE < 1) SIEVE = 1;
  if (mag < 0) mag = 0;

  const unsigned W_in = 2u;

  // Memory guard (~64 bytes/cell, W=2).
  size_t estBytes = 0;
  if (tubeMode)
  {
    estBytes = (size_t)LX * LY * LZ * W_in * 64u;
    if (LX < 5 || LY < 5 || LZ < 5 || (LX % 2) == 0 ||
        (LY % 2) == 0 || (LZ % 2) == 0)
    {
      fprintf(stderr, "tube edges must be odd and >= 5 (got %u x %u x %u)\n",
              LX, LY, LZ);
      return 1;
    }
  }
  else
  {
    estBytes = (size_t)EL_in * EL_in * EL_in * W_in * 64u;
  }
  if (estBytes > (size_t)8ull * 1024 * 1024 * 1024)
  {
    fprintf(stderr, "geometry needs ~%.1f GB; reduce the edges.\n",
            (double)estBytes / (double)(1024 * 1024 * 1024));
    return 1;
  }

  if (tubeMode)
  {
    printf("=== gravity probe S0: tube geometry (unequal edges) ===\n");
    printf("tube ELX=%u ELY=%u ELZ=%u W_USED=%u SEP=%u frames=%u sieve=%d mag=%d mode=%s\n",
           LX, LY, LZ, W_in, SEP, FRAMES, SIEVE, mag,
           repel ? "repel" : "attract");
  }
  else
  {
    printf("=== gravity probe S0: two aggregates at rest ===\n");
    printf("EL=%u W_USED=%u RMAX=%u SEP=%u frames=%u sieve=%d mag=%d mode=%s\n",
           EL_in, W_in, EL_in / 2u, SEP, FRAMES, SIEVE, mag,
           repel ? "repel" : "attract");
  }

  automaton::s2b_target = SIEVE;

  bool allocOk = false;
  if (tubeMode)
    allocOk = automaton::tryAllocateTube(LX, LY, LZ, W_in);
  else
  {
    automaton::calculateParameters(EL_in, W_in);
    allocOk = automaton::tryAllocate((int)EL_in, (int)W_in);
  }
  if (!allocOk)
  {
    fprintf(stderr, "allocation failed: %s\n",
            automaton::lastAllocationError.c_str());
    return 2;
  }

  for (int step = 0; step <= 7; ++step)
    automaton::initSimulation(step);

  // Placement: centred on the x axis, on the per-axis midpoints of the
  // short sides (they coincide with CENTER in the cube mode).
  const unsigned off = SEP / 2u;
  const unsigned cx = (automaton::ELX - 1u) / 2u;
  const unsigned cy = (automaton::ELY - 1u) / 2u;
  const unsigned cz = (automaton::ELZ - 1u) / 2u;
  if (off > cx || cx + off >= automaton::ELX)
  {
    fprintf(stderr,
            "SEP=%u too large for the x extent (centre %u): would underflow/overflow.\n",
            SEP, cx);
    return 3;
  }
  const unsigned char chA = 0x00;
  const unsigned char chB = repel ? 0x00 : 0x3F;

  placeSource(0, cx - off, cy, cz,  mag, 0, 0, chA);  // left
  placeSource(1, cx + off, cy, cz, -mag, 0, 0, chB);  // right

  const auto t0 = std::chrono::steady_clock::now();

  struct Row
  {
    unsigned frame;
    unsigned long long tick; // total automaton ticks since start at this row
    double d;
    double dd;          // second difference of d (raw acceleration proxy)
    long long m0, m1;
    long long calls, s2b, pairs;
  };
  std::vector<Row> rows;
  rows.reserve(FRAMES);

  double dPrev = -1.0, dPrev2 = -1.0;
  long long prevCalls = 0, prevS2b = 0, prevPairs = 0;
  unsigned frame = 0;
  unsigned long long ticks = 0;

  while (frame < FRAMES)
  {
    const bool newFrame = automaton::simulation();
    ++ticks;
    if (!newFrame) continue;

    ++frame;
    const auto& p0 = automaton::lcenters[0];
    const auto& p1 = automaton::lcenters[1];
    int dx = (int)p1[0] - (int)p0[0];
    int dy = (int)p1[1] - (int)p0[1];
    int dz = (int)p1[2] - (int)p0[2];
    // Shortest torus distance, per-axis wrap (equal edges reduce to EL/2).
    const int hx = (int)(automaton::ELX / 2u);
    const int hy = (int)(automaton::ELY / 2u);
    const int hz = (int)(automaton::ELZ / 2u);
    if (dx >  hx) dx -= (int)automaton::ELX; else if (dx < -hx) dx += (int)automaton::ELX;
    if (dy >  hy) dy -= (int)automaton::ELY; else if (dy < -hy) dy += (int)automaton::ELY;
    if (dz >  hz) dz -= (int)automaton::ELZ; else if (dz < -hz) dz += (int)automaton::ELZ;
    const double d = std::sqrt((double)(dx*dx + dy*dy + dz*dz));

    double dd = 0.0;
    if (dPrev2 >= 0.0)
      dd = d - 2.0 * dPrev + dPrev2;

    const long long calls = automaton::enc_calls - prevCalls;
    const long long s2b   = automaton::enc_s2b   - prevS2b;
    const long long pairs = automaton::enc_pair  - prevPairs;
    prevCalls = automaton::enc_calls;
    prevS2b   = automaton::enc_s2b;
    prevPairs = automaton::enc_pair;

    rows.push_back({ frame, ticks, d, dd, islandMass(0), islandMass(1),
                     calls, s2b, pairs });

    dPrev2 = dPrev;
    dPrev = d;
  }

  const double wall = std::chrono::duration<double>(
      std::chrono::steady_clock::now() - t0).count();

  // ---- summary ----------------------------------------------------------
  double dmin = 1e30, dmax = -1e30;
  unsigned iMin = 0;
  long long totalCalls = 0, totalS2b = 0, totalPairs = 0;
  unsigned approachFrames = 0;   // frames where d strictly decreased vs prior
  for (size_t i = 0; i < rows.size(); ++i)
  {
    if (rows[i].d < dmin) { dmin = rows[i].d; iMin = (unsigned)i; }
    if (rows[i].d > dmax) dmax = rows[i].d;
    if (i > 0 && rows[i].d < rows[i - 1].d) ++approachFrames;
    totalCalls += rows[i].calls;
    totalS2b   += rows[i].s2b;
    totalPairs += rows[i].pairs;
  }

  FILE* f = fopen(csvPath, "w");
  if (f)
  {
    fprintf(f, "frame,tick,d,dd,m0,m1,calls,s2b,pairs\n");
    for (const Row& r : rows)
      fprintf(f, "%u,%llu,%.6f,%.6f,%lld,%lld,%lld,%lld,%lld\n",
              r.frame, r.tick, r.d, r.dd, r.m0, r.m1, r.calls, r.s2b, r.pairs);
    fclose(f);
    printf("CSV written: %s\n", csvPath);
  }
  else
    fprintf(stderr, "warning: cannot open %s\n", csvPath);

  printf("ran %u light frames in %.2fs (%llu ticks)\n", frame, wall, ticks);
  printf("encounters: calls=%lld s2B=%lld pairs=%lld (sieve S=%d)\n",
         totalCalls, totalS2b, totalPairs, SIEVE);
  printf("separation: d0=%.3f dmin=%.3f@frame=%u dmax=%.3f dfinal=%.3f\n",
         rows.empty() ? -1.0 : rows[0].d, dmin, iMin, dmax,
         rows.empty() ? -1.0 : rows.back().d);
  if (!rows.empty())
  {
    printf("approach frames=%u/%-zu (%.1f%%)\n",
           approachFrames, rows.size(),
           100.0 * (double)approachFrames / (double)rows.size());
    // G0 pipeline check: measurement reproducible & non-degenerate.
    if (dmax - dmin < 1e-9 && totalCalls == 0)
      printf("G0 status: STATIC-NO-CONTACT (pipeline ok, no encounter fired)\n");
    else
      printf("G0 status: MEASURED (d span %.3f, encounters fired)\n",
             dmax - dmin);
  }
  return 0;
}
