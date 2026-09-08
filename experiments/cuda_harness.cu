/*
 * cuda_harness.cu - headless CUDA driver for the parity gate (DESIGN D7).
 * Compiled by nvcc (host part by cl).  Seeds the host model as the CPU probes
 * do (tube or cube, two complementary bubbles at SEP with opposite m/reloc),
 * converts the lattice to CellDevice, uploads it, runs whole light frames on
 * the GPU (FRAME x cudaSimulationStep per frame) and dumps per-frame source
 * centres + kinds to "<csv>.gpu".
 *
 * Usage: cuda_harness LX LY LZ FRAMES [SIEVE] [SCENARIO] [SEP] [csv]
 *   (odd edges >= 5; tube via tryAllocateTube; cube = LX=LY=LZ)
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <chrono>

#include "model/simulation.h"
#include "config.h"
#include "cuda_common.h"
#include "cuda_sim_optimized.h"

std::vector<unsigned int> voxels;   // used by tryAllocate()
Config gConfig;

namespace automaton
{
  bool convol_delay  = false;
  bool diffuse_delay = false;
  bool reloc_delay   = false;
  bool tryEnableCuda()    { return true; }
  void disableCuda()      {}
  bool isCudaEnabled()    { return true; }
}

extern "C" void setCudaConstants(unsigned EL, unsigned W_USED, unsigned RMAX);
extern "C" void setCudaTubeDimensions(unsigned LX, unsigned LY, unsigned LZ);
extern "C" void setCudaSieve(unsigned S);
extern "C" void setCudaSourceCenters(const unsigned* centers, unsigned W);

// Full Cell <-> CellDevice conversion (superset of the bridge version:
// includes bstamp/pol/leader_w/pair_count/reloc so GPU and CPU agree).
static void toDevice(const automaton::Cell& s, CellDevice& d)
{
  d.ch  = (uint8_t)s.ch;
  d.pB  = s.pB ? 1 : 0;
  d.sB  = s.sB ? 1 : 0;
  d.a   = (uint32_t)s.a;
  for (int i = 0; i < 4; ++i) d.x[i] = (uint32_t)s.x[i];
  d.r2 = (uint32_t)s.r2;
  d.r  = (int32_t)s.r;
  d.u  = (int32_t)s.u;
  d.v  = (int32_t)s.v;
  d.bstamp = (uint32_t)s.bstamp;
  d.pol_u  = (int32_t)s.pol_u;
  d.pol_v  = (int32_t)s.pol_v;
  d.active = s.active ? 1u : 0u;
  d.phiB   = s.phiB ? 1u : 0u;
  d.t = (uint32_t)s.t;
  d.f = (uint32_t)s.f;
  for (int i = 0; i < 3; ++i) d.c[i] = (uint32_t)s.c[i];
  d.c[3] = 0;
  d.k   = (uint32_t)s.k;
  d.s2B = s.s2B ? 1 : 0;
  d.kB  = s.kB ? 1 : 0;
  d.bB  = s.bB ? 1 : 0;
  d.homB  = s.homB ? 1 : 0;
  d.cB  = s.cB ? 1 : 0;
  d.gB  = s.gB ? 1 : 0;
  for (int i = 0; i < 3; ++i) d.g[i] = (int32_t)s.g[i];
  d.kind        = (uint8_t)s.kind;
  d.parent      = (uint32_t)s.parent;
  d.spin_target = (int32_t)s.spin_target;
  d.pair_idx    = (uint32_t)s.pair_idx;
  d.leader_w    = (uint32_t)s.leader_w;
  d.pair_count  = (uint32_t)s.pair_count;
  for (int i = 0; i < 3; ++i) d.m[i] = (int32_t)s.m[i];
  for (int i = 0; i < 3; ++i) d.reloc[i] = (int32_t)s.reloc[i];
}

static void toHost(const CellDevice& s, automaton::Cell& d)
{
  d.ch  = (unsigned char)s.ch;
  d.pB  = s.pB != 0;
  d.sB  = s.sB != 0;
  d.a   = s.a;
  for (int i = 0; i < 4; ++i) d.x[i] = s.x[i];
  d.r2 = s.r2;
  d.r  = s.r;
  d.u  = s.u;
  d.v  = s.v;
  d.bstamp = s.bstamp;
  d.pol_u  = s.pol_u;
  d.pol_v  = s.pol_v;
  d.active = s.active != 0;
  d.phiB   = s.phiB != 0;
  d.t = s.t;
  d.f = s.f;
  for (int i = 0; i < 3; ++i) d.c[i] = s.c[i];
  d.k   = s.k;
  d.s2B = s.s2B != 0;
  d.kB  = s.kB != 0;
  d.bB  = s.bB != 0;
  d.homB  = s.homB != 0;
  d.cB  = s.cB != 0;
  d.gB  = s.gB != 0;
  for (int i = 0; i < 3; ++i) d.g[i] = s.g[i];
  d.kind        = (automaton::SourceKind)s.kind;
  d.parent      = s.parent;
  d.spin_target = (int8_t)s.spin_target;
  d.pair_idx    = s.pair_idx;
  d.leader_w    = s.leader_w;
  d.pair_count  = s.pair_count;
  for (int i = 0; i < 3; ++i) d.m[i] = s.m[i];
  for (int i = 0; i < 3; ++i) d.reloc[i] = s.reloc[i];
}

// clear one whole layer, then plant a source centre (scatter_main style).
static void clearLayer(unsigned w)
{
  const int LX = (int)automaton::ELX;
  const int LY = (int)automaton::ELY;
  const int LZ = (int)automaton::ELZ;
  for (int x = 0; x < LX; ++x)
    for (int y = 0; y < LY; ++y)
      for (int z = 0; z < LZ; ++z)
      {
        automaton::Cell* ps[3] = {
          &automaton::getCell(automaton::lattice_curr, x, y, z, (int)w),
          &automaton::getCell(automaton::lattice_draft, x, y, z, (int)w),
          &automaton::getCell(automaton::lattice_partner, x, y, z, (int)w) };
        for (automaton::Cell* p : ps)
        {
          p->r2 = INF_R2; p->r = -1; p->u = 0; p->v = 0;
          p->ch = 0; p->active = 0;
          p->pol_u = p->pol_v = 0; p->bstamp = 0;
          p->m[0] = p->m[1] = p->m[2] = 0;
          p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
          p->kind = automaton::SourceKind::S;
          p->pair_idx = automaton::NO_PAIR;
          p->pair_count = 0;
          p->parent = automaton::NO_PARENT;
          p->leader_w = automaton::NO_LEADER_W;
          p->a = automaton::W_USED;
          p->t = 0; p->f = 0;
          p->pB = p->sB = p->s2B = false;
          p->kB = p->bB = p->homB = p->cB = false;
          p->gB = false;
          for (int i = 0; i < 3; ++i) { p->c[i] = 0; p->g[i] = 0; }
          p->spin_target = 0;
        }
      }
}

static void plantSource(unsigned w, int x, int y, int z,
                        unsigned char chW, int mx, int my, int mz)
{
  automaton::Cell* ps[3] = {
    &automaton::getCell(automaton::lattice_curr, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_draft, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_partner, x, y, z, (int)w) };
  for (automaton::Cell* p : ps)
  {
    p->r2 = 0; p->r = 0; p->u = 2048; p->v = 0;
    p->ch = chW;
    p->m[0] = mx; p->m[1] = my; p->m[2] = mz;
    p->reloc[0] = mx; p->reloc[1] = my; p->reloc[2] = mz;
    p->t = 0; p->f = 0;
    p->kind = automaton::SourceKind::S;
    p->a = automaton::W_USED;
    p->leader_w = automaton::NO_LEADER_W;
  }
  automaton::lcenters[w][0] = (unsigned)x;
  automaton::lcenters[w][1] = (unsigned)y;
  automaton::lcenters[w][2] = (unsigned)z;
}


static void seedIsland(unsigned n_i, unsigned n_pi, int SIEVE);

int main(int argc, char** argv)
{
  const bool cpuMode = (argc > 1) && (strcmp(argv[1], "cpu") == 0);
  if (argc < (cpuMode ? 6 : 5))
  {
    fprintf(stderr,
            "usage: cuda_harness [cpu] LX LY LZ FRAMES [SIEVE] [SCENARIO] [SEP] [csv]\n");
    return 1;
  }
  unsigned LX = (unsigned)atoi(argv[cpuMode ? 2 : 1]);
  unsigned LY = (unsigned)atoi(argv[cpuMode ? 3 : 2]);
  unsigned LZ = (unsigned)atoi(argv[cpuMode ? 4 : 3]);
  unsigned FRAMES = (unsigned)atoi(argv[cpuMode ? 5 : 4]);
  int      SIEVE = (argc > (cpuMode ? 6 : 5)) ? atoi(argv[cpuMode ? 6 : 5]) : 64;
  int      SCEN  = (argc > (cpuMode ? 7 : 6)) ? atoi(argv[cpuMode ? 7 : 6]) : 7;
  unsigned SEP   = (argc > (cpuMode ? 8 : 7)) ? (unsigned)atoi(argv[cpuMode ? 8 : 7]) : 4u;
  const char* csvPath = (argc > (cpuMode ? 9 : 8)) ? argv[cpuMode ? 9 : 8] : "gpu_run.csv";

  if (LX < 5 || LY < 5 || LZ < 5 || (LX % 2) == 0 ||
      (LY % 2) == 0 || (LZ % 2) == 0)
  {
    fprintf(stderr, "edges must be odd and >= 5\n");
    return 2;
  }
  if (SIEVE < 1) SIEVE = 1;
  // Optional island+propeller mode (ISLAND=1, ISLAND_NI, ISLAND_NPI).
  const bool island = getenv("ISLAND") != nullptr;
  unsigned n_i = 0u, n_pi = 0u;
  if (island)
  {
    n_i = (unsigned)atoi(getenv("ISLAND_NI") ? getenv("ISLAND_NI") : "6");
    n_pi = (unsigned)atoi(getenv("ISLAND_NPI") ? getenv("ISLAND_NPI") : "3");
    if (n_i < 2 || n_pi > 8) { fprintf(stderr, "bad ISLAND_NI/NPI\n"); return 2; }
  }
  const unsigned W = island ? (n_i + 2u * n_pi) : 2u;

  printf("=== cuda_harness: %u x %u x %u, W=2, frames=%u sieve=%d scenario=%d ===\n",
         LX, LY, LZ, FRAMES, SIEVE, SCEN);

  automaton::s2b_target = SIEVE;
  if (!automaton::tryAllocateTube(LX, LY, LZ, W))
  {
    fprintf(stderr, "allocation failed: %s\n",
            automaton::lastAllocationError.c_str());
    return 3;
  }
  for (int step = 0; step <= 7; ++step)
    automaton::initSimulation(step);

  if (island)
  {
    seedIsland(n_i, n_pi, SIEVE);
  }
  else
  {
    // Two complementary bubbles around the middle of the long axis, SEP apart.
    const int cy = (int)((automaton::ELY - 1u) / 2u);
    const int cz = (int)((automaton::ELZ - 1u) / 2u);
    const int cx = (int)(automaton::ELX / 2u);
    const unsigned char chA = 0x08;          // -L (Orbis)
    const unsigned char chB = (unsigned char)(~chA & 0x3F); // complementary +L
    clearLayer(0);
    clearLayer(1);
    plantSource(0, cx - (int)(SEP / 2u), cy, cz, chA, +1, 0, 0);
    plantSource(1, cx + (int)(SEP / 2u), cy, cz, chB, -1, 0, 0);
  }

  FILE* f = fopen(csvPath, "w");
  if (f)
  {
    if (island)
      fprintf(f, "P %u %u %u %u %u %u %u\n", LX, LY, LZ,
              (unsigned)automaton::RMAX, W, n_i, n_pi);
    else
      fprintf(f, "frame,x0,y0,z0,x1,y1,z1,k0,k1\n");
  }

  if (cpuMode)
  {
    // Reference CPU run: full automaton::simulation() (per tick), dump every
    // completed light frame - same seed and CSV schema as the GPU path.
    const auto t0 = std::chrono::steady_clock::now();
    unsigned frame = 0;
    unsigned long long ticks = 0;
    while (frame < FRAMES)
    {
      const bool nf = automaton::simulation();
      ++ticks;
      if (!nf) continue;
      ++frame;
      const automaton::Cell& c0 = automaton::getCell(
          automaton::lattice_curr,
          (int)automaton::lcenters[0][0], (int)automaton::lcenters[0][1],
          (int)automaton::lcenters[0][2], 0);
      const automaton::Cell& c1 = automaton::getCell(
          automaton::lattice_curr,
          (int)automaton::lcenters[1][0], (int)automaton::lcenters[1][1],
          (int)automaton::lcenters[1][2], 1);
      if (f)
      {
        if (island)
        {
          for (unsigned w = 0; w < W; ++w)
          {
            const automaton::Cell& cc = automaton::getCell(
                automaton::lattice_curr,
                (int)automaton::lcenters[w][0], (int)automaton::lcenters[w][1],
                (int)automaton::lcenters[w][2], (int)w);
            fprintf(f, "F %u\n", frame);
            fprintf(f, "%u %u %u %u %d %d %d %d %d %d %d %u %d %d %d %d\n",
                    w, automaton::lcenters[w][0], automaton::lcenters[w][1],
                    automaton::lcenters[w][2], (int)cc.kind,
                    cc.m[0], cc.m[1], cc.m[2],
                    cc.reloc[0], cc.reloc[1], cc.reloc[2],
                    cc.t, cc.pB ? 1 : 0, cc.sB ? 1 : 0,
                    (cc.pair_idx != automaton::NO_PAIR) ? 1 : 0,
                    cc.a == automaton::W_USED ? -1 : (int)cc.a);
          }
        }
        else
          fprintf(f, "%u,%u,%u,%u,%u,%u,%u,%d,%d\n",
                  frame,
                  automaton::lcenters[0][0], automaton::lcenters[0][1],
                  automaton::lcenters[0][2],
                  automaton::lcenters[1][0], automaton::lcenters[1][1],
                  automaton::lcenters[1][2],
                  (int)c0.kind, (int)c1.kind);
      }
    }
    const double wall = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - t0).count();
    if (f) fclose(f);
    printf("CPU ran %u light frames in %.2fs (%llu ticks)\n",
           FRAMES, wall, ticks);
    printf("CPU CSV: %s\n", csvPath);
    return 0;
  }

  if (!isCudaAvailable())
  {
    fprintf(stderr, "no CUDA device\n");
    return 4;
  }
  if (!initCudaSimulation(automaton::EL, automaton::W_USED))
  {
    fprintf(stderr, "initCudaSimulation failed\n");
    return 5;
  }
  setCudaConstants(automaton::EL, automaton::W_USED, automaton::RMAX);
  setCudaTubeDimensions(LX, LY, LZ);
  setCudaSieve((unsigned)SIEVE);

  const size_t total = (size_t)automaton::ELX * automaton::ELY *
                       automaton::ELZ * automaton::W_USED;
  std::vector<CellDevice> dev(total);
  for (size_t i = 0; i < total; ++i)
    toDevice(automaton::lattice_curr[i], dev[i]);
  if (!uploadLatticeToCuda(dev.data(), total))
  {
    fprintf(stderr, "upload failed\n");
    return 6;
  }

  const auto t0 = std::chrono::steady_clock::now();
  unsigned long long pulse = 0;
  for (unsigned fr = 0; fr < FRAMES; ++fr)
  {
    for (unsigned tick = 0; tick < automaton::FRAME; ++tick)
    {
      cudaSimulationStep(
          automaton::ENCOUNTER, automaton::GSLOT_Z,
          automaton::SLOT1, automaton::SLOT2, automaton::SLOT3,
          automaton::SLOT4, automaton::DIFFUSION, automaton::SLOT5,
          automaton::SLOT6, automaton::SLOT7, automaton::SLOT8,
          automaton::RELOC, automaton::REISSUE, automaton::FLOOD,
          automaton::FRAME, automaton::RMAX, SCEN, pulse + tick);
    }
    pulse += automaton::FRAME;

    // Download and record the two source centres + kinds.
    if (!downloadLatticeFromCuda(dev.data(), total))
    {
      fprintf(stderr, "download failed at frame %u\n", fr);
      break;
    }
    for (unsigned w = 0; w < W; ++w)
    {
      const auto& p = automaton::lcenters[w];
      const size_t sp = ((size_t)p[0] * automaton::ELY + p[1]) *
                        automaton::ELZ + p[2];
      toHost(dev[sp * automaton::W_USED + w],
             automaton::lattice_curr[sp * automaton::W_USED + w]);
    }
    if (f)
    {
      if (island)
      {
        for (unsigned w = 0; w < W; ++w)
        {
          const automaton::Cell& cc = automaton::getCell(
              automaton::lattice_curr,
              (int)automaton::lcenters[w][0], (int)automaton::lcenters[w][1],
              (int)automaton::lcenters[w][2], (int)w);
          fprintf(f, "F %u\n", fr + 1);
          fprintf(f, "%u %u %u %u %d %d %d %d %d %d %d %u %d %d %d %d\n",
                  w, automaton::lcenters[w][0], automaton::lcenters[w][1],
                  automaton::lcenters[w][2], (int)cc.kind,
                  cc.m[0], cc.m[1], cc.m[2],
                  cc.reloc[0], cc.reloc[1], cc.reloc[2],
                  cc.t, cc.pB ? 1 : 0, cc.sB ? 1 : 0,
                  (cc.pair_idx != automaton::NO_PAIR) ? 1 : 0,
                  cc.a == automaton::W_USED ? -1 : (int)cc.a);
        }
      }
      else
      {
        const automaton::Cell& c0 = automaton::getCell(
            automaton::lattice_curr,
            (int)automaton::lcenters[0][0], (int)automaton::lcenters[0][1],
            (int)automaton::lcenters[0][2], 0);
        const automaton::Cell& c1 = automaton::getCell(
            automaton::lattice_curr,
            (int)automaton::lcenters[1][0], (int)automaton::lcenters[1][1],
            (int)automaton::lcenters[1][2], 1);
        fprintf(f, "%u,%u,%u,%u,%u,%u,%u,%d,%d\n",
                fr + 1,
                automaton::lcenters[0][0], automaton::lcenters[0][1],
                automaton::lcenters[0][2],
                automaton::lcenters[1][0], automaton::lcenters[1][1],
                automaton::lcenters[1][2],
                (int)c0.kind, (int)c1.kind);
      }
    }
  }
  const double wall = std::chrono::duration<double>(
      std::chrono::steady_clock::now() - t0).count();
  if (f) fclose(f);
  printf("ran %u light frames in %.2fs\n", FRAMES, wall);
  printf("GPU CSV: %s\n", csvPath);
  return 0;
}


// ---------------------------------------------------------------------------
// Island + propellers seed (mirrors inertia_probe electron composition).
// ---------------------------------------------------------------------------
static unsigned char islElectron() { return 0x08; }  // -L (Orbis)
static unsigned char islPlus()     { return 0x37; }  // +L complement of -L

static void plantIslandBubble(unsigned w, int x, int y, int z,
                              unsigned char chW, automaton::SourceKind kind)
{
  automaton::Cell* ps[3] = {
    &automaton::getCell(automaton::lattice_curr, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_draft, x, y, z, (int)w),
    &automaton::getCell(automaton::lattice_partner, x, y, z, (int)w) };
  for (automaton::Cell* p : ps)
  {
    p->r2 = 0; p->r = 0; p->u = 2048; p->v = 0; p->ch = chW;
    p->m[0] = p->m[1] = p->m[2] = 0;
    p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
    p->t = 0; p->f = 0;
    p->kind = kind;
    p->a = 0u;                       // island member: leader 0 / affinity 0
    p->leader_w = 0u;
  }
  automaton::lcenters[w][0] = (unsigned)x;
  automaton::lcenters[w][1] = (unsigned)y;
  automaton::lcenters[w][2] = (unsigned)z;
}

static void seedIsland(unsigned n_i, unsigned n_pi, int SIEVE)
{
  (void)SIEVE;
  const unsigned XQ = automaton::ELX / 8u;
  const unsigned cy = (automaton::ELY - 1u) / 2u;
  const unsigned cz = (automaton::ELZ - 1u) / 2u;
  const int xI = (int)(XQ * 3u / 4u);
  const int yI = (int)cy, zI = (int)cz;
  if (xI < 2)
  {
    fprintf(stderr, "seedIsland: need LX/8 >= 3 (first eighth too small)\n");
    exit(9);
  }
  // Electron composition: -L singles (incl. K chief) + nu_e dressing pairs.
  const unsigned pairs   = n_i / 3u;
  const unsigned singles = n_i - 2u * pairs;
  fprintf(stderr, "island seed: -L singles=%u (chief K) nu_e pairs=%u\n",
          singles, pairs);
  for (unsigned i = 0; i < n_i; ++i)
  {
    unsigned char chW;
    if (i < singles) chW = islElectron();
    else chW = ((i - singles) & 1u) ? islPlus() : islElectron();
    clearLayer(i);
    plantIslandBubble(i, xI, yI, zI, chW,
                      (i == 0) ? automaton::SourceKind::K
                               : automaton::SourceKind::S);
  }
  for (unsigned p = 0; p < n_pi; ++p)
  {
    const int span = xI - 1;
    const int xp = 1 + (int)(((unsigned)span * p) / n_pi);
    int yp = (int)cy + (int)(p % 3u) - 1;
    int zp = (int)cz + (int)((p / 3u) % 3u) - 1;
    const unsigned wa = n_i + 2 * p;
    const unsigned wb = n_i + 2 * p + 1;
    clearLayer(wa);
    clearLayer(wb);
    unsigned char chA = (p & 1u) ? islElectron() : islPlus();
    unsigned char chB = (unsigned char)(~chA & 0x3F);
    plantIslandBubble(wa, xp, yp, zp, chA, automaton::SourceKind::S);
    plantIslandBubble(wb, xp, yp, zp, chB, automaton::SourceKind::S);
    // Propellers are orphans (no island affinity).
    for (unsigned h = 0; h < 2; ++h)
    {
      const unsigned wh = wa + h;
      automaton::Cell* ps[3] = {
        &automaton::getCell(automaton::lattice_curr, xp, yp, zp, (int)wh),
        &automaton::getCell(automaton::lattice_draft, xp, yp, zp, (int)wh),
        &automaton::getCell(automaton::lattice_partner, xp, yp, zp, (int)wh) };
      for (automaton::Cell* p : ps) { p->a = automaton::W_USED; p->leader_w = automaton::NO_LEADER_W; }
    }
  }
}
