// One physical propeller (two reciprocal P layers) and one K.
// Build only with build_propeller_k.bat; no coordinates are forced while running.
#ifdef PROPELLER_K_FLIGHT_EXPERIMENT
#error A test-only flight rule invalidates this acceptance experiment.
#endif

#include "inertia_fixture.h"
#include <cstdlib>
#include <cstring>
#include <string>

using namespace inertia_fixture;

namespace {
void require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

unsigned number(const char* argument, unsigned minimum, unsigned maximum) {
  char* end = nullptr;
  unsigned long value = std::strtoul(argument, &end, 10);
  if (!*argument || *end || value < minimum || value > maximum)
    throw std::runtime_error("invalid numeric argument");
  return static_cast<unsigned>(value);
}

// Initial geometry only. There is no position override in the evolution loop.
void place(unsigned w, unsigned x) {
  Cell seed = source(w);
  Cell& old = source(w);
  old.r2 = INF_R2; old.r = -1; old.u = 0;
  old.kind = SourceKind::S; old.parent = NO_PARENT;
  old.pair_idx = NO_PAIR; old.pair_count = 0;
  old.m[0] = old.m[1] = old.m[2] = 0;
  lcenters[w][0] = x;
  seed.x[0] = x;
  source(w) = seed;
}
}

int main(int argc, char** argv) {
  try {
    if (argc < 2 || argc > 9) {
      fprintf(stderr, "usage: propeller_k CSV [FRAMES=32] [SEP=8] [right|left] [same|foreign] [LX=51] [PHASE_P=0] [PHASE_K=PHASE_P]\n");
      return 1;
    }
    const unsigned frames = argc > 2 ? number(argv[2], 1, 10000) : 32;
    const unsigned sep = argc > 3 ? number(argv[3], 0, 1000) : 8;
    const std::string direction = argc > 4 ? argv[4] : "right";
    const std::string affinity = argc > 5 ? argv[5] : "same";
    const unsigned lx = argc > 6 ? number(argv[6], 9, 1001) : 51;
    const unsigned initialPhase = argc > 7 ? number(argv[7], 0, 5) : 0;
    const unsigned chiefPhase = argc > 8 ? number(argv[8], 0, 5) : initialPhase;
    require(direction == "right" || direction == "left", "direction must be right or left");
    require(affinity == "same" || affinity == "foreign", "affinity must be same or foreign");
    require(lx % 2 == 1 && sep + 4 < lx, "LX must be odd and leave room for the separation");
    const int sign = direction == "right" ? 1 : -1;
    const bool foreign = affinity == "foreign";

    // W=3: layer 0 is K, layers 1 and 2 are the two halves of ONE pair.
    // The short edges fix RMAX=3, independent of the length of the tube.
    prepare(lx, 7, 7, 1, 1, 0, sign, false, foreign);
    const unsigned pairX = sign > 0 ? 2 : lx - 3;
    const unsigned chiefX = sign > 0 ? pairX + sep : pairX - sep;
    place(0, chiefX); place(1, pairX); place(2, pairX);
    for (unsigned w = 0; w < 3; ++w) source(w).t = initialPhase;
    source(0).t = chiefPhase;
    replicate();
    const auto initial = lcenters;
    auto previous = lcenters;
    long long xK = chiefX, xP = pairX;
    unsigned contacts = 0, kickFrames = 0, wrapFrames = 0;
    long long firstContact = -1;
    long long capturedAt = !foreign && sep == 0 ? 0 : -1;
    unsigned coupledFrames = 0;
    unsigned long long ticks = 0;
    FILE* csv = std::fopen(argv[1], "w");
    require(csv != nullptr, "cannot open CSV output");
    setvbuf(csv, nullptr, _IONBF, 0);
    fprintf(csv, "frame,tick,k_x,p1_x,p2_x,k_unwrapped,p_unwrapped,dx_k,dx_p,phase_p,contact_p1_k,contact_p2_k,kick,wrap,distance\n");
    fprintf(csv, "0,0,%u,%u,%u,%lld,%lld,0,0,%u,0,0,0,0,%u\n",
            chiefX,pairX,pairX,xK,xP,effective_t(initialPhase),sep);

    for (unsigned frame = 1; frame <= frames;) {
      ++ticks;
      if (!simulation()) continue;

      // Observe actual lattice sources, independently of the centre cache
      // used by the transport implementation. Never repair the state here.
      std::array<unsigned,3> sourceCounts{0,0,0};
      for (const Cell& cell : lattice_curr) {
        if (cell.r2 != 0) continue;
        require(cell.w < 3, "invalid source layer");
        ++sourceCounts[cell.w];
        require(cell.x[0] == lcenters[cell.w][0] &&
                cell.x[1] == lcenters[cell.w][1] &&
                cell.x[2] == lcenters[cell.w][2], "source/cache mismatch");
      }
      for (unsigned count : sourceCounts) require(count == 1, "missing or duplicate source");

      int dx[3]; bool wrapped = false;
      for (unsigned w = 0; w < 3; ++w) {
        const int raw = (int)lcenters[w][0] - (int)previous[w][0];
        dx[w] = wrappedDelta(previous[w][0], lcenters[w][0], ELX);
        wrapped = wrapped || raw != dx[w];
        require(lcenters[w][1] == initial[w][1] && lcenters[w][2] == initial[w][2],
                "unexpected transverse displacement");
      }
      if (dx[1] != sign || dx[2] != sign) {
        fprintf(stderr, "FAIL frame=%u tick=%llu dx_K=%d dx_P1=%d dx_P2=%d expected_P=%d phase_P=%u phase_K=%u\n",
                frame,ticks,dx[0],dx[1],dx[2],sign,initialPhase,chiefPhase);
        fclose(csv);
        throw std::runtime_error("ordinary kernel failed maximum-speed flight");
      }
      require(lcenters[1] == lcenters[2], "pair halves separated");
      require(dx[0] == 0 || dx[0] == sign, "unexpected K displacement");
      require(source(0).kind == SourceKind::K && source(0).ch == 0x08 && source(0).a == 0,
              "K identity changed");
      for (unsigned w : {1u, 2u}) {
        const Cell& p = source(w);
        require(p.kind == SourceKind::P && p.pair_idx == (w == 1 ? 2u : 1u) && p.pair_count == 1,
                "pair identity changed");
        require(p.a == (foreign ? 1u : 0u) && p.m[0] == sign * (int)RMAX &&
                p.m[1] == 0 && p.m[2] == 0, "pair affinity or direction changed");
      }
      const bool c1 = hadInternalContact(0,1), c2 = hadInternalContact(0,2);
      const bool contact = c1 || c2;
      const bool kick = dx[0] != 0;
      require(!kick || contact, "K moved without a recorded internal contact");
      require(!foreign || !kick, "foreign pair propelled K");
      if (contact) { ++contacts; if (firstContact < 0) firstContact = frame; }
      kickFrames += kick; wrapFrames += wrapped;
      xK += dx[0]; xP += dx[1];
      if (!foreign) {
        require(sign * (xK - xP) >= 0, "P overtook K");
        if (capturedAt >= 0) {
          require(dx[0] == sign && xK == xP,
                  "coupled P/K lost maximum-speed motion");
          ++coupledFrames;
        } else if (xK == xP) capturedAt = frame;
      }
      const unsigned phase = effective_t(source(1).t);
      const int distance = std::abs(wrappedDelta(lcenters[1][0],lcenters[0][0],ELX));
      fprintf(csv, "%u,%llu,%u,%u,%u,%lld,%lld,%d,%d,%u,%d,%d,%d,%d,%d\n",frame,ticks,
              lcenters[0][0],lcenters[1][0],lcenters[2][0],xK,xP,dx[0],dx[1],phase,c1,c2,kick,wrapped,distance);
      previous = lcenters;
      ++frame;
    }
    fclose(csv);
    if (!foreign && frames >= 6 * sep + 12)
      require(capturedAt >= 0 && coupledFrames >= 6,
              "no sustained capture within the observation window");
    printf("P_K frames=%u ticks=%llu pair_speed=%+.6f cells/light-frame K_mean_speed=%+.6f first_contact=%lld contact_frames=%u kick_frames=%u wrap_frames=%u\n",
           frames,ticks,(double)(xP-pairX)/frames,(double)(xK-chiefX)/frames,firstContact,contacts,kickFrames,wrapFrames);
    printf("capture_frame=%lld coupled_frames=%u initial_phase=%u\n",
           capturedAt,coupledFrames,initialPhase);
    printf("PASS: pair speed, contact-only K motion, no overtaking and sustained capture checks; identities preserved.\nCSV: %s\n",argv[1]);
    return 0;
  } catch (const std::exception& e) {
    fprintf(stderr, "propeller_k: %s\n", e.what());
    return 2;
  }
}
