// cascade_probe.cpp -- A2: locate the membership cascade in the light frame, and identify the
// encounter that leaves exactly one delegate per charge word.
//
// READ-ONLY, and no new rule: it reads the ledger that chief_transition.h records only when
// CASCADE_LOG is defined.  The macro changes neither the lattice writes nor the encounter schedule,
// so the dynamics being measured are the reference dynamics.  Printed after every light frame:
//   - the global tally of role changes by kind (S->K election, S->D adoption, D->K promotion, K->D);
//   - per charge word: the number of changes, the tick window of that word's cascade, and its LAST
//     transition -- the encounter that ends the cascade and leaves the word with one delegate.
//
// Usage: cascade_probe [frames] [sieve] [EL]
#include "inertia_fixture.h"
#include "model/island_identity.h"
#include "model/chief_transition.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

using namespace inertia_fixture;
using namespace automaton;

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  try {
    unsigned frames = 2;
    if (argc >= 2) {
      frames = (unsigned)strtoul(argv[1], nullptr, 10);
      if (frames < 1 || frames > 64) throw std::runtime_error("frames must be 1..64");
    }
    int sieve = 16384;
    if (argc >= 3) sieve = (int)strtol(argv[2], nullptr, 10);
    unsigned L = 9;
    if (argc >= 4) L = (unsigned)strtoul(argv[3], nullptr, 10);

    if (!tryAllocate(L, 3u * L * L)) throw std::runtime_error(lastAllocationError);
    calculateParameters(L, 3u * L * L);
    initSimulation(0);      // the canonical superposed Platonic seed
    replicate();

    s2b_target = sieve;
    printf("CASCADE_PROBE L=%u W=%u RMAX=%u FRAME=%u sieve=%d frames=%u\n",
           L, W_USED, RMAX, FRAME, sieve, frames);

    unsigned frame = 0;
    while (frame < frames) {
      if (simulation()) {   // one light-frame journey completed
        ++frame;
        cascadeDump(frame, pulse_tick);
      }
    }
    printf("CASCADE_PROBE DONE frames=%u notes=%u\n", frame,
           (unsigned)cascadeLedger().size());
  } catch (const std::exception& e) {
    fprintf(stderr, "cascade_probe: %s\n", e.what());
    return 2;
  }
  return 0;
}
