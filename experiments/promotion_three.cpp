// promotion_three.cpp -- minimal production-path test of the D x D intra-island
// promotion (WP3.3).
//
// Three equal-charge S sources (one W-island family, w = 0,1,2) start coincident
// in a small tube.  The seed election makes one K and two D; then the reference
// promotion cascades the two same-island D donors into 2 K + 1 D, while the
// candidate fix (DD_INTRA_ISLAND_FIX) keeps 1 K + 2 D.  Ordinary production
// simulation() path.
//
// Build/run: experiments\build_promotion_three.bat ; build\promotion_three\*.exe

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <array>
#include <cstdio>
#include <string>

using namespace inertia_fixture;
using namespace automaton;

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch)
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
}

int main()
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    constexpr unsigned LX = 21, LY = 5, LZ = 5, W = 3;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    // Emulate the L=9 production structure: one seed family = 3 copies
    // (ISLAND_SIZE = 3).  The tube allocator sets ISLAND_SIZE = 1, so we
    // override it here to make the intra-family election faithful.
    ISLAND_SIZE = 3;
    printf("ISLAND_SIZE=%u (family of %u copies)\n", ISLAND_SIZE, W);
    for (unsigned w = 0; w < W; ++w) placeLayer(w, 2, 2, 2, 0x08);
    replicate();

#ifdef DD_INTRA_ISLAND_FIX
    const char* variant = "fix";
#else
    const char* variant = "ref";
#endif

    unsigned frame = 0, lastK = 0, lastD = 0, lastS = 0;
    for (;;) {
      unsigned nk = 0, nd = 0, ns = 0;
      for (unsigned w = 0; w < W; ++w) {
        const Cell& s = source(w);
        nk += s.kind == SourceKind::K;
        nd += s.kind == SourceKind::D;
        ns += s.kind == SourceKind::S;
      }
      printf("FRAME %u [%s] K=%u D=%u S=%u\n", frame, variant, nk, nd, ns);
      lastK = nk; lastD = nd; lastS = ns;
      if (frame == 8) break;
      while (!simulation()) {}
      ++frame;
    }
    const char* verdict = (lastK == 1 && lastD == 2) ? "1K+2D (island)" :
                          (lastK == 2 && lastD == 1) ? "2K+1D (over-promoted)" :
                          "other";
    printf("VERDICT [%s]: K=%u D=%u S=%u -> %s\n", variant, lastK, lastD, lastS,
           verdict);
    return 0;
  } catch (const std::exception& e) {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
