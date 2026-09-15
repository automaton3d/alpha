// p7_concentration.cpp -- P7 of DYNAMIC_QUANTIZATION_DERIVATION.md.
//
// A prepared 1K+2D island of one charge word (one family, w = 0,1,2) is planted
// in a 15x5x5 tube, where RMAX = min(LY,LZ)/2 = 2, so two wavefronts can meet
// only at centre distance <= 2*RMAX = 4 cells.  Two layouts:
//
//   co     (default): all three constituents at ONE site (co-located).  The
//          membership theorem predicts that the two delegates meet with the
//          gate open and the D x D promotion (T2) fires, so the island loses a
//          member: 1K+2D -> 2K+1D (max group population 2).
//   spread: the constituents 5 cells apart along the long axis (beyond the
//          contact range), so no contact is possible: no promotion, the group
//          of population 3 survives.
//
// Ordinary production simulation() path; no candidate macro.  This is the
// falsifier left open in the derivation note (section 2, "Scope").
//
// Build: experiments\build_p7_concentration.bat
// Run:   build\p7_concentration\p7_concentration.exe [frames] [co|spread]

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdexcept>
#include <vector>

using namespace inertia_fixture;
using namespace automaton;

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch, unsigned phase = 0u)
{
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz)
      {
        Cell& c = getCell(lattice_curr, xx, yy, zz, w);
        c.ch = ch; c.a = W_USED; c.kind = SourceKind::S;
        c.parent = NO_PARENT;
        c.r2 = INF_R2; c.r = -1; c.u = 0; c.v = 0;
        c.t = 0; c.f = 0; c.active = 0;
      }
  Cell& c = getCell(lattice_curr, x, y, z, w);
  c.r2 = 0u; c.r = 0; c.u = 2048; c.v = 0;
  // The breathing phase of the source: distinct values make the copies
  // distinguishable for the PHASE_DISTINCT_FSM candidate.
  c.t = phase; c.f = 0; c.bstamp = 0; c.pol_u = c.pol_v = 0;
  c.active = 0; c.phiB = false;
  c.c[0] = c.c[1] = c.c[2] = 0;
}

static void markChief(unsigned w, unsigned char ch)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    const unsigned fam = 3u * (w / 3u);
    c.kind = SourceKind::K; c.parent = NO_PARENT;
    c.leader_w = (WIndex)fam; c.a = fam;
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
  }
}

static void markDelegate(unsigned w, unsigned chief, unsigned char ch)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::D; c.parent = chief; c.leader_w = (WIndex)chief;
    c.a = 3u * (w / 3u);
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
  }
}

// Group populations over the source centres: a K keys its own group, a D the
// group of its parent.  Returns the sorted multiset of populations.
static std::vector<unsigned> populations(unsigned W)
{
  std::map<unsigned, unsigned> pop;
  for (unsigned w = 0; w < W; ++w)
  {
    const Cell& s = source(w);
    if (s.kind == SourceKind::K) pop[w] += 1;
    else if (s.kind == SourceKind::D && s.parent < W_USED) pop[s.parent] += 1;
  }
  std::vector<unsigned> v;
  for (const auto& kv : pop) v.push_back(kv.second);
  std::sort(v.begin(), v.end());
  return v;
}

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    const unsigned frames = (argc > 1) ? (unsigned)atoi(argv[1]) : 10u;
    const char* mode = (argc > 2) ? argv[2] : "co";

    // Site layouts along the long axis (ELX = 15).  `gap` is the spacing between
    // consecutive constituents; RMAX = 2 here, so the wavefront-overlap range is
    // 2*RMAX = 4 cells.  The gap sequence 0, 1, 3, 4, 5 brackets that range from
    // both sides, which is what the escape rule's prediction is stated against.
    unsigned xs[5] = {7u, 7u, 7u, 7u, 7u};
    unsigned phaseStep = 0u;      // 1 => copy w gets breathing phase w
    unsigned copies = 3u;         // family multiplicity
    if      (strcmp(mode, "co")     == 0) { xs[0]=7;  xs[1]=7;  xs[2]=7;  }
    else if (strcmp(mode, "chain")  == 0) { xs[0]=7;  xs[1]=8;  xs[2]=9;  }
    else if (strcmp(mode, "gap2")   == 0) { xs[0]=7;  xs[1]=9;  xs[2]=11; }
    else if (strcmp(mode, "mid")    == 0) { xs[0]=7;  xs[1]=10; xs[2]=13; }
    else if (strcmp(mode, "edge")   == 0) { xs[0]=6;  xs[1]=10; xs[2]=14; }
    else if (strcmp(mode, "coPhase") == 0) { xs[0]=7;  xs[1]=7;  xs[2]=7; phaseStep = 1u; }
    else if (strcmp(mode, "five")    == 0) { copies = 5u; phaseStep = 1u; }
    else if (strcmp(mode, "fiveCo")  == 0) { copies = 5u; }
    else if (strcmp(mode, "spread") == 0) { xs[0]=2;  xs[1]=7;  xs[2]=12; }
    else { fprintf(stderr, "mode must be co|chain|mid|edge|spread\n"); return 2; }
    const bool co = (strcmp(mode, "co") == 0);
    const unsigned gap = xs[1] - xs[0];
    const bool escapePredicted = gap > 2u * RMAX;

    constexpr unsigned LX = 15, LY = 5, LZ = 5;
    const unsigned W = copies;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    ISLAND_SIZE = 3;                    // one family of 3 copies (tube default)
    const unsigned site = (LY - 1) / 2;
    for (unsigned w = 0; w < W; ++w)
      placeLayer(w, xs[w], site, site, 0x08, w * phaseStep);
    replicate();
    markChief(0, 0x08);
    for (unsigned w = 1; w < W; ++w) markDelegate(w, 0, 0x08);
    replicate();

    printf("P7 mode=%s ELX=%u ELY=%u ELZ=%u W=%u RMAX=%u ISLAND_SIZE=%u "
           "contact-range=%u gap=%u\n",
           mode, ELX, ELY, ELZ, W, RMAX, ISLAND_SIZE, 2u * RMAX, gap);
#ifdef SURFACE_ESCAPE_FSM
    const char* ruleOn = "ON";
#else
    const char* ruleOn = "OFF";
#endif
    printf("PREDICTION escape-rule=%s: %s\n", ruleOn,
           escapePredicted
             ? "gap > 2*RMAX -> no shell overlap, every delegate released"
             : "gap <= 2*RMAX -> contacts keep the group, T2 caps max_pop at 2");
#ifdef PHASE_DISTINCT_FSM
    const char* phaseRule = "ON";
#else
    const char* phaseRule = "OFF";
#endif
    printf("PREDICTION phase-distinct=%s step=%u: %s\n", phaseRule, phaseStep,
           phaseStep ? "distinct phases -> one group of 3 expected (max_pop 3)"
                     : "identical phases -> no group expected, all S (max_pop 1)");

    unsigned frame = 0, promotions = 0;
    long long prevCalls = conv_calls;
    std::vector<unsigned char> prevKind(W);
    for (unsigned w = 0; w < W; ++w) prevKind[w] = (unsigned char)source(w).kind;

    for (;;)
    {
      unsigned nk = 0, nd = 0, ns = 0;
      for (unsigned w = 0; w < W; ++w)
      {
        const SourceKind k = source(w).kind;
        nk += (k == SourceKind::K); nd += (k == SourceKind::D);
        ns += (k == SourceKind::S);
      }
      const std::vector<unsigned> pops = populations(W);
      const unsigned maxPop = pops.empty() ? 0u : pops.back();
      for (unsigned w = 0; w < W; ++w)
      {
        const unsigned char k = (unsigned char)source(w).kind;
        if (prevKind[w] == (unsigned char)SourceKind::D &&
            k == (unsigned char)SourceKind::K) ++promotions;
        prevKind[w] = k;
      }
      printf("FRAME %2u K=%u D=%u S=%u  groups=%zu max_pop=%u  contacts=+%lld"
             "  promotions=%u\n",
             frame, nk, nd, ns, pops.size(), maxPop,
             (long long)conv_calls - prevCalls, promotions);
      prevCalls = conv_calls;

      if (frame >= frames) break;
      while (!simulation()) {}
      ++frame;
    }

    const std::vector<unsigned> pops = populations(W);
    const unsigned maxPop = pops.empty() ? 0u : pops.back();
    const bool split = (promotions > 0);
    unsigned nk = 0, nd = 0, ns = 0;
    for (unsigned w = 0; w < W; ++w)
    {
      const SourceKind k = source(w).kind;
      nk += (k == SourceKind::K); nd += (k == SourceKind::D); ns += (k == SourceKind::S);
    }
#ifdef SURFACE_ESCAPE_FSM
    printf("surface_escapes_total=%u\n", surface_escapes);
#endif
    printf("MEASURED [%s gap=%u] promotions=%u max_pop=%u final=%uK+%uD+%uS\n",
           mode, gap, promotions, maxPop, nk, nd, ns);
    printf("EXPECTED reference-build: %s\n",
           (gap == 0u) ? "split via T2 -> 2K+1D, max_pop 2"
                       : "no contact -> 1K+2D survives, max_pop 3");
    printf("EXPECTED SURFACE_ESCAPE_FSM: %s\n",
           escapePredicted ? "all delegates released -> max_pop 1"
                           : "same as the reference build");
    const char* verdict = co
        ? ((split && maxPop <= 2) ? "P7 CONFIRMED (co-located island splits via T2)"
                                  : "P7 NOT CONFIRMED (co-located island kept its roles)")
        : ((!split && maxPop == 3) ? "CONTROL OK (spread island keeps 1K+2D)"
                                   : "CONTROL ANOMALY (spread island changed)");
    printf("VERDICT [%s] promotions=%u max_pop=%u -> %s\n", mode, promotions,
           maxPop, verdict);
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
