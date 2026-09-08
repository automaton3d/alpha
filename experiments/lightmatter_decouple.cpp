// lightmatter_decouple.cpp — provisional test: colour rule + light-matter
// decoupling of still-superposed constituents.
//
// Stand-in: the real pB/sB polarisation channel is deferred (no bootstrap).
// Each source receives a deterministic random two-bit light-matter label
// (SplitMix64, same pattern as the coagulation experiment) for immediate
// use.  Under LIGHT_MATTER_FSM the colour/matched FSM overrides the matched
// impulse (zero for coincident bound partners) with one antisymmetric
// separation step whenever two bound, coincident, equal-charge constituents
// carry different labels.  This tests whether "superposed bubbles detach via
// light-matter" after the colour rule has spread the unbound ones.
//
// Build: experiments\build_lightmatter_decouple.bat
// Run:   build\lightmatter_decouple\lightmatter_decouple.exe [seed] [frames]

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <cstdint>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#ifndef LIGHT_MATTER_FSM
#error Requires build_lightmatter_decouple.bat
#endif
using namespace inertia_fixture;

namespace {
// SplitMix64 (fixed-width arithmetic, reproducible per seed).
uint64_t rngState;
uint64_t random64() {
  uint64_t z = (rngState += UINT64_C(0x9e3779b97f4a7c15));
  z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
  return z ^ (z >> 31);
}
unsigned draw(unsigned bound) {
  const uint64_t threshold = (uint64_t(0) - bound) % bound;
  uint64_t x;
  do { x = random64(); } while (x < threshold);
  return unsigned(x % bound);
}

std::vector<unsigned char> lm;   // per-source light-matter label, 1..4
std::map<WIndex, std::vector<unsigned>> members;  // last sampled census
std::vector<std::array<unsigned, 3>> previous;
}  // namespace

namespace automaton {
void lightMatterInit() {
  // Called by the harness before the run (no per-frame action).
}
int lightMatterKick(unsigned a, unsigned w, int axis) {
  if (a == w || axis < 0 || axis > 2) return 0;
  if (a >= W_USED || w >= W_USED || lcenters.size() != W_USED) return 0;
  if (lm[a] == 0 || lm[w] == 0 || lm[a] == lm[w]) return 0;
  if (lcenters[a] != lcenters[w]) return 0;      // must still be superposed
  const Cell& sa = source(a);
  const Cell& sw = source(w);
  if (sa.ch != sw.ch) return 0;                  // equal charge only
  const bool bodyA = sa.kind == SourceKind::K || sa.kind == SourceKind::D;
  const bool bodyW = sw.kind == SourceKind::K || sw.kind == SourceKind::D;
  if (!bodyA || !bodyW) return 0;
  const WIndex ca = islandChief(sa), cw = islandChief(sw);
  if (ca == NO_PARENT || ca != cw) return 0;     // same chief (bound pair)
  const int chosen = int((uint64_t(a) + w) % 3u);
  if (axis != chosen) return 0;
  return a < w ? 1 : -1;                          // antisymmetric step
}
}  // namespace automaton

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    unsigned frames = 16;
    uint64_t seed = 1;
    std::string tag = "x";
    std::string mode = "cube";     // "cube" | "sparse8"
    if (argc > 5)
      throw std::runtime_error(
          "usage: lightmatter_decouple [seed] [frames] [tag] [mode]");
    if (argc >= 2) {
      size_t used = 0;
      seed = std::stoull(argv[1], &used, 10);
      if (used != std::string(argv[1]).size())
        throw std::runtime_error("invalid seed");
    }
    if (argc >= 3) {
      char* end = nullptr;
      unsigned long v = strtoul(argv[2], &end, 10);
      if (end == argv[2] || *end || v < 1 || v > 1000)
        throw std::runtime_error("frames must be 1..1000");
      frames = (unsigned)v;
    }
    if (argc == 4) tag = argv[3];
    if (argc == 5) {
      tag = argv[3];
      mode = argv[4];
      if (mode != "cube" && mode != "sparse8" && mode != "sparse24" &&
          mode != "sparse81" && mode != "spread81" && mode != "duo2" &&
          mode != "duo4")
        throw std::runtime_error(
            "mode must be cube, sparse8, sparse24, sparse81, spread81, "
            "duo2 or duo4");
    }

    rngState = seed;
    const std::string dir = "build/lightmatter_decouple/" + tag + "_seed_" +
                            std::to_string(seed);
    std::filesystem::create_directories(dir);

    unsigned targetPop = 3;   // predicted island population per family
    if (mode == "sparse8" || mode == "sparse24" || mode == "sparse81" ||
        mode == "spread81" || mode == "duo2" || mode == "duo4") {
      // Sparse tube: families placed at distinct sites.  Charge word of
      // family f follows the canonical seed formula and repeats every 8
      // families, so same-charge families are separated by 8*family-index
      // steps (>= 2*RMAX+1 = 5 for RMAX=2); different-charge families never
      // interact (colour/election/cohesion all require equal charge).
      //   sparse8 : 8 families, LX=41, spacing 5 (legacy local test);
      //   sparse24: 24 families, LX=27, consecutive sites;
      //   sparse81: 81 families, LX=85, consecutive sites (same charge out
      //             of reach -> islands of 3 are geometry-guaranteed);
      //   spread81: 81 families, LX=85, reordered by charge word so that
      //             same-charge families are ADJACENT (spacing 1 <= 2*RMAX)
      //             -> full-scale measure of same-charge merging;
      //   duo2/4  : 2 families of the SAME charge word at distance 2 or 4
      //             (<= 2*RMAX), so their shells contact (discriminating
      //             test of identity survival under same-charge contact).
      const bool duo = (mode == "duo2" || mode == "duo4");
      const bool spread = (mode == "spread81");
      unsigned NF = 0, LX = 0, spacing = 0;
      if (mode == "sparse8")      { NF = 8;  LX = 41; spacing = 5; }
      else if (mode == "sparse24"){ NF = 24; LX = 27; spacing = 1; }
      else if (mode == "sparse81"){ NF = 81; LX = 85; spacing = 1; }
      else if (mode == "spread81"){ NF = 81; LX = 85; spacing = 1; }
      else if (mode == "duo2")    { NF = 2;  LX = 21; spacing = 2; }
      else                        { NF = 2;  LX = 21; spacing = 4; }
      const unsigned LY = 5, LZ = 5;
      // spread81: reorder family sites in charge-word-major order so that
      // families of the same charge word become adjacent.
      std::vector<unsigned> famPos;
      if (spread) {
        famPos.assign(NF, 0u);
        unsigned idx = 0;
        for (unsigned word = 0; word < 8u; ++word)
          for (unsigned fam = 0; fam < NF; ++fam)
            if (fam % 8u == word) famPos[fam] = idx++;
      }
      if (!tryAllocateTube(LX, LY, LZ, 3u * NF))
        throw std::runtime_error(lastAllocationError);
      initSimulation(0);
      for (unsigned w = 0; w < W_USED; ++w) {
        const unsigned fam = w / 3;
        const unsigned island = duo ? 0u : fam;  // duo: same charge word
        const unsigned char w0 = (unsigned char)(island & 1u);
        const unsigned char w1 = (unsigned char)((island >> 1) & 1u);
        const unsigned char q = (unsigned char)(w0 ^ w1);
        const unsigned char famCh =
            (unsigned char)((island % 8u) | (q << 3) | (w0 << 4) | (w1 << 5));
        const unsigned siteX = 2u + (spread ? famPos[fam] : spacing * fam);
        lcenters[w] = {siteX, 2u, 2u};
        for (unsigned x = 0; x < ELX; ++x)
          for (unsigned y = 0; y < ELY; ++y)
            for (unsigned z = 0; z < ELZ; ++z) {
              Cell& c = getCell(lattice_curr, x, y, z, w);
              c.ch = famCh;
              c.a = 3u * fam;
              c.leader_w = 3u * fam;
            }
        // (Re)place the source centre at the family site and clear the old
        // superposed centre left by initSimulation(0).
        for (unsigned x = 0; x < ELX; ++x)
          for (unsigned y = 0; y < ELY; ++y)
            for (unsigned z = 0; z < ELZ; ++z) {
              Cell& c = getCell(lattice_curr, x, y, z, w);
              if (x == siteX && y == 2u && z == 2u) {
                c.kind = SourceKind::S;
                c.parent = NO_PARENT;
                c.pair_idx = NO_PAIR;
                c.pair_count = 0;
                c.m[0] = c.m[1] = c.m[2] = 0;
                c.reloc[0] = c.reloc[1] = c.reloc[2] = 0;
                c.r2 = 0u; c.r = 0; c.u = 2048; c.v = 0;
                c.t = 0; c.f = 0; c.bstamp = 0;
                c.pol_u = c.pol_v = 0;
                c.active = 0; c.phiB = false;
                c.c[0] = c.c[1] = c.c[2] = 0;
              } else if (c.r2 == 0u || c.r == 0 || c.u == 2048) {
                c.r2 = INF_R2; c.r = -1; c.u = 0; c.v = 0;
                c.t = 0; c.f = 0; c.active = 0;
              }
            }
      }
      replicate();
      printf("MODE %s LX=%u LY=%u LZ=%u W=%u copies=3 spacing=%u "
             "(2*RMAX=%u)%s\n", mode.c_str(), ELX, ELY, ELZ, W_USED, spacing,
             2u * RMAX, spread ? " (charge-word-major order)" : "");
    } else {
      if (!tryAllocate(9, 243)) throw std::runtime_error(lastAllocationError);
      calculateParameters(9, 243);
      initSimulation(0);
      replicate();
      targetPop = EL / 3;   // L/3 = 3 for L=9
    }

    // Deterministic random light-matter labels, 1..4 (two bits: electric/mag).
    lm.assign(W_USED, 0);
    for (unsigned w = 0; w < W_USED; ++w) lm[w] = (unsigned char)(1 + draw(4));
    lightMatterInit();

    FILE* trace = fopen((dir + "/trace.csv").c_str(), "w");
    FILE* groups = fopen((dir + "/groups.csv").c_str(), "w");
    FILE* labels = fopen((dir + "/labels.csv").c_str(), "w");
    if (!trace || !groups || !labels)
      throw std::runtime_error("cannot open diagnostics");
    for (FILE* f : {trace, groups, labels}) setvbuf(f, nullptr, _IONBF, 0);
    fprintf(trace,
            "frame,K,D,S,centers,chief_centers,groups_of_three,"
            "unresolved,decouple_pairs\n");
    fprintf(groups,
            "frame,chief,population,families,centers,max_distance_to_chief\n");
    for (unsigned w = 0; w < W_USED; ++w)
      fprintf(labels, "%u,%u,%u\n", w, source(w).ch, lm[w]);

    // Charge and intrinsic-address invariant baselines.
    std::vector<unsigned char> charges(W_USED);
    for (unsigned w = 0; w < W_USED; ++w) charges[w] = source(w).ch;
    previous = lcenters;

    unsigned frameNumber = 0;
    for (;;) {
      // ---- frame-boundary census ----
      unsigned nk = 0, nd = 0, ns = 0, unresolved = 0, three = 0;
      unsigned decouplePairs = 0;
      std::set<std::array<unsigned, 3>> centers, chiefCenters;
      members.clear();
      for (unsigned w = 0; w < W_USED; ++w) {
        const Cell& s = source(w);
        if (s.w != w || s.ch != charges[w] || s.a != (w / 3) * 3 ||
            s.kind == SourceKind::P || s.pair_idx != NO_PAIR ||
            s.m[0] || s.m[1] || s.m[2])
          throw std::runtime_error("identity or no-inertia invariant failed");
        nk += s.kind == SourceKind::K;
        nd += s.kind == SourceKind::D;
        ns += s.kind == SourceKind::S;
        centers.insert(lcenters[w]);
        if (s.kind == SourceKind::K) chiefCenters.insert(lcenters[w]);
        const WIndex chief = islandChief(s);
        if (chief != NO_PARENT) {
          members[chief].push_back(w);
          if (source(chief).kind != SourceKind::K) ++unresolved;
          if (source(chief).ch != s.ch)
            throw std::runtime_error("mixed-charge parent");
        } else if (s.kind == SourceKind::D) {
          ++unresolved;   // delegate without a valid living chief
        }
      }
      // Unordered coincident bound same-charge pairs with different labels.
      for (auto& [chief, ws] : members) {
        std::sort(ws.begin(), ws.end());
        for (size_t i = 0; i < ws.size(); ++i)
          for (size_t j = i + 1; j < ws.size(); ++j) {
            const unsigned a = ws[i], b = ws[j];
            if (lcenters[a] != lcenters[b]) continue;
            if (source(a).ch != source(b).ch) continue;
            if (lm[a] != lm[b]) ++decouplePairs;
          }
        if (ws.size() == targetPop) ++three;
        std::set<unsigned> families;
        std::set<std::array<unsigned, 3>> sites;
        unsigned radius = 0;
        for (unsigned w : ws) {
          families.insert(w / 3);
          sites.insert(lcenters[w]);
          unsigned d = 0;
          for (int k = 0; k < 3; ++k)
            d += (unsigned)std::abs(wrappedDelta(
                lcenters[chief][k], lcenters[w][k], EL));
          radius = std::max(radius, d);
        }
        fprintf(groups, "%u,%u,%zu,%zu,%zu,%u\n", frameNumber, chief,
                ws.size(), families.size(), sites.size(), radius);
      }
      fprintf(trace, "%u,%u,%u,%u,%zu,%zu,%u,%u,%u\n", frameNumber, nk, nd,
              ns, centers.size(), chiefCenters.size(), three, unresolved,
              decouplePairs);
      printf("FRAME %u K=%u D=%u S=%u centers=%zu size3=%u "
             "decouple_pairs=%u\n",
             frameNumber, nk, nd, ns, centers.size(), three, decouplePairs);
      if (frameNumber == frames) break;

      previous = lcenters;
      while (!simulation()) {}
      ++frameNumber;
    }

    for (FILE* f : {trace, groups, labels}) fclose(f);
    puts("PASS identity and transport checks; decoupling is a measured "
         "outcome, not an imposed island count.");
    return 0;
  } catch (const std::exception& e) {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
