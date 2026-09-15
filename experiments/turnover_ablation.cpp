// turnover_ablation.cpp -- the EXCLUSION on/off ablation on a configuration with
// TURNOVER (item 2 of "what to try next"; feeds quantization/quantize_stdlib.py).
//
// The superposed seed cannot host this ablation: every build freezes (zero
// escapes), so Gamma_esc == 0 by construction and no drift can be estimated.  This
// harness builds a prepared configuration in which constituents CAN be exchanged:
//
//   tube 27 x 5 x 5  (RMAX = min(LY,LZ)/2 = 2, so contact needs d <= 2*RMAX = 4)
//   body      : layers 0,1,2 (one family, charge 0x08) as K + 2 D, spread at
//               x = 3, 10, 17 (pairwise distances 7, 7, 9 -- all > 4, so the body
//               itself is not self-contacting)
//   reservoir : layers 3,4,5 at x = 6, 13, 20 ("co", each 3 from one body member,
//               pairwise 7 apart) or layer 3 alone at x = 24 ("far", >= 5 from
//               every body member and out of contact)
//
// Arms: the same harness built with and without /D EXCLUSION_FSM.  Expectations,
// registered before the runs:
//   ref  + co   : captures > 0 (a reservoir S attaches through T4) and escapes > 0
//                 (the incoming D and a body D then meet and T2 promotes) -> turnover
//   ref  + far  : nothing in contact -> frozen
//   excl + co   : the hard core blocks cross-family equal-charge contacts, so the
//                 reservoir can never feed the body -> frozen (this is the point:
//                 the hard core isolates, it does not select)
//
// Writes constituents.csv in the census format
//   frame,chief_w,constituents,captures,escapes
// so quantization/flux_from_census.py consumes it directly.
//
// Build: experiments\build_turnover_ablation.bat
// Run:   build\turnover_ablation\turnover_ablation_<ref|exc>.exe [frames] [co|far] [outdir]

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
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

// membership snapshot: chief -> sorted list of member layers (chief included)
static std::map<unsigned, std::vector<unsigned>> membership(unsigned W)
{
  std::map<unsigned, std::vector<unsigned>> m;
  for (unsigned w = 0; w < W; ++w)
  {
    const Cell& s = source(w);
    if (s.kind == SourceKind::K) { m[w].push_back(w); }
    else if (s.kind == SourceKind::D && s.parent < W_USED) m[s.parent].push_back(w);
  }
  for (auto& kv : m) std::sort(kv.second.begin(), kv.second.end());
  return m;
}

// membership diff: members of `cur` not in `prev` are captures, the reverse escapes
static void diffCounts(const std::vector<unsigned>& prev,
                       const std::vector<unsigned>& cur,
                       size_t* captures, size_t* escapes)
{
  *captures = 0; *escapes = 0;
  for (unsigned v : cur)
    if (std::find(prev.begin(), prev.end(), v) == prev.end()) ++*captures;
  for (unsigned v : prev)
    if (std::find(cur.begin(), cur.end(), v) == cur.end()) ++*escapes;
}

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    const unsigned frames = (argc > 1) ? (unsigned)atoi(argv[1]) : 40u;
    const char* mode = (argc > 2) ? argv[2] : "co";
    const char* outdir = (argc > 3) ? argv[3] : ".";
    const bool co = (strcmp(mode, "co") == 0);
    const bool far = (strcmp(mode, "far") == 0);
    const bool move = (strcmp(mode, "move_co") == 0) || (strcmp(mode, "move_far") == 0);
    const bool moveCo = (strcmp(mode, "move_co") == 0);
    if (!co && !far && !move)
    { fprintf(stderr, "mode must be co, far, move_co or move_far\n"); return 2; }

    constexpr unsigned LX = 27, LY = 5, LZ = 5, W = 6;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;

    if (co || far)
    {
      // Static body: one family (layers 0-2), charge 0x08, spread beyond reach
      placeLayer(0, 3, y, z, 0x08);
      placeLayer(1, 10, y, z, 0x08);
      placeLayer(2, 17, y, z, 0x08);
      if (co)
      {
        placeLayer(3, 6, y, z, 0x08);
        placeLayer(4, 13, y, z, 0x08);
        placeLayer(5, 20, y, z, 0x08);
      }
      else
      {
        placeLayer(3, 24, y, z, 0x08);   // the only same-charge singleton
        placeLayer(4, 24, y, z, 0x09);   // inert spectators: another charge word,
        placeLayer(5, 24, y, z, 0x0A);   // so T1-T5 can never involve them
      }
      replicate();
      markChief(0, 0x08);
      markDelegate(1, 0, 0x08);
      markDelegate(2, 0, 0x08);
      replicate();
    }
    else
    {
      // Moving body: a K alone (layer 0) plus ONE drive pair (layers 1,2) bound
      // to it with momentum +RMAX along x -- the K-only transport case that the
      // inertia campaign validates.  A compact body is what can move, and a
      // compact body is where T2 afterwards splits a capture.
      placeLayer(0, 3, y, z, 0x08);
      placeLayer(1, 3, y, z, 0x00);
      placeLayer(2, 3, y, z, 0x1F);
      if (moveCo)
      {
        placeLayer(3, 8, y, z, 0x08);    // reservoir ahead, reachable (3 -> 8 -> ...)
        placeLayer(4, 14, y, z, 0x08);
        placeLayer(5, 20, y, z, 0x08);
      }
      else
      {
        // Control: SAME geometry, different charge words, so T1-T5 can never
        // involve the passing body.  (The body travels ~0.75 cells/frame, so a
        // distance-based "behind" control is reached within a wrap; the charge
        // word is the clean discriminator.)
        placeLayer(3, 8, y, z, 0x09);
        placeLayer(4, 14, y, z, 0x0A);
        placeLayer(5, 20, y, z, 0x0B);
      }
      replicate();
      markChief(0, 0x08);
      for (unsigned w = 1; w <= 2; ++w)          // reciprocal drive pair
      {
        for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
        {
          const auto& ctr = lcenters[w];
          Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
          c.kind = SourceKind::P;
          c.parent = 0u; c.leader_w = 0u; c.a = 0u;
          c.pair_idx = (w == 1u) ? 2u : 1u;
          c.pair_count = 1u;
          c.m[0] = (int)RMAX;                    // +x, as inertia_fixture::prepare
        }
      }
      replicate();
    }


#ifdef EXCLUSION_FSM
    const char* arm = "excl";
#else
    const char* arm = "ref";
#endif
    char csvPath[512];
    snprintf(csvPath, sizeof csvPath, "%s/constituents.csv", outdir);
    FILE* csv = fopen(csvPath, "w");
    if (!csv)
      throw std::runtime_error("cannot open the output CSV (create the directory first)");
    fprintf(csv, "frame,chief_w,constituents,captures,escapes\n");
    printf("TURNOVER_ABLATION arm=%s mode=%s LX=%u LY=%u LZ=%u W=%u RMAX=%u "
           "contact-range=%u\n", arm, mode, LX, LY, LZ, W, RMAX, 2u * RMAX);

    long long totCap = 0, totEsc = 0, prevCalls = conv_calls;
    auto emit = [&](unsigned frame,
                    const std::map<unsigned, std::vector<unsigned>>& prev,
                    const std::map<unsigned, std::vector<unsigned>>& cur)
    {
      std::vector<unsigned> keys;
      for (const auto& kv : prev) keys.push_back(kv.first);
      for (const auto& kv : cur)
        if (std::find(keys.begin(), keys.end(), kv.first) == keys.end())
          keys.push_back(kv.first);
      std::sort(keys.begin(), keys.end());
      for (unsigned k : keys)
      {
        static const std::vector<unsigned> empty;
        const auto ita = prev.find(k), itb = cur.find(k);
        const std::vector<unsigned>& a = (ita == prev.end()) ? empty : ita->second;
        const std::vector<unsigned>& b = (itb == cur.end()) ? empty : itb->second;
        size_t cap = 0, esc = 0;
        diffCounts(a, b, &cap, &esc);
        totCap += (long long)cap;
        totEsc += (long long)esc;
        fprintf(csv, "%u,%u,%zu,%zu,%zu\n", frame, k,
                b.empty() ? (size_t)0 : b.size() - 1, cap, esc);
      }
    };

    std::map<unsigned, std::vector<unsigned>> prev = membership(W);
    // No baseline row: the initial membership is the reference snapshot for
    // frame 1, so the first row has zero flux by construction (matching the
    // census convention, where a run's first logged frame shows no captures).

    unsigned frame = 0;
    for (;;)
    {
      if (frame >= frames) break;
      while (!simulation()) {}
      ++frame;
      const std::map<unsigned, std::vector<unsigned>> cur = membership(W);
      const long long contacts = conv_calls - prevCalls;
      prevCalls = conv_calls;
      emit(frame, prev, cur);
      unsigned nk = 0, nd = 0, ns = 0, maxPop = 0, members = 0;
      for (unsigned w = 0; w < W; ++w)
      {
        const SourceKind k = source(w).kind;
        nk += (k == SourceKind::K);
        nd += (k == SourceKind::D);
        ns += (k == SourceKind::S);
      }
      for (const auto& kv : cur)
      {
        maxPop = std::max<unsigned>(maxPop, (unsigned)kv.second.size());
        members += (unsigned)kv.second.size();
      }
      printf("FRAME %3u K=%u D=%u S=%u groups=%zu max_pop=%u members=%u "
             "x0=%u x1=%u contacts=+%lld totals(cap=%lld esc=%lld)\n",
             frame, nk, nd, ns, cur.size(), maxPop, members,
             lcenters[0][0], lcenters[1][0], contacts, totCap, totEsc);
      prev = cur;
    }
    fclose(csv);
    printf("SUMMARY arm=%s mode=%s frames=%u captures=%lld escapes=%lld -> %s\n",
           arm, mode, frame, totCap, totEsc,
           (totEsc == 0)
               ? "FROZEN (no escapes: the flux harness will report INCONCLUSIVE)"
               : "TURNOVER (usable by the flux harness)");
    printf("wrote %s\n", csvPath);
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
