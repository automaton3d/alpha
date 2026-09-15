// pair_channel_probe.cpp -- menu item 3, closed by measurement.
//
// The reachability table of CHARGE_SPECTRUM.md is algebra: eight reachable words,
// three of the six pair channels closed for every admissible word, the photon
// channel needing a word and its conjugate.  This probe measures which words the
// pair branch of encounter() actually accepts.
//
// It plants n co-located, in-phase S sources (so samePos and sameT hold) in a
// small tube, opens the sieve gate directly (s2b_target, as island_census does,
// plus s2B preset on the centre cells), and reports every formation.  What it
// measures is the CHARGE RULE acceptance, not the sieve timing.
//
// Modes and the prediction from the reachability table:
//   r3       0x00 0x00     1 pair -- R3 neutrino-type, both words from the seed
//   r6       0x2A 0x2A     1 pair -- R6 up-quark-type, both from the seed
//   r2       0x00 0x1F     1 pair -- R2 photon-type, 0x1F is a conjugate word
//   r2b      0x2A 0x35     1 pair -- R2 again, conjugate of 0x2A
//   r1       0x00 0x3F     1 pair -- R1 graviton-type, needs 0x3F (not in M)
//   r5       0x11 0x11     1 pair -- R5 exists in code, 0x11 violates q^w0=w1
//   cross    0x00 0x19     0 pairs -- two seed words, different colour
//   seedset  seed words + repeats -> exactly 3 pairs: 0x00/0x00, 0x2A/0x2A,
//           0x2E/0x2E, i.e. only the channels the seed can reach.
//
// Build: experiments\build_pair_channel_probe.bat (defines PAIR_WORD_LOG)
// Run:   build\pair_channel\pair_channel_probe.exe [frames] [mode]

#include "inertia_fixture.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace inertia_fixture;
using namespace automaton;

namespace {

struct Mode { const char* name; std::vector<unsigned char> words; unsigned expected; };

// The canonical seed's eight words plus one repeat of 0x00 (81 = 8*10+1 puts the
// extra family on 0x00), one of 0x2A and one of 0x2E -- i.e. the words that can
// find a partner of their own word in a real seed.
std::vector<unsigned char> seedSetWords()
{
  return { 0x00, 0x19, 0x2A, 0x33, 0x04, 0x1D, 0x2E, 0x37,
           0x00, 0x2A, 0x2E };
}

std::vector<Mode> modes()
{
  return {
    { "r3",      { 0x00, 0x00 }, 1 },
    { "r6",      { 0x2A, 0x2A }, 1 },
    { "r2",      { 0x00, 0x1F }, 1 },
    { "r2b",     { 0x2A, 0x35 }, 1 },
    { "r1",      { 0x00, 0x3F }, 1 },
    { "r5",      { 0x11, 0x11 }, 1 },
    { "r4",      { 0x3F, 0x3F }, 0 },   // R4: identical words -> election wins
    { "r6b",     { 0x2E, 0x2E }, 0 },   // R6 on the other seed colour: same
    { "cross",   { 0x00, 0x19 }, 0 },
    { "seedset", seedSetWords(), 3 },
  };
}

void plantLayer(unsigned w, unsigned char ch, const std::array<unsigned,3>& centre)
{
  lcenters[w] = centre;
  for (unsigned x = 0; x < ELX; ++x)
  for (unsigned y = 0; y < ELY; ++y)
  for (unsigned z = 0; z < ELZ; ++z)
  {
    Cell& c = getCell(lattice_curr, x, y, z, w);
    c = Cell{};
    c.w = w; c.ch = ch;
    c.x[0] = x; c.x[1] = y; c.x[2] = z; c.x[3] = w;
    c.a = W_USED; c.leader_w = NO_LEADER_W; c.parent = NO_PARENT;
    c.r2 = INF_R2; c.r = -1; c.kind = SourceKind::S;
    c.t = 0; c.f = 0; c.k = 0;
  }
  // Centre cell of the layer: the source.  s2B is preset so the electroweak
  // gate of encounter() is open from the first tick.
  Cell& s = getCell(lattice_curr, centre[0], centre[1], centre[2], w);
  s.r2 = 0; s.r = 0; s.u = 2048; s.v = 0;
  s.s2B = true; s.pB = true;
  s.kind = SourceKind::S; s.parent = NO_PARENT; s.leader_w = NO_LEADER_W;
  s.ch = ch;
}

} // namespace

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    const unsigned frames = (argc > 1) ? (unsigned)atoi(argv[1]) : 8u;
    const char* want = (argc > 2) ? argv[2] : "r3";

    const std::vector<Mode> all = modes();      // keep the storage alive
    const Mode* mode = nullptr;
    for (const Mode& m : all)
      if (strcmp(m.name, want) == 0) mode = &m;
    if (!mode) { fprintf(stderr, "mode must be one of r3 r6 r2 r2b r1 r5 cross seedset\n"); return 2; }

    constexpr unsigned LX = 15, LY = 5, LZ = 5;
    const unsigned n = (unsigned)mode->words.size();
    if (!tryAllocateTube(LX, LY, LZ, n))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    s2b_target = 1;                      // open the sieve gate (see island_census)

    const std::array<unsigned,3> centre = { LX / 2, LY / 2, LZ / 2 };
    for (unsigned w = 0; w < n; ++w) plantLayer(w, mode->words[w], centre);
    replicate();

    printf("PAIR_CHANNEL mode=%s layers=%u tube=%ux%ux%u RMAX=%u sieve=%d "
           "expected_pairs=%u\n", mode->name, n, ELX, ELY, ELZ, RMAX,
           (int)s2b_target, mode->expected);
    for (unsigned w = 0; w < n; ++w)
      printf("  planted w=%u ch=0x%02X\n", w, (unsigned)mode->words[w]);

    unsigned frame = 0;
    for (;;)
    {
      unsigned np = 0;
      for (unsigned w = 0; w < n; ++w) np += (source(w).kind == SourceKind::P);
      printf("FRAME %2u P_half=%u\n", frame, np);
      if (frame >= frames) break;
      while (!simulation()) {}
      ++frame;
    }

    // Which pairs exist, and of which words?
    unsigned pairs = 0;
    for (unsigned a = 0; a < n; ++a)
      for (unsigned b = a + 1; b < n; ++b)
      {
        const Cell& sa = source(a);
        const Cell& sb = source(b);
        if (sa.kind == SourceKind::P && sb.kind == SourceKind::P &&
            sa.pair_idx == b && sb.pair_idx == a)
        {
          ++pairs;
          printf("PAIR %u-%u words 0x%02X/0x%02X\n", a, b, (unsigned)sa.ch, (unsigned)sb.ch);
        }
      }
    printf("MEASURED [%s] pairs=%u expected=%u -> %s\n", mode->name, pairs,
           mode->expected, (pairs == mode->expected) ? "MATCHES" : "DIFFERS");
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}
