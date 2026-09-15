// spacing_probe.cpp -- item 1: can the model's OWN channel hold same-charge
// constituents apart, so that the D x D promotion (T2) never fires?
//
// The `81 x 3` candidate state exists only with a hard core keyed on the seed
// family (w/3, `EXCLUSION_FSM`), which is a stand-in: it blocks cross-family
// contacts by fiat.  A mechanism that spaces same-charge delegates beyond the
// contact range would do the same job with the model's own ingredients, and
// without any rule containing L/3.
//
// What is already excluded (measured elsewhere): the mediated R2/R1 channel is
// sign-blind between distinct families (PHOTON_MEDIATION_FAR.md), and in a cubic
// cell the contact graph at the turnaround is complete (GEOMETRIC_QUANTUM.md).
// What remains is the ELECTRIC branch of `encounter`, live only when `pB/sB` are
// live -- and the polarization cannot bootstrap from the symmetric seed
// (SEED_ASYMMETRY.md).  This probe therefore asks the sharp question: if the
// bootstrap price is PAID (a candidate macro, as in the pbsb probes), does the
// electric channel separate two same-charge delegates that start inside the
// contact range?  Either answer is decisive for item 1.
//
// Configuration: tube 27x5x5 (RMAX = 2, contact range 2*RMAX = 4), W = 2:
//   layer 0: chief  K, charge 0x08, at x = 3
//   layer 1: delegate D, charge 0x08, parent 0, at x = 6 (d = 3 <= 4, so the
//            reference build MUST promote one of them through T2)
//
// Arms (build flags; see build_spacing_probe.bat):
//   spacing_ref    : no macro        -> the no-channel baseline
//   spacing_pol    : /D SPACING_POL   -> polarization::seedAxis(+x) on every layer
//   spacing_pol_em : + /D EM_FIRST_FSM -> electric decision before the identity merge
//
// Build: experiments\build_spacing_probe.bat
// Run:   build\spacing_probe\spacing_probe_<ref|pol|polem>.exe [frames]

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include "model/polarization.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

using namespace inertia_fixture;
using namespace automaton;

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch)
{
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz)
      {
        Cell& c = getCell(lattice_curr, xx, yy, zz, w);
        c = Cell{};
        c.w = w; c.ch = ch; c.a = W_USED; c.kind = SourceKind::S;
        c.parent = NO_PARENT; c.leader_w = NO_LEADER_W;
        c.x[0] = xx; c.x[1] = yy; c.x[2] = zz; c.x[3] = w;
        c.r2 = INF_R2; c.r = -1;
      }
  Cell& s = getCell(lattice_curr, x, y, z, w);
  s.r2 = 0u; s.r = 0; s.u = 2048;
}

static void markChief(unsigned w, unsigned char ch)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::K; c.parent = NO_PARENT;
    c.leader_w = (WIndex)w; c.a = w; c.ch = ch;
  }
}

static void markDelegate(unsigned w, unsigned chief, unsigned char ch)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::D; c.parent = chief; c.leader_w = (WIndex)chief;
    c.a = chief; c.ch = ch;
  }
}

static unsigned pairDistance()
{
  unsigned d = 0;
  const unsigned edges[3] = { ELX, ELY, ELZ };
  for (int k = 0; k < 3; ++k)
    d += (unsigned)std::abs(wrappedDelta((int)lcenters[0][k],
                                         (int)lcenters[1][k], edges[k]));
  return d;
}

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    const unsigned frames = (argc > 1) ? (unsigned)atoi(argv[1]) : 60u;
    const unsigned dD = (argc > 2) ? (unsigned)atoi(argv[2]) : 3u;   // D1-D2 gap
    constexpr unsigned LX = 27, LY = 5, LZ = 5, W = 3;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;
    // A three-copy family, as in the seed: one K and TWO delegates, so that a
    // D x D pair exists and T2 can fire.  dD is the D1-D2 gap; the K sits three
    // cells from D1 in every configuration.
    const unsigned xK = 3, xD1 = 6, xD2 = 6 + dD;
    placeLayer(0, xK % LX, y, z, 0x08);
    placeLayer(1, xD1 % LX, y, z, 0x08);
    placeLayer(2, xD2 % LX, y, z, 0x08);
    replicate();
    markChief(0, 0x08);
    markDelegate(1, 0, 0x08);
    markDelegate(2, 0, 0x08);
    replicate();

#if defined(SPACING_POL)
    for (unsigned w = 0; w < W; ++w)
      if (!polarization::seedAxis(w, 1, 0, 0))
        fprintf(stderr, "warning: seedAxis(w=%u) failed\n", w);
    replicate();
    const char* arm = "pol";
#else
    const char* arm = "ref";
#endif
#if defined(EM_FIRST_FSM)
    const char* sub = "+emfirst";
#else
    const char* sub = "";
#endif
    printf("SPACING_PROBE arm=%s%s RMAX=%u contact-range=%u dD=%u W=%u "
           "sites(K/D1/D2)=%u/%u/%u\n",
           arm, sub, RMAX, 2u * RMAX, dD, W, xK, xD1, xD2);

    unsigned frame = 0, promotions = 0, maxD = 0;
    unsigned char prev[3];
    for (unsigned w = 0; w < W; ++w) prev[w] = (unsigned char)source(w).kind;
    long long totalRepel = 0, totalContacts = 0;
    for (;;)
    {
      const unsigned d12 = pairDistance();
      maxD = std::max(maxD, d12);
      for (unsigned w = 0; w < W; ++w)
      {
        const unsigned char k = (unsigned char)source(w).kind;
        if (prev[w] == (unsigned char)SourceKind::D &&
            k == (unsigned char)SourceKind::K) ++promotions;
        prev[w] = k;
      }
      totalRepel = conv_repel;   // keep the running total (printed below)
      printf("FRAME %3u kinds=%u,%u,%u d12=%2u maxd=%2u promotions=%u "
             "repel_total=%lld\n",
             frame, (unsigned)source(0).kind, (unsigned)source(1).kind,
             (unsigned)source(2).kind, d12, maxD, promotions,
             (long long)conv_repel);
      if (frame >= frames) break;
      while (!simulation()) {}
      ++frame;
    }
    (void)totalContacts;
    {
      unsigned nk = 0, nd = 0, ns = 0;
      for (unsigned w = 0; w < W; ++w)
      {
        const SourceKind k = source(w).kind;
        nk += (k == SourceKind::K);
        nd += (k == SourceKind::D);
        ns += (k == SourceKind::S);
      }
      printf("SUMMARY arm=%s%s dD=%u frames=%u promotions=%u K=%u D=%u S=%u "
             "maxd12=%u -> %s\n", arm, sub, dD, frame, promotions, nk, nd, ns,
             maxD,
             promotions == 0 ? "ROLES KEPT (no T2: the family survived)"
                             : "T2 FIRED (the family lost a member)");
    }
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
