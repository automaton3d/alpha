// probe-rest: is the drive-pair dressing a rest-mass contribution, and is a
// dressing dynamically maintained (self-formed) or a frozen initial condition?
//
// A prepared 1K+(N-1)D island is dressed with CANCEL photon pairs whose
// transport vectors form either a CLOSED set (sum m = 0 -> rest dressing) or an
// OPEN one (sum m != 0 -> resultant).  IMPULSE aligned pairs supply the
// response probe.  Two layouts distinguish the two readings of DRIVE_PAIR_SHELL:
//
//   stack : the cancelling pairs sit on the island centre (internal
//           multiplicity; pair_count semantics, frequency = 2n)
//   shell : the cancelling pairs sit on distinct W addresses whose spatial
//           positions cover the model's own integer-radius shell r = isqrt(r2)
//           around the island (the r used by isOrphanShell)
//
// Discriminators (see DRIVE_PAIR_SHELL.md):
//   - v_per_impulse_pair vs CANCEL   -> does the dressing add inertia?
//   - stack vs shell at equal CANCEL -> spatial shell, or multiplicity only?
//   - OPEN dressing (odd CANCEL)     -> does the resultant relax to zero?
//
// This is a prepared-initial-condition instrument.  It does NOT show that the
// Platonic seed self-assembles an island or a dressing.
#include "inertia_fixture.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace inertia_fixture;

namespace {

// Offsets of the model's integer-radius shell: r = isqrt(x^2+y^2+z^2) == radius,
// i.e. lo <= |d|^2 < hi.  Same metric as the cell field r and isOrphanShell.
std::vector<std::array<int, 3>> shellOffsets(unsigned radius) {
  std::vector<std::array<int, 3>> out;
  if (radius == 0) { out.push_back({0, 0, 0}); return out; }
  const int lo = (int)radius * (int)radius;
  const int hi = (int)(radius + 1) * (int)(radius + 1);
  const int m = (int)std::ceil(std::sqrt((double)(hi - 1)));
  for (int x = -m; x <= m; ++x)
    for (int y = -m; y <= m; ++y)
      for (int z = -m; z <= m; ++z) {
        const int s = x * x + y * y + z * z;
        if (s >= lo && s < hi) out.push_back({x, y, z});
      }
  return out;
}

// Dressing state: pair population, closure (sum of transport vectors over the
// first half of each pair), and how many reciprocal links are still intact.
struct Dressing {
  unsigned pairs = 0;      // surviving kind == P sources, counted once per pair
  unsigned intact = 0;     // pairs whose pair_idx is still mutual
  unsigned freeS = 0;      // non-P sources beyond the island (unformed material)
  int sum_m[3] = {0, 0, 0};
};

Dressing dressing(unsigned n) {
  Dressing d;
  for (unsigned w = n; w < W_USED; ++w) {
    const Cell& s = source(w);
    if (s.kind != SourceKind::P) { ++d.freeS; continue; }
    ++d.pairs;
    if (s.pair_idx < W_USED && source(s.pair_idx).pair_idx == w) ++d.intact;
    for (int k = 0; k < 3; ++k) d.sum_m[k] += s.m[k];
  }
  d.pairs /= 2;            // two halves per pair
  d.intact /= 2;
  return d;
}

} // namespace

int main(int argc, char** argv) {
  if (argc < 10) {
    fprintf(stderr,
            "usage: rest_shell_probe LX LY LZ N CANCEL IMPULSE [stack|shell] RADIUS FRAMES "
            "[SIEVE] [CSV] [BURN]\n");
    return 1;
  }
  const unsigned lx = atoi(argv[1]), ly = atoi(argv[2]), lz = atoi(argv[3]);
  const unsigned n = atoi(argv[4]);
  const unsigned cancel = atoi(argv[5]);
  const unsigned impulse = atoi(argv[6]);
  const char* layout = argv[7];
  const unsigned radius = atoi(argv[8]);
  const unsigned frames = atoi(argv[9]);
  s2b_target = argc > 10 ? atoi(argv[10]) : 16384;
  const char* path = argc > 11 ? argv[11] : "rest_shell.csv";
  const unsigned burn = argc > 12 ? atoi(argv[12]) : frames / 4;

  const bool shell = strcmp(layout, "shell") == 0;
  const bool vacuum = strcmp(layout, "vacuum") == 0;
  if (strcmp(layout, "stack") != 0 && !shell && !vacuum) return 2;
  if (lx < 5 || ly < 5 || lz < 5 || !(lx % 2) || !(ly % 2) || !(lz % 2)) return 2;
  if (n < 1 || n > 32 || cancel > 32 || impulse > 32) return 2;
  if (frames <= burn) return 2;

  const unsigned pairs = cancel + impulse;
  if (!tryAllocateTube(lx, ly, lz, n + 2 * pairs)) {
    fprintf(stderr, "%s\n", lastAllocationError.c_str());
    return 3;
  }
  initSimulation(0);

  const unsigned axis = 0;
  const std::array<unsigned, 3> centre{lx / 2, ly / 2, lz / 2};
  const unsigned edges[3] = {lx, ly, lz};
  const auto offsets = shellOffsets(shell ? radius : 0u);

  for (unsigned w = 0; w < W_USED; ++w) {
    const bool freeLayer = w >= n;               // past the island: pair material
    lcenters[w] = centre;
    const unsigned char charge = freeLayer ? (((w - n) % 2) ? 0x1f : 0x00) : 0x08;
    for (unsigned x = 0; x < lx; ++x)
      for (unsigned y = 0; y < ly; ++y)
        for (unsigned z = 0; z < lz; ++z) {
          Cell& c = getCell(lattice_curr, x, y, z, w);
          c = Cell{};
          c.w = w;
          c.ch = charge;
          c.x[0] = x; c.x[1] = y; c.x[2] = z; c.x[3] = w;
          c.a = 0; c.leader_w = 0;
        }
    Cell& s = source(w);
    s.r2 = 0; s.r = 0; s.u = 2048;
    if (!freeLayer) {
      s.kind = (w == 0) ? SourceKind::K : SourceKind::D;
      s.parent = (w == 0) ? NO_PARENT : 0u;
      continue;
    }
    if (vacuum) {
      // Free pair material: a complementary-charge SINGLETON layer.  Nothing is
      // a pair yet -- the dynamics must form it (sieve/s2B) before it can be a
      // dressing at all.  This is the self-formation test.
      s.kind = SourceKind::S;
      s.parent = NO_PARENT;
      s.a = 0; s.leader_w = 0;
      continue;
    }
    const unsigned p = (w - n) / 2;                 // pair index
    s.kind = SourceKind::P;
    s.parent = 0u;
    s.pair_idx = n + 2 * p + (((w - n) % 2) ? 0u : 1u);
    s.pair_count = 1;
    // Cancelling group: alternating signs -> sum m = 0 when CANCEL is even.
    // Impulse group: aligned, so it supplies the net resultant under test.
    const int direction = (p < cancel) ? ((p % 2) ? -1 : +1) : +1;
    s.m[axis] = (int)RMAX * direction;
    if (shell && p < cancel) {
      const auto& o = offsets[p % offsets.size()];
      for (int k = 0; k < 3; ++k)
        lcenters[w][k] = (unsigned)((((int)centre[k] + o[k]) % (int)edges[k] +
                                     (int)edges[k]) % (int)edges[k]);
    }
  }
  replicate();

  const Dressing seeded = dressing(n);

  // Own tracker of EVERY source centre (body + dressing), so that a recoil of
  // the dressing relative to the body is visible rather than assumed away.
  std::vector<std::array<unsigned, 3>> previous = lcenters;
  Measurement measurement(n);
  FILE* f = fopen(path, "w");
  if (!f) { perror(path); return 4; }
  fprintf(f, "frame,tick,measured,v_body_x,v_all_x,body_x,pairs_P,intact,sum_m_x,span,gap\n");

  std::array<double, 3> sumBody{0, 0, 0}, sumAll{0, 0, 0};
  std::array<double, 3> firstBody{0, 0, 0}, secondBody{0, 0, 0};
  unsigned long long ticks = 0;
  for (unsigned frame = 1; frame <= frames;) {
    ++ticks;
    if (!simulation()) continue;
    const auto velocity = measurement.sample();

    std::array<double, 3> allVelocity{0, 0, 0};
    for (unsigned w = 0; w < W_USED; ++w)
      for (int k = 0; k < 3; ++k) {
        allVelocity[k] +=
            (double)wrappedDelta(previous[w][k], lcenters[w][k], edges[k]) / (double)W_USED;
        previous[w][k] = lcenters[w][k];
      }

    const Dressing d = dressing(n);
    std::array<double, 3> bodyPosition{0, 0, 0};
    for (unsigned w = 0; w < n; ++w)
      for (int k = 0; k < 3; ++k) bodyPosition[k] += (double)measurement.unwrapped[w][k] / n;

    const bool measured = frame > burn;
    if (measured)
      for (int k = 0; k < 3; ++k) {
        sumBody[k] += velocity[k];
        sumAll[k] += allVelocity[k];
        (frame <= burn + (frames - burn) / 2 ? firstBody : secondBody)[k] += velocity[k];
      }
    fprintf(f, "%u,%llu,%u,%.9f,%.9f,%.9f,%u,%u,%d,%u,%u\n",
            frame, ticks, measured ? 1u : 0u, velocity[0], allVelocity[0],
            bodyPosition[0], d.pairs, d.intact, d.sum_m[0],
            measurement.maxSpread, measurement.maxPairGap);
    ++frame;
  }
  fclose(f);

  const unsigned samples = frames - burn;
  const double era = 2.0 * (double)RMAX;
  const double meanBody = sumBody[0] / samples, meanAll = sumAll[0] / samples;
  const unsigned firstN = samples / 2, secondN = samples - firstN;
  const Dressing finalState = dressing(n);
  printf("PROBE_REST N=%u cancel=%u impulse=%u layout=%s radius=%u RMAX=%u era=%.0f burn=%u samples=%u\n",
         n, cancel, impulse, layout, radius, RMAX, era, burn, samples);
  printf("MEAN_V body_x=%.9f all_x=%.9f cells_per_light_frame body_x_per_era=%.6f\n",
         meanBody, meanAll, meanBody * era);
  if (impulse)
    printf("IMPULSE_RESPONSE v_per_impulse_pair=%.9f m_eff_proxy=%.6f\n",
           meanBody / (double)impulse, (double)impulse / (meanBody * era + 1e-30));
  if (firstN && secondN)
    printf("WINDOWS body_first=%.9f body_second=%.9f all_mean=%.9f\n",
           firstBody[0] / firstN, secondBody[0] / secondN, meanAll);
  printf("DRESSING seeded_pairs=%u seeded_sum_m_x=%d final_pairs=%u intact_links=%u final_sum_m_x=%d "
         "free_singleton_layers=%u max_body_span=%u max_pair_gap=%u\n",
         seeded.pairs, seeded.sum_m[0], finalState.pairs, finalState.intact,
         finalState.sum_m[0], finalState.freeS, measurement.maxSpread, measurement.maxPairGap);

  // Did the automaton produce a DIRECTION of its own?  homb_events counts the
  // ported carrier/homer producers (HOMB_PRODUCER_FSM; zero in the reference),
  // sources_c_nonzero counts sources whose relocation vector left zero, and
  // sources_homB counts live homing flags.
  long long cNonzero = 0, hombTrue = 0;
  for (unsigned w = 0; w < W_USED; ++w) {
    const Cell& s = source(w);
    if (s.c[0] || s.c[1] || s.c[2]) ++cNonzero;
    if (s.homB) ++hombTrue;
  }
  printf("PRODUCERS homb_events=%lld sources_c_nonzero=%lld sources_homB=%lld\n",
         (long long)homb_events, cNonzero, hombTrue);
  return 0;
}
