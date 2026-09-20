// island_rate_probe.cpp -- small-W rate-balance probe: RELEASE vs RECRUITMENT.
//
// WHY SMALL W.  The surface-escape rule releases a delegate only after `W_USED` consecutive
// light frames without a same-charge contact (src/model/interaction.cpp:762-764).  In the
// cubic census W_USED = 243 at L=9 (~3.4 h of run time at ~50 s/frame), so no release can
// fire in a run of feasible length and the balance is out of reach there (measured: the
// escape build is identical to the repair build at frame 2, escapes = 0).  The tube fixtures
// allocate W = n + 2*pairs layers (3-7), so the timer expires in 3-7 frames and the
// competition runs in SECONDS.  The cube topology is lost; the rate question is what this
// answers.
//
// OUTPUT is exactly the flux-harness format consumed by quantization/quantize_stdlib.py:
//     run,island,frame,N,captures,escapes
// where N is the island population at the end of the light frame, captures are the membership
// ARRIVALS and escapes the membership DEPARTURES between consecutive light frames (measured
// from the parent linkage, not from a counter), and island is the chief's W address.
//
// ARMS.  Two builds (see build_island_rate_probe.bat):
//     rb_ref.exe   SURFACE_ESCAPE_FSM                       (reference rules + release)
//     rb_cand.exe  + PARENT_SELECTIVE_FSM + FUSION_ABSORB + NOMINATION_REPAIR
// so the A/B isolates the candidate's transitions from the release rule.
// Sweep N and the number of drive pairs; each invocation is one `run` in the CSV.
//
// Usage: island_rate_probe N PAIRS FRAMES CSV [SIEVE]

#include "inertia_fixture.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace inertia_fixture;

namespace {
constexpr unsigned LX = 15, LY = 5, LZ = 5;   // transport axis long; RMAX = 2 (tube default)

// --- placement helpers for the PLACED multi-island fixture (pattern: experiments/spin_probe.cpp) ---
void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z, unsigned char ch) {
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz) {
        Cell& c = getCell(lattice_curr, xx, yy, zz, w);
        c.ch = ch; c.a = W_USED; c.kind = SourceKind::S;
        c.parent = NO_PARENT; c.leader_w = NO_LEADER_W;
        c.r2 = INF_R2; c.r = -1; c.u = 0; c.v = 0;
        c.t = 0; c.f = 0; c.active = 0; c.m[0] = c.m[1] = c.m[2] = 0;
      }
  Cell& c = getCell(lattice_curr, x, y, z, w);
  c.r2 = 0u; c.r = 0; c.u = 2048; c.v = 0;
  c.t = 0; c.f = 0; c.bstamp = 0; c.pol_u = c.pol_v = 0;
  c.active = 0; c.phiB = false;
  c.c[0] = c.c[1] = c.c[2] = 0;
}
void markChief(unsigned w, unsigned char ch) {
  for (auto* lat : {&lattice_curr, &lattice_draft, &lattice_partner}) {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::K; c.parent = NO_PARENT; c.leader_w = (WIndex)w;
    c.a = 0; c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
  }
}
void markDelegate(unsigned w, unsigned chief, unsigned char ch) {
  for (auto* lat : {&lattice_curr, &lattice_draft, &lattice_partner}) {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::D; c.parent = chief; c.leader_w = (WIndex)chief;
    c.a = 0; c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
  }
}
void markSingleton(unsigned w, unsigned char ch) {
  for (auto* lat : {&lattice_curr, &lattice_draft, &lattice_partner}) {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::S; c.parent = NO_PARENT; c.leader_w = NO_LEADER_W;
    c.a = 0; c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
  }
}
// One half of a reciprocal drive pair, matched to its island's affinity (a = 0 here), with the
// transport vector m[axis] = RMAX * direction -- the same setting experiments/inertia_probe.cpp
// uses, whose measured rate is 0.25 cells per light frame per pair.
void markPairHalf(unsigned w, unsigned other, unsigned chief, unsigned char ch, int axis, int direction) {
  for (auto* lat : {&lattice_curr, &lattice_draft, &lattice_partner}) {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::P; c.parent = (WIndex)chief; c.leader_w = (WIndex)chief;
    c.a = 0; c.pair_idx = (WIndex)other; c.pair_count = 1; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
    c.m[axis] = (int)RMAX * direction;
  }
}

using Members = std::map<WIndex, std::vector<WIndex>>;

Members membership() {
  Members m;
  for (WIndex w = 0; w < W_USED; ++w) {
    const Cell& s = source(w);
    if (s.kind == SourceKind::K) m[w];                       // register the chief
    else if (s.kind == SourceKind::D && s.parent < W_USED) m[s.parent].push_back(w);
  }
  return m;
}
inline bool has(const std::vector<WIndex>& v, WIndex w) {
  for (WIndex x : v) if (x == w) return true;
  return false;
}
}  // namespace

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    if (argc < 4) {
      fprintf(stderr, "usage: island_rate_probe N PAIRS FRAMES CSV [SIEVE]\n"
                      "       island_rate_probe placed FRAMES CSV [SIEVE]   (multi-island fixture)\n");
      return 1;
    }
    const bool placed = (strcmp(argv[1], "placed") == 0);
    const bool driven = (strcmp(argv[1], "driven") == 0);
    unsigned n = 2, pairs = 0, frames = 0;
    const char* csvPath = nullptr;
    unsigned sieve = 16384u;
    if (placed || driven) {
      frames = (unsigned)atoi(argv[2]);
      csvPath = argv[3];
      sieve = (argc > 4) ? (unsigned)atoi(argv[4]) : 16384u;
      if (frames < 2) return 2;
    } else {
      if (argc < 5) return 2;
      n = (unsigned)atoi(argv[1]);
      pairs = (unsigned)atoi(argv[2]);
      frames = (unsigned)atoi(argv[3]);
      csvPath = argv[4];
      sieve = (argc > 5) ? (unsigned)atoi(argv[5]) : 16384u;
      if (n < 2 || n > 8 || pairs > 4 || frames < 2) return 2;
    }

#if defined(PARENT_SHELL_RELEASE_FSM)
    const char* build = "shell";
#elif defined(PARENT_SELECTIVE_FSM)
    const char* build = "cand";
#else
    const char* build = "ref";
#endif

    s2b_target = sieve;
    if (placed) {
      // ------------------------------------------------------------------
      // PLACED multi-island fixture -- the configuration the rate balance needs (specification
      // in experiments/PARENT_SELECTIVE.md): two islands separated beyond the contact range,
      // one delegate per island left OUT of contact with its chief (so its escape timer runs),
      // the two stray delegates ADJACENT to each other (so a cross-island D x D pair exists:
      // repulsion in the candidate, a promotion in the reference), and one free S in contact
      // with each chief (so recruitment can fire).  W = 6 -> the escape timer is 6 light frames.
      //   w0 K_A @2    w1 D_A @9    w2 K_B @18    w3 D_B @10    w4 S1 @5    w5 S2 @15
      //   contact range 2*RMAX = 4
      // ------------------------------------------------------------------
      constexpr unsigned PX = 21;   // long axis; RMAX = min(LY,LZ)/2 = 2
      if (!tryAllocateTube(PX, LY, LZ, 6)) throw std::runtime_error(lastAllocationError);
      initSimulation(0);
      const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;
      placeLayer(0, 2, y, z, 0x08);
      placeLayer(1, 9, y, z, 0x08);
      placeLayer(2, 18, y, z, 0x08);
      placeLayer(3, 10, y, z, 0x08);
      placeLayer(4, 5, y, z, 0x08);
      placeLayer(5, 15, y, z, 0x08);
      replicate();
      markChief(0, 0x08);
      markDelegate(1, 0, 0x08);
      markChief(2, 0x08);
      markDelegate(3, 2, 0x08);
      markSingleton(4, 0x08);
      markSingleton(5, 0x08);
      replicate();
      n = W_USED;
    } else if (driven) {
      // ------------------------------------------------------------------
      // DRIVEN fixture -- the placed configuration PLUS the carrier the structural tension
      // requires: a reciprocal drive pair, affinity-matched to island B, that transports B toward
      // the gap, so the bubble RELEASED by island A is eventually reached and recruited.  The pair
      // measures 0.25 cells per light frame (one pair, inertia campaign), so B walks ~1 cell per 4
      // frames and crosses the ~9-cell gap in ~35 frames, while D_A's escape timer (W = 6 frames)
      // expires long before.  Island A carries a DIFFERENT affinity (1) so the pair can dress only B.
      //   w0 K_A @2 (a=1)   w1 D_A @9 (a=1, isolated)   w2 K_B @18 (a=0)   w3 D_B @18 (a=0)
      //   w4,w5 pair @18 (a=0), m = (RMAX,0,0) * direction, direction = -1 -> B moves toward x=2
      // ------------------------------------------------------------------
      constexpr unsigned PX = 21;
      if (!tryAllocateTube(PX, LY, LZ, 6)) throw std::runtime_error(lastAllocationError);
      initSimulation(0);
      const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;
      placeLayer(0, 2, y, z, 0x08);
      placeLayer(1, 9, y, z, 0x08);
      placeLayer(2, 18, y, z, 0x08);
      placeLayer(3, 18, y, z, 0x08);
      placeLayer(4, 18, y, z, 0x08);
      placeLayer(5, 18, y, z, 0x08);
      replicate();
      markChief(0, 0x08);
      markDelegate(1, 0, 0x08);
      markChief(2, 0x08);
      markDelegate(3, 2, 0x08);
      markPairHalf(4, 5, 2, 0x08, /*axis=*/0, /*direction=*/-1);
      markPairHalf(5, 4, 2, 0x08, /*axis=*/0, /*direction=*/-1);
      for (auto* lat : {&lattice_curr, &lattice_draft, &lattice_partner})
        for (unsigned w : {0u, 1u}) {
          const auto& ctr = lcenters[w];
          getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w).a = 1u;   // island A affinity
        }
      replicate();
      n = W_USED;
    } else {
      prepare(LX, LY, LZ, n, pairs, /*axis=*/0, /*direction=*/1);
    }
    Measurement measurement(n);

    char run[64];
    if (placed || driven) snprintf(run, sizeof run, "%s_%s", placed ? "placed" : "driven", build);
    else snprintf(run, sizeof run, "%s_N%u_P%u", build, n, pairs);

    FILE* f = fopen(csvPath, "w");
    if (!f) { perror(csvPath); return 3; }
    fprintf(f, "run,island,frame,N,captures,escapes\n");

    printf("RATE_PROBE build=%s mode=%s N=%u pairs=%u W_USED=%u RMAX=%u contact=%u sieve=%u "
           "frames=%u run=%s\n", build, (placed || driven) ? (placed ? "placed" : "driven") : "prepared",
           n, pairs, W_USED, RMAX,
           2u * RMAX, sieve, frames, run);
    printf("NOTE: escape timer expires after W_USED = %u contact-free light frames\n", W_USED);

    Members prev = membership();
    std::vector<unsigned char> prevKind(W_USED);
    for (WIndex w = 0; w < W_USED; ++w) prevKind[w] = (unsigned char)source(w).kind;
    unsigned frame = 0, released = 0, recruited = 0;
    unsigned nRel = 0, nProm = 0, nDem = 0, nRecruit = 0;
    for (;;) {
      if (!simulation()) continue;
      measurement.sample();
      Members now = membership();

      // Role transitions, counted separately: the membership diff below cannot tell a RELEASE
      // (D -> S) from a PROMOTION (D -> K) or a demotion (K -> D), and they mean different things.
      for (WIndex w = 0; w < W_USED; ++w) {
        const unsigned char k = (unsigned char)source(w).kind;
        const unsigned char p = prevKind[w];
        if (p == (unsigned char)SourceKind::D && k == (unsigned char)SourceKind::S) ++nRel;
        else if (p == (unsigned char)SourceKind::D && k == (unsigned char)SourceKind::K) ++nProm;
        else if (p == (unsigned char)SourceKind::K && k == (unsigned char)SourceKind::D) ++nDem;
        else if (p == (unsigned char)SourceKind::S && k == (unsigned char)SourceKind::D) ++nRecruit;
        prevKind[w] = k;
      }

      // Islands seen in either frame: an island that dissolved must still report its departures.
      std::vector<WIndex> islands;
      for (const auto& [g, v] : prev) islands.push_back(g);
      for (const auto& [g, v] : now)
        if (!has(islands, g)) islands.push_back(g);

      unsigned cap = 0, esc = 0;
      for (WIndex g : islands) {
        const std::vector<WIndex> empty;
        const std::vector<WIndex>& a = prev.count(g) ? prev.at(g) : empty;
        const std::vector<WIndex>& b = now.count(g) ? now.at(g) : empty;
        unsigned c = 0, e = 0;
        for (WIndex w : b) if (!has(a, w)) ++c;
        for (WIndex w : a) if (!has(b, w)) ++e;
        cap += c;
        esc += e;
        if (a.empty() && b.empty()) continue;
        fprintf(f, "%s,%u,%u,%u,%u,%u\n", run, (unsigned)g, frame,
                (unsigned)(b.empty() ? a.size() : b.size()), c, e);
      }
      released += esc;
      recruited += cap;
      printf("FRAME %3u islands=%zu captures=%u escapes=%u (cum cap=%u esc=%u)\n",
             frame, islands.size(), cap, esc, recruited, released);

      prev = now;
      if (frame >= frames) break;
      ++frame;
    }
    fclose(f);
    printf("MEASURED build=%s N=%u pairs=%u frames=%u total_captures=%u total_escapes=%u\n",
           build, n, pairs, frames, recruited, released);
    printf("MECHANISM build=%s releases(D->S)=%u promotions(D->K)=%u demotions(K->D)=%u "
           "recruits(S->D)=%u\n", build, nRel, nProm, nDem, nRecruit);
    printf("VERDICT build=%s release_active=%s turnover=%s\n", build,
           nRel > 0 ? "YES" : "NO", (recruited > 0 && released > 0) ? "YES" : "NO");
  } catch (const std::exception& e) {
    fprintf(stderr, "island_rate_probe: %s\n", e.what());
    return 2;
  }
  return 0;
}
