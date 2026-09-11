// pbsb_two_wide.cpp -- WP4.3 probe: does the seeded polarization broadcast light
// pB/sB on the shells when the tube is wide enough for reconstructPair?
//
// pbsb_two runs a 21x5x5 tube (RMAX = 2); phase_step calls
// reconstructPair(bstamp, RMAX-2, ...), and RMAX-2 = 0 forces pol=(0,0) for every
// cell, so pB/sB can never light there.  The "dormant broadcast" of pbsb_two is
// therefore (at least partly) a short-edge geometry artifact.  This variant takes
// the short edge as argv[1] (default 9 -> RMAX = 4) and reports the pB/sB and the
// maximum |pol_u| seen, so the reconstruction can be tested directly.
//
// Build/run: experiments\build_pbsb_two_wide.bat
//            build\pbsb_two_wide\pbsb_two_wide.exe [shortEdge] [opp]

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include "model/polarization.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>

namespace automaton {
extern long long enc_calls, enc_collapse, enc_adiah, enc_repel, enc_self;
}

using namespace inertia_fixture;
using namespace automaton;

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch, int ax, int ay, int az)
{
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz) {
        Cell& c = getCell(lattice_curr, xx, yy, zz, w);
        c.ch = ch;
        c.a = W_USED;
        c.kind = SourceKind::S;
        c.parent = NO_PARENT;
      }
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz) {
        Cell& c = getCell(lattice_curr, xx, yy, zz, w);
        if (xx == x && yy == y && zz == z) {
          c.r2 = 0u; c.r = 0; c.u = 2048; c.v = 0;
          c.t = 0; c.f = 0; c.bstamp = 0; c.pol_u = c.pol_v = 0;
          c.active = 0; c.phiB = false;
          c.c[0] = c.c[1] = c.c[2] = 0;
        } else if (c.r2 == 0u || c.r == 0 || c.u == 2048) {
          c.r2 = INF_R2; c.r = -1; c.u = 0; c.v = 0;
          c.t = 0; c.f = 0; c.active = 0;
        }
      }
  if (!polarization::seedAxis(w, ax, ay, az))
    throw std::runtime_error("seedAxis failed");
}

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    unsigned shortEdge = (argc >= 2) ? (unsigned)std::stoul(argv[1]) : 9u;
    bool opposite = (argc >= 3 && std::string(argv[2]) == "opp");
    const int s2b = (argc >= 4) ? atoi(argv[3]) : 0;   // 0 = leave the default sieve
    const unsigned lastFrame = (argc >= 5) ? (unsigned)std::stoul(argv[4]) : 16u;
    constexpr unsigned LX = 21;
    const unsigned LY = shortEdge, LZ = shortEdge;
    const unsigned cy = shortEdge / 2, cz = shortEdge / 2;
    if (!tryAllocateTube(LX, LY, LZ, 2u))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    if (s2b > 0) s2b_target = s2b;
    printf("tube %ux%ux%u  RMAX=%u  RMAX-2=%d  s2b_target=%d  frames=%u  %s\n",
           LX, LY, LZ, RMAX, (int)RMAX - 2, (int)s2b_target, lastFrame,
           opposite ? "opposite axes" : "same axes");
    placeLayer(0, 2, cy, cz, 0x08, (int)RMAX, 0, 0);
    placeLayer(1, 4, cy, cz, 0x08,
               opposite ? -(int)RMAX : (int)RMAX, 0, 0);
    replicate();

    unsigned frame = 0;
    for (;;) {
      unsigned nk = 0, nd = 0, ns = 0;
      unsigned pB = 0, sB = 0; int minPu = 0, maxPu = 0;
      for (unsigned w = 0; w < W_USED; ++w) {
        const Cell& s = source(w);
        nk += s.kind == SourceKind::K;
        nd += s.kind == SourceKind::D;
        ns += s.kind == SourceKind::S;
      }
      for (const Cell& c : lattice_curr) {
        if (c.pB) ++pB;
        if (c.sB) ++sB;
        if (c.pol_u < minPu) minPu = c.pol_u;
        if (c.pol_u > maxPu) maxPu = c.pol_u;
      }
      std::set<std::array<unsigned, 3>> sites;
      for (unsigned w = 0; w < W_USED; ++w) sites.insert(lcenters[w]);
      printf("FRAME %u K=%u D=%u S=%u sites=%zu pB=%u sB=%u pol_u=[%d,%d] "
             "coll=%lld ad=%lld rep=%lld\n",
             frame, nk, nd, ns, sites.size(), pB, sB, minPu, maxPu,
             (long long)enc_collapse, (long long)enc_adiah,
             (long long)enc_repel);
      if (frame == lastFrame) break;
      while (!simulation()) {}
      ++frame;
    }
    return 0;
  } catch (const std::exception& e) {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
