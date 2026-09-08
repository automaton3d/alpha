// pbsb_two.cpp — minimal production-path check of the pB/sB preemption.
//
// Two equal-charge S clouds (tube 21x5x5, W=2, charge 0x08) at torus
// distance 2 (<= 2*RMAX), each with an initial polarisation axis seeded via
// polarization::seedAxis so phase_step reconstructs pB/sB on the shells.
// The design note (PBSB_ISLANDS.md) predicts that the equal-charge identity
// path (chiefContact) still preempts the EM channel, i.e. the two clouds
// merge into 1 K + 1 D even though pB/sB are active.  This run records
// roles, centres, per-layer pB/sB counts and the encounter counters to test
// that prediction.
//
// Build/run: experiments\build_pbsb_two.bat ; build\pbsb_two\pbsb_two.exe

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include "model/polarization.h"
#include <array>
#include <cstdio>
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
        c.a = W_USED;                 // orphan singletons
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
    bool opposite = (argc >= 2 && std::string(argv[1]) == "opp");
    constexpr unsigned LX = 21, LY = 5, LZ = 5;
    if (!tryAllocateTube(LX, LY, LZ, 2u))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    placeLayer(0, 2, 2, 2, 0x08, (int)RMAX, 0, 0);
    placeLayer(1, 4, 2, 2, 0x08, opposite ? -(int)RMAX : (int)RMAX, 0, 0);
    replicate();

    FILE* out = fopen("build/pbsb_two/trace.csv", "w");
    if (!out) throw std::runtime_error("cannot open output");
    setvbuf(out, nullptr, _IONBF, 0);
    fprintf(out, "frame,K,D,S,centers,dx,pB0,pB1,sB0,sB1,collapse,adiah,repel\n");

    unsigned frame = 0;
    for (;;) {
      unsigned nk = 0, nd = 0, ns = 0;
      unsigned pB[2] = {0, 0}, sB[2] = {0, 0};
      for (unsigned w = 0; w < W_USED; ++w) {
        const Cell& s = source(w);
        nk += s.kind == SourceKind::K;
        nd += s.kind == SourceKind::D;
        ns += s.kind == SourceKind::S;
      }
      for (const Cell& c : lattice_curr)
        if (c.pB) ++pB[c.x[3]];
      for (const Cell& c : lattice_curr)
        if (c.sB) ++sB[c.x[3]];
      std::set<std::array<unsigned, 3>> sites;
      for (unsigned w = 0; w < W_USED; ++w) sites.insert(lcenters[w]);
      const int dx = (int)lcenters[1][0] - (int)lcenters[0][0];
      fprintf(out, "%u,%u,%u,%u,%zu,%d,%u,%u,%u,%u,%lld,%lld,%lld\n",
              frame, nk, nd, ns, sites.size(), dx, pB[0], pB[1], sB[0], sB[1],
              (long long)enc_collapse, (long long)enc_adiah,
              (long long)enc_repel);
      printf("FRAME %u K=%u D=%u S=%u sites=%zu dx=%d pB=(%u,%u) "
             "sB=(%u,%u) coll=%lld ad=%lld rep=%lld\n",
             frame, nk, nd, ns, sites.size(), dx, pB[0], pB[1], sB[0], sB[1],
             (long long)enc_collapse, (long long)enc_adiah,
             (long long)enc_repel);
      if (frame == 16) break;
      while (!simulation()) {}
      ++frame;
    }
    fclose(out);
    return 0;
  } catch (const std::exception& e) {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
