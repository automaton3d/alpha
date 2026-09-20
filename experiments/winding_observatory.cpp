// winding_observatory.cpp -- B0 of the topological programme: MEASURE, do not plant.
//
// QUESTION (B0).  Does the torus supply a measurable integer triple per island -- the
// winding numbers (n_x,n_y,n_z) of the island's centre of mass around the three
// non-contractible cycles of T^3 -- and is the instrument trustworthy?
//
// READ-ONLY.  This harness defines no rule and plants no global: it reads
// lcenters[], the source roles and the fixture's unwrapped accumulator
// (experiments/inertia_fixture.h) and reports.  Its predecessor
// (experiments/winding_probe.cpp, parked in attic/retired_experiments) did the
// opposite -- it planted a global label and asked whether a rule could read it --
// which is why its positive result was uninformative plumbing.
//
// QUANTITY.  unwrapped[w][k] is layer w's accumulated displacement along axis k from
// its initial position (the fixture adds the per-frame wrapped delta).  With the body
// members w < BODY sharing a centre,
//     D_k = (sum_{w<BODY} unwrapped[w][k] - start_k) / BODY     (centre of mass, exact)
//     n_k = round(D_k) mod edge_k                                (the winding number)
// and the spread of the members' own unwindings is reported too, because an island
// winding is only well defined while the members stay together.
//
// GEOMETRY.  The transport axis is the long side (15), the other two stay short (5), so
// RMAX = min(LY,LZ)/2 = 2 for every arm and the three axes are related by an exact
// relabelling -- which is what makes the permutation test below meaningful.
//
// ARMS.  baseline (no driver) | x+ y+ z+ (driver along +axis) | x- y- z- (reversed).
// VERDICT.  The instrument is validated if (i) no arm fails the accumulator check,
// (ii) the baseline does not wind, (iii) each driven arm winds in its own axis and not
// in the others, and (iv) reversing the driver negates the winding.  Nothing here is a
// statement about the model's physics: that starts at B1 (conservation across events).
//
// Usage: winding_observatory ARM [FRAMES] [CSV]

#include "inertia_fixture.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

using namespace inertia_fixture;

namespace {
constexpr unsigned LONG_SIDE = 15;   // transport axis
constexpr unsigned SHORT_SIDE = 5;   // the other two (sets RMAX = 2)
constexpr unsigned BODY = 3;         // 1 K + 2 D
constexpr unsigned PAIRS = 1;        // one reciprocal drive pair
constexpr unsigned SIEVE = 16384;    // reference sieve modulus

inline long long roundedDiv(long long num, unsigned den) {
  const long long h = (long long)(den / 2);
  return (num >= 0) ? ((num + h) / (long long)den) : (-((-num + h) / (long long)den));
}
inline int wrapMod(long long v, unsigned edge) {
  const long long m = (long long)edge;
  return (int)(((v % m) + m) % m);
}
}  // namespace

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    if (argc < 2) {
      fprintf(stderr, "usage: winding_observatory ARM [FRAMES] [CSV]\n"
                      "  ARM = baseline | x+ x- | y+ y- | z+ z-\n");
      return 1;
    }
    const std::string arm = argv[1];
    const unsigned frames = (argc > 2) ? (unsigned)atoi(argv[2]) : 80u;
    const char* csvPath = (argc > 3) ? argv[3] : nullptr;

    const bool baseline = (arm == "baseline");
    int axis = 0, direction = 1;
    if (!baseline) {
      if (arm.size() != 2 || (arm[0] != 'x' && arm[0] != 'y' && arm[0] != 'z') ||
          (arm[1] != '+' && arm[1] != '-'))
        throw std::runtime_error("ARM must be 'baseline' or one of x+/x-/y+/y-/z+/z-");
      axis = (arm[0] == 'x') ? 0 : ((arm[0] == 'y') ? 1 : 2);
      direction = (arm[1] == '+') ? 1 : -1;
    }
    const unsigned pairs = baseline ? 0u : PAIRS;

    unsigned lx = SHORT_SIDE, ly = SHORT_SIDE, lz = SHORT_SIDE;
    if (axis == 0) lx = LONG_SIDE;
    else if (axis == 1) ly = LONG_SIDE;
    else lz = LONG_SIDE;

    s2b_target = SIEVE;
    prepare(lx, ly, lz, BODY, pairs, axis, direction);
    Measurement measurement(BODY);

    std::array<long long, 3> start{0, 0, 0};
    for (int k = 0; k < 3; ++k) {
      long long s = 0;
      for (unsigned w = 0; w < BODY; ++w) s += measurement.unwrapped[w][k];
      start[k] = s;
    }
    const unsigned edges[3] = {ELX, ELY, ELZ};

    printf("WINDING_OBSERVATORY arm=%s geom=%ux%ux%u RMAX=%u BODY=%u pairs=%u "
           "sieve=%u frames=%u\n", arm.c_str(), lx, ly, lz, RMAX, BODY, pairs, SIEVE, frames);
    printf("PREDICTION: %s\n",
           baseline ? "no driver -> no transport: disp == 0, W == (0,0,0)"
                    : "driver along its own axis -> disp and W only in that component; reversed driver negates them");
    printf("START com=(%lld,%lld,%lld)/%u edge=(%u,%u,%u)\n",
           start[0], start[1], start[2], BODY, edges[0], edges[1], edges[2]);

    FILE* f = csvPath ? fopen(csvPath, "w") : nullptr;
    if (csvPath && !f) { perror(csvPath); return 3; }
    if (f)
      fprintf(f, "frame,tick,dx,dy,dz,nx,ny,nz,K,D,S,P,roles_changed_total,"
                 "member_spread,accum_mismatch,max_body_span,max_pair_gap\n");

    std::vector<unsigned char> prevKind(BODY);
    for (unsigned w = 0; w < BODY; ++w) prevKind[w] = (unsigned char)source(w).kind;

    unsigned frame = 0, roleChanges = 0, accumMismatch = 0;
    unsigned long long ticks = 0;
    std::array<long long, 3> disp{0, 0, 0};
    std::array<int, 3> wind{0, 0, 0};

    for (;;) {
      ++ticks;
      if (!simulation()) continue;

      // Integrity check of the instrument: recompute one step of the accumulator from the
      // fixture's OWN stored previous position (not from a fresh snapshot, which would be
      // identical to the current one) and compare with what the fixture stores.
      std::array<std::array<unsigned, 3>, BODY> prevPos{};
      std::array<std::array<long long, 3>, BODY> prevUnw{};
      for (unsigned w = 0; w < BODY; ++w)
        for (int k = 0; k < 3; ++k) {
          prevPos[w][k] = measurement.previous[w][k];
          prevUnw[w][k] = measurement.unwrapped[w][k];
        }

      measurement.sample();

      for (unsigned w = 0; w < BODY; ++w)
        for (int k = 0; k < 3; ++k) {
          const int d = wrappedDelta((int)prevPos[w][k], (int)lcenters[w][k], edges[k]);
          if (measurement.unwrapped[w][k] != prevUnw[w][k] + (long long)d) ++accumMismatch;
        }

      unsigned nk = 0, nd = 0, ns = 0, np = 0;
      bool changed = false;
      for (unsigned w = 0; w < W_USED; ++w) {
        const SourceKind kind = source(w).kind;
        nk += (kind == SourceKind::K);
        nd += (kind == SourceKind::D);
        ns += (kind == SourceKind::S);
        np += (kind == SourceKind::P);
        if (w < BODY && (unsigned char)kind != prevKind[w]) {
          changed = true;
          prevKind[w] = (unsigned char)kind;
        }
      }
      if (changed) ++roleChanges;

      long long sum[3] = {0, 0, 0};
      for (int k = 0; k < 3; ++k) {
        for (unsigned w = 0; w < BODY; ++w) sum[k] += measurement.unwrapped[w][k];
        disp[k] = roundedDiv(sum[k] - start[k], BODY);
        wind[k] = wrapMod(disp[k], edges[k]);
      }
      unsigned memberSpread = 0;
      for (int k = 0; k < 3; ++k) {
        long long lo = measurement.unwrapped[0][k], hi = lo;
        for (unsigned w = 1; w < BODY; ++w) {
          const long long off = measurement.unwrapped[w][k];
          if (off < lo) lo = off;
          if (off > hi) hi = off;
        }
        const unsigned sp = (unsigned)(hi - lo);
        if (sp > memberSpread) memberSpread = sp;
      }

      printf("FRAME %3u tick=%llu disp=(%lld,%lld,%lld) W=(%d,%d,%d) K=%u D=%u S=%u P=%u "
             "roles_changed_total=%u spread=%u mismatch=%u\n",
             frame, ticks, disp[0], disp[1], disp[2], wind[0], wind[1], wind[2],
             nk, nd, ns, np, roleChanges, memberSpread, accumMismatch);
      if (f)
        fprintf(f, "%u,%llu,%lld,%lld,%lld,%d,%d,%d,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
                frame, ticks, disp[0], disp[1], disp[2], wind[0], wind[1], wind[2],
                nk, nd, ns, np, roleChanges, memberSpread, accumMismatch,
                measurement.maxSpread, measurement.maxPairGap);

      if (frame >= frames) break;
      ++frame;
    }
    if (f) fclose(f);

    const bool baselineOk = !baseline || (disp[0] == 0 && disp[1] == 0 && disp[2] == 0);
    const bool ownAxisOnly = baseline ||
        (disp[axis] != 0 && disp[(axis + 1) % 3] == 0 && disp[(axis + 2) % 3] == 0);
    printf("MEASURED arm=%s disp_final=(%lld,%lld,%lld) W_final=(%d,%d,%d) "
           "roles_changed_total=%u accum_mismatch=%u\n",
           arm.c_str(), disp[0], disp[1], disp[2], wind[0], wind[1], wind[2],
           roleChanges, accumMismatch);
    printf("INSTRUMENT arm=%s accumulator=%s baseline_no_wind=%s driver_axis_only=%s\n",
           arm.c_str(),
           accumMismatch == 0 ? "OK" : "FAILED",
           baselineOk ? "OK" : "FAILED",
           ownAxisOnly ? "OK" : "FAILED");
  } catch (const std::exception& e) {
    fprintf(stderr, "winding_observatory: %s\n", e.what());
    return 2;
  }
  return 0;
}

