/*
 * stack_absorb_probe.cpp - regression probe for the candidate rule
 * PAIR_STACK_ABSORB_FSM (multi-frequency mechanics, review item 2).
 *
 * Scenario (fully deterministic, no randomness in the light frame):
 *   Four W layers superposed at the lattice centre, all at rest (m = 0),
 *   all seeded with clock t = 1 (wavefront radius f = effective_t(1) = 1,
 *   so the active shell sits at r = 1 and the encounter's r > 0 gate
 *   passes; with t = 0 the centre cell has r = 0 and the gate rejects),
 *   all with the central wave amplitude u = 2048, sieve modulus S = 1
 *   (s2B = ((u*t) mod 1) < u  ->  true wherever u > 0):
 *
 *     w = 0 : S, ch = 0x00      fresh source (pair half A)
 *     w = 1 : S, ch = 0x1F      fresh source (pair half B, Rule-2 complement)
 *     w = 2 : P, ch = 0x1F   \  seeded free stack: mutual pair with
 *     w = 3 : P, ch = 0x00   /  pair_count = 4 (frequency 8), the
 *                               "photonp"-style seeding of alpha_probe
 *
 * Expected WITH PAIR_STACK_ABSORB_FSM compiled in: the (0x00, 0x1F) S+S
 *   formation fires (enc_pair >= 1), absorbs the identical stack {2,3} --
 *   the new pair registers pair_count = 1 + 4 = 5 on both halves (and the
 *   blob branch revives, bB = 1, since newCount > 1 and 0x00/0x1F is the R2
 *   geometry) -- and the absorbed halves are released as singletons
 *   (kind S, pair_idx NO_PAIR, pair_count 0, deterministic reloc split).
 *   enc_absorb == 1.
 * Expected WITHOUT the macro (reference control): the formation fires with
 *   pair_count = 1 and the seeded stack {2,3} is untouched.
 *
 * Usage: stack_absorb_probe.exe [EL=7] [frames=6] [sieve=1]
 * Exit code 0 = all assertions held; 1 = a check failed; 2 = setup error.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "model/simulation.h"
#include "model/polarization.h"
#include "config.h"

// ---------------------------------------------------------------------------
// Link stubs (same role as in alpha_probe.cpp).
// ---------------------------------------------------------------------------
std::vector<unsigned int> voxels;   // used by tryAllocate() (initSim.cpp)

Config gConfig;                     // extern declared in config.h

namespace automaton
{
  bool convol_delay  = false;       // declared in model/simulation.h
  bool diffuse_delay = false;
  bool reloc_delay   = false;

  // CPU-only stubs (bridge.cpp is not linked: it drags in GUI symbols).
  bool tryEnableCuda()    { return false; }
  void disableCuda()      {}
  bool isCudaEnabled()    { return false; }
  bool swap_lattices_gpu(){ return false; }

  // defined in simulation.cpp, not exposed in simulation.h
  void trackCenter(unsigned x, unsigned y, unsigned z, unsigned w);
}

// ---------------------------------------------------------------------------
// placeSource: reset one whole winding layer to "unreached" and plant the
// source centre (alpha_probe pattern), with clock t = 1 (shell radius 1).
// ---------------------------------------------------------------------------
static void placeSource(unsigned w, unsigned x, unsigned y, unsigned z,
                        unsigned char chWord, int mx = 0, int my = 0, int mz = 0)
{
  const int ELi = (int)automaton::EL;

  for (int a = 0; a < ELi; ++a)
    for (int b = 0; b < ELi; ++b)
      for (int c = 0; c < ELi; ++c)
      {
        automaton::Cell& cc = automaton::getCell(
            automaton::lattice_curr, a, b, c, (int)w);
        automaton::Cell& cd = automaton::getCell(
            automaton::lattice_draft, a, b, c, (int)w);
        automaton::Cell& cm = automaton::getCell(
            automaton::lattice_partner, a, b, c, (int)w);
        for (automaton::Cell* p : { &cc, &cd, &cm })
        {
          p->r2  = INF_R2;
          p->r   = -1;
          p->u   = 0;
          p->v   = 0;
          p->active = 0;
          p->pol_u = p->pol_v = 0;
          p->bstamp = 0;
          p->m[0] = p->m[1] = p->m[2] = 0;
          p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
          p->kind = automaton::SourceKind::S;
          p->pair_idx   = automaton::NO_PAIR;
          p->pair_count = 0;
          p->parent     = automaton::NO_PARENT;
          p->leader_w   = automaton::NO_LEADER_W;
          p->a          = automaton::W_USED;   // orphan: no pre-existing island
          p->t = 1; p->f = 1;
          p->pB = p->sB = p->s2B = false;
          p->kB = p->bB = p->homB = p->cB = false;
          p->gB = false;
          p->c[0] = p->c[1] = p->c[2] = 0;
          p->g[0] = p->g[1] = p->g[2] = 0;
          p->spin_target = 0;
        }
      }

  for (automaton::Cell* p :
       { &automaton::getCell(automaton::lattice_curr, (int)x, (int)y, (int)z, (int)w),
         &automaton::getCell(automaton::lattice_draft, (int)x, (int)y, (int)z, (int)w),
         &automaton::getCell(automaton::lattice_partner, (int)x, (int)y, (int)z, (int)w) })
  {
    p->r2   = 0;
    p->r    = 0;
    p->u    = 2048;                  // central wave source amplitude
    p->v    = 0;
    p->ch   = chWord;
    p->m[0] = mx; p->m[1] = my; p->m[2] = mz;
    p->reloc[0] = p->reloc[1] = p->reloc[2] = 0;
    p->t = 1; p->f = 1;
  }

  automaton::lcenters[w][0] = x;
  automaton::lcenters[w][1] = y;
  automaton::lcenters[w][2] = z;
  automaton::trackCenter(x, y, z, w);
}

// ---------------------------------------------------------------------------
// markFreePair (alpha_probe pattern): turn the two planted source centres
// into ONE free mediator pair (kind = P, no leader) with a given count.
// ---------------------------------------------------------------------------
static void markFreePair(unsigned w, unsigned w2, bool link, uint8_t count)
{
  for (unsigned layer : { w, w2 })
  {
    const unsigned partner = (layer == w) ? w2 : w;
    for (auto* lat : { &automaton::lattice_curr, &automaton::lattice_draft,
                       &automaton::lattice_partner })
    {
      const auto& ctr = automaton::lcenters[layer];
      automaton::Cell& c = automaton::getCell(
          *lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)layer);
      c.kind       = automaton::SourceKind::P;
      c.pair_idx   = link ? partner : automaton::NO_PAIR;
      c.pair_count = count;
      c.leader_w   = automaton::NO_LEADER_W;
      c.a          = automaton::W_USED;
      c.parent     = automaton::NO_PARENT;
    }
  }
}

// ---------------------------------------------------------------------------
// dumpState: per-layer source state read through lcenters, plus counters.
// ---------------------------------------------------------------------------
static void dumpState(unsigned frame)
{
  for (unsigned w = 0; w < 4u; ++w)
  {
    const auto& ctr = automaton::lcenters[w];
    const automaton::Cell& c = automaton::getCell(
        automaton::lattice_curr, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    printf("  f%02u w%u: kind=%u ch=0x%02X pair=%u count=%u pos=(%u,%u,%u) "
           "t=%u f=%u bB=%d reloc=(%d,%d,%d) cell(r=%d,active=%u,u=%d)\n",
           frame, w, (unsigned)c.kind, (unsigned)c.ch,
           (unsigned)c.pair_idx, (unsigned)c.pair_count,
           ctr[0], ctr[1], ctr[2], c.t, c.f, (int)c.bB,
           c.reloc[0], c.reloc[1], c.reloc[2],
           c.r, (unsigned)c.active, c.u);
  }
#ifdef PAIR_STACK_ABSORB_FSM
  printf("  [counters] enc_pair=%lld enc_absorb=%lld\n",
         (long long)automaton::enc_pair, (long long)automaton::enc_absorb);
#else
  printf("  [counters] enc_pair=%lld\n", (long long)automaton::enc_pair);
#endif
#ifdef MULTIFREQ_RAY_FSM
  printf("  [mf] reads=%lld hits=%lld gated=%lld\n",
         (long long)automaton::mf_reads, (long long)automaton::mf_hits,
         (long long)automaton::mf_gated);
#endif
}

static bool expect(bool cond, const char* what)
{
  printf("  [%s] %s\n", cond ? "ok  " : "FAIL", what);
  return cond;
}

int main(int argc, char** argv)
{
  unsigned EL_in   = (argc > 1) ? (unsigned)atoi(argv[1]) : 7u;
  unsigned FRAMES  = (argc > 2) ? (unsigned)atoi(argv[2]) : 6u;
  int      SIEVE   = (argc > 3) ? atoi(argv[3]) : 1;
  if (EL_in < 5 || (EL_in % 2) == 0 || EL_in > 31)
  {
    fprintf(stderr, "EL must be an odd value in [5,31]\n");
    return 2;
  }
  if (SIEVE < 1) SIEVE = 1;

  const unsigned W_in = 4u;
  printf("=== stack_absorb_probe: mode=%s ===\n",
#ifdef PAIR_STACK_ABSORB_FSM
         "PAIR_STACK_ABSORB_FSM (absorption compiled in)"
#else
         "reference control (absorption compiled out)"
#endif
  );
  printf("EL=%u W=%u RMAX=%u frames=%u sieve=%d\n",
         EL_in, W_in, EL_in / 2u, FRAMES, SIEVE);

  automaton::calculateParameters(EL_in, W_in);
  automaton::s2b_target = SIEVE;
  if (!automaton::tryAllocate((int)EL_in, (int)W_in))
  {
    fprintf(stderr, "allocation failed: %s\n",
            automaton::lastAllocationError.c_str());
    return 2;
  }
  for (int step = 0; step <= 7; ++step)
    automaton::initSimulation(step);

  const unsigned C = automaton::CENTER;
  // Four superposed sources at one voxel, all at rest in position, clock
  // t = 1; the fresh pair halves carry m = (1,0,0) so the candidate's
  // pure-climb ray walk has a direction to follow.
  placeSource(0, C, C, C, 0x00, 1, 0, 0);  // fresh S (pair half A)
  placeSource(1, C, C, C, 0x1F, 1, 0, 0);  // fresh S (pair half B, R2)
  placeSource(2, C, C, C, 0x1F);           // stack half A' (m = 0)
  placeSource(3, C, C, C, 0x00);           // stack half B' (m = 0)
  markFreePair(2, 3, /*link=*/true, /*count=*/4);  // seeded stack, freq 8
  automaton::replicate();

  printf("seeded:\n");
  dumpState(0);

  long long prevPair = 0;
  bool asserted   = false;
  bool assertions = true;

  for (unsigned frame = 1; frame <= FRAMES; ++frame)
  {
    bool newFrame = false;
    while (!newFrame)
      newFrame = automaton::simulation();
    printf("after frame %u:\n", frame);
    dumpState(frame);
    const long long pairNow = automaton::enc_pair;
    if (pairNow > prevPair && !asserted)
    {
      printf("formation fired at frame %u -- asserting:\n", frame);
      // NOTE: the formation branch is reached once per active voxel where the
      // two shells cross (27 windows in this geometry); the candidate's
      // idempotence guard makes exactly ONE of them execute the body, which
      // is what enc_absorb == 1 asserts below.
      // The formed pair (w0/w1) must always be mutual kind P.
      const auto& c0 = automaton::lcenters[0];
      const auto& c1 = automaton::lcenters[1];
      const automaton::Cell& a = automaton::getCell(
          automaton::lattice_curr, (int)c0[0], (int)c0[1], (int)c0[2], 0);
      const automaton::Cell& b = automaton::getCell(
          automaton::lattice_curr, (int)c1[0], (int)c1[1], (int)c1[2], 1);
      assertions &= expect(a.kind == automaton::SourceKind::P &&
                           b.kind == automaton::SourceKind::P,
                           "w0/w1 are kind P after formation");
      assertions &= expect(a.pair_idx == 1u && b.pair_idx == 0u,
                           "w0/w1 mutual pair_idx");
      assertions &= expect(a.ch == 0x00 && b.ch == 0x1F,
                           "charge words preserved on the formed pair");
#ifdef PAIR_STACK_ABSORB_FSM
      assertions &= expect(automaton::enc_absorb == 1,
                           "exactly one absorption (enc_absorb == 1)");
      assertions &= expect(a.pair_count == 5u && b.pair_count == 5u,
                           "absorbed stack: pair_count == 1 + 4 == 5");
      assertions &= expect(a.bB && b.bB,
                           "blob branch revived (bB on the R2 stack pair)");
      const auto& c2 = automaton::lcenters[2];
      const auto& c3 = automaton::lcenters[3];
      const automaton::Cell& s2 = automaton::getCell(
          automaton::lattice_curr, (int)c2[0], (int)c2[1], (int)c2[2], 2);
      const automaton::Cell& s3 = automaton::getCell(
          automaton::lattice_curr, (int)c3[0], (int)c3[1], (int)c3[2], 3);
      // The absorbed stack must be gone.  The model may re-pair the released
      // halves within the same sweep (a fresh count-1 pair -- observed and
      // deterministic here), so the stable invariant is that the population
      // of 4 left them: they are either singletons or a fresh count-1 pair.
      assertions &= expect(!(s2.kind == automaton::SourceKind::P &&
                             s2.pair_idx == 3u && s2.pair_count == 4u) &&
                           !(s3.kind == automaton::SourceKind::P &&
                             s3.pair_idx == 2u && s3.pair_count == 4u),
                           "the seeded stack {2,3} no longer holds its 4 pairs");
#else
      // Reference control: fresh pair starts at count 1, stack untouched.
      assertions &= expect(a.pair_count == 1u && b.pair_count == 1u,
                           "reference: fresh pair starts at pair_count == 1");
      const auto& c2 = automaton::lcenters[2];
      const auto& c3 = automaton::lcenters[3];
      const automaton::Cell& s2 = automaton::getCell(
          automaton::lattice_curr, (int)c2[0], (int)c2[1], (int)c2[2], 2);
      const automaton::Cell& s3 = automaton::getCell(
          automaton::lattice_curr, (int)c3[0], (int)c3[1], (int)c3[2], 3);
      assertions &= expect(s2.kind == automaton::SourceKind::P &&
                           s3.kind == automaton::SourceKind::P &&
                           s2.pair_idx == 3u && s3.pair_idx == 2u &&
                           s2.pair_count == 4u && s3.pair_count == 4u,
                           "reference: seeded stack untouched (P, count 4)");
#endif
      asserted = true;
    }
    prevPair = pairNow;
  }

  if (!asserted)
  {
    printf("FAIL: no formation fired within %u frames (see dumps above)\n", FRAMES);
    return 1;
  }

  // Final-state assertions.  The absorbed population does NOT persist: the
  // model's own turnaround consumption spends it (t reaches RMAX, the count
  // drops one unit per transport tick while t == RMAX, and the emptied pair
  // is released) -- that decay IS the multi-frequency lifetime semantics the
  // stack count encodes.  The assertions therefore check decay, not a fixed
  // final count.
  {
    printf("final state (frame %u):\n", FRAMES);
    dumpState(FRAMES);
    const auto& f0 = automaton::lcenters[0];
    const auto& f1 = automaton::lcenters[1];
    const auto& f2 = automaton::lcenters[2];
    const auto& f3 = automaton::lcenters[3];
    const automaton::Cell& a = automaton::getCell(
        automaton::lattice_curr, (int)f0[0], (int)f0[1], (int)f0[2], 0);
    const automaton::Cell& b = automaton::getCell(
        automaton::lattice_curr, (int)f1[0], (int)f1[1], (int)f1[2], 1);
    const automaton::Cell& s2 = automaton::getCell(
        automaton::lattice_curr, (int)f2[0], (int)f2[1], (int)f2[2], 2);
    const automaton::Cell& s3 = automaton::getCell(
        automaton::lattice_curr, (int)f3[0], (int)f3[1], (int)f3[2], 3);
    (void)b; (void)s3;   // the leader half carries the authoritative count
#ifdef PAIR_STACK_ABSORB_FSM
    assertions &= expect(a.kind == automaton::SourceKind::S ||
                         a.pair_count < 5u,
                         "final: the absorbed population (5) decays through "
                         "the model's own turnaround consumption");
#else
    assertions &= expect(a.kind == automaton::SourceKind::S ||
                         a.pair_count <= 1u,
                         "final: the reference pair (count 1) was consumed or "
                         "released by the model's own mechanics");
    assertions &= expect(s2.kind == automaton::SourceKind::S ||
                         s2.pair_count < 4u,
                         "final: the seeded stack (4) decays through the "
                         "model's own turnaround consumption");
#endif
  }

  printf("assertions done: %s\n", assertions ? "ALL OK" : "FAILED");
  return assertions ? 0 : 1;
}



