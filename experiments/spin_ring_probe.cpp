// spin_ring_probe.cpp -- J2 of the spin programme (J0/J1 are in J1_SPIN.md).
//
// QUESTION.  Does the total circulation J *select* a size -- is some N special
// -- or does it only *protect* whatever size happens to be present?
//
// J1 showed that SPIN_GATED_FSM's S3 veto keeps a group alive while J != 0.  But
// S3 tests `Jx || Jy || Jz`: a BOOLEAN, with no N dependence anywhere.  The
// prediction registered below is therefore that protection is FLAT in N -- that
// J protects but does not select, and that what is missing is a
// *magnitude*-dependent cost (the |J| cap / spin fission of S4), not a stronger
// veto.  Measuring the flat curve is what makes the S4 test meaningful later.
//
// CONFIGURATION.  Tube 61x5x5, RMAX = 2, contact range 2*RMAX = 4, chief at
// x = 30 and N delegates on the long axis.  Every pairwise toroidal distance is
// >= 5 > 4, so no member ever contacts another: nothing promotes, nothing else
// releases, and the escape rule is the ONLY agent that can act.
//
// ARMS (third argument):
//   j0       : members at +5k, every m = 0           -> J = 0, no momenta
//   coherent : members at +5k, every m = (0,+1,0)    -> J = (0,0,S) != 0
//              with S = 5*(1+2+...+N) = 5*N*(N+1)/2
//   cancel   : members on alternating sides in PAIRS at the same distance
//              (+5,-5,+10,-10,...), every m = (0,+1,0) -> J = 0 exactly, with
//              the same momenta present as in `coherent`.  For odd N the single
//              unpaired outermost member is given m = 0, so that J is exactly 0
//              for every N rather than leaving a +5 residue.
//
// `coherent` vs `cancel` is the decisive pair: both arms carry the same number
// of members with the same individual momenta `m`, and the only difference is
// whether the contributions r x m add up or cancel.  Protection that tracks
// `coherent` and not `cancel` is a statement about J as a *collective* quantity,
// not about the members merely having momentum.
//
// REGISTERED PREDICTION (printed before the run, not rationalised after):
//   ref (threat alone) : every arm releases all N delegates  (escapes = N)
//   gate, j0           : releases all N                      (no J to conserve)
//   gate, cancel       : releases all N                      (J = 0 despite m)
//   gate, coherent     : releases 0 for EVERY N              (J != 0; S3 boolean)
// => dN vs N has zero slope: J protects, it does not select.
//
// Build: experiments\build_spin_probe.bat   (produces spin_ring_ref / _gate)
// Run:   build\spin_probe\spin_ring_<ref|gate>.exe [frames] [N] [j0|coherent|cancel]

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

// Long axis 61 keeps every centre far from the toroidal seam (half = 30), so
// no shortest-distance ever wraps and the offsets below are the literal ones.
static constexpr unsigned LX = 61, LY = 5, LZ = 5;
static constexpr unsigned XCHIEF = 30;
static constexpr unsigned SPACING = 5;   // > 2*RMAX = 4, so members never touch

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch)
{
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < LX; ++xx)
    for (unsigned yy = 0; yy < LY; ++yy)
      for (unsigned zz = 0; zz < LZ; ++zz)
      {
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

static void markChief(unsigned w, unsigned char ch)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::K; c.parent = NO_PARENT;
    c.leader_w = (WIndex)w; c.a = 0;
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
  }
}

static void markDelegate(unsigned w, unsigned chief, unsigned char ch,
                         int my)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::D; c.parent = chief; c.leader_w = (WIndex)chief;
    c.a = 0;
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = 0; c.m[1] = my; c.m[2] = 0;
  }
}

// The probe's own reading of the chief circulation, computed exactly as the S1
// block of applySurfaceEscape() does.
static void chiefSpin(unsigned W, unsigned g, long long J[3])
{
  J[0] = J[1] = J[2] = 0;
  for (unsigned w = 0; w < W; ++w)
  {
    const Cell& s = source(w);
    if (s.kind != SourceKind::D) continue;
    if (s.parent != (WIndex)g) continue;
    if (source(g).kind != SourceKind::K) continue;
    const long long rx = wrappedDelta((int)lcenters[g][0], (int)lcenters[w][0], LX);
    const long long ry = wrappedDelta((int)lcenters[g][1], (int)lcenters[w][1], LY);
    const long long rz = wrappedDelta((int)lcenters[g][2], (int)lcenters[w][2], LZ);
    const long long mx = s.m[0], my = s.m[1], mz = s.m[2];
    J[0] += ry * mz - rz * my;
    J[1] += rz * mx - rx * mz;
    J[2] += rx * my - ry * mx;
  }
}

static std::vector<unsigned> populations(unsigned W)
{
  std::map<unsigned, unsigned> pop;
  for (unsigned w = 0; w < W; ++w)
  {
    const Cell& s = source(w);
    if (s.kind == SourceKind::K) pop[w] += 1;
    else if (s.kind == SourceKind::D) pop[s.parent] += 1;
  }
  std::vector<unsigned> out;
  for (const auto& [chief, n] : pop) out.push_back(n);
  std::sort(out.begin(), out.end());
  return out;
}
int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    const unsigned frames = (argc > 1) ? (unsigned)atoi(argv[1]) : 12u;
    const unsigned N      = (argc > 2) ? (unsigned)atoi(argv[2]) : 3u;
    const char* arm       = (argc > 3) ? argv[3] : "coherent";
    if (N < 1u || N > 6u) throw std::runtime_error("N must be 1..6");
    const bool coherent = std::strcmp(arm, "coherent") == 0;
    const bool cancel   = std::strcmp(arm, "cancel") == 0;
    const bool zeroM    = std::strcmp(arm, "j0") == 0;
    if (!coherent && !cancel && !zeroM)
      throw std::runtime_error("arm must be j0, coherent or cancel");

    const unsigned W = N + 1u;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;

    placeLayer(0, XCHIEF, y, z, 0x08);
    for (unsigned k = 0; k < N; ++k)
    {
      // coherent/j0: steps 1,2,3,... so every r x m contribution ADDS.
      // cancel: PAIRED steps 1,1,2,2,3,3,... on alternating sides, so the
      // contributions cancel exactly (sum of r = 0) while every paired member
      // still carries the same m as in the coherent arm.  For odd N the single
      // unpaired outermost member is given m = 0, keeping J exactly 0.
      const unsigned step = cancel ? SPACING * (k / 2u + 1u)
                                   : SPACING * (k + 1u);
      const unsigned x = (cancel && (k % 2u) == 1u) ? XCHIEF - step
                                                    : XCHIEF + step;
      placeLayer(k + 1u, x, y, z, 0x08);
    }
    replicate();
    markChief(0, 0x08);
    for (unsigned k = 0; k < N; ++k)
    {
      const bool unpaired = cancel && (N % 2u == 1u) && (k == N - 1u);
      markDelegate(k + 1u, 0, 0x08,
                   ((coherent || cancel) && !unpaired) ? +1 : 0);
    }
    replicate();

#if defined(SPIN_GATED_FSM)
    const char* build = "gate";
#else
    const char* build = "ref";
#endif

    long long J0[3];
    chiefSpin(W, 0, J0);
    const unsigned long long J0m2 =
        (unsigned long long)(J0[0]*J0[0] + J0[1]*J0[1] + J0[2]*J0[2]);
    printf("SPIN_RING build=%s arm=%s N=%u W=%u LX=%u RMAX=%u contact-range=%u "
           "spacing=%u\n", build, arm, N, W, LX, RMAX, 2u * RMAX, SPACING);
    printf("PREDICTION: %s\n",
           (build[0] == 'g')
             ? ((coherent)
                  ? "gate + coherent -> J != 0 -> all N held (escapes 0) for EVERY N"
                  : "gate + J == 0 -> no circulation to conserve: escapes = N")
             : "escape rule alone -> escapes = N (m alone gates nothing, any arm)");
    for (unsigned w = 0; w < W; ++w)
    {
      const Cell& s = source(w);
      printf("MEMBER w=%u kind=%d parent=%u x=%u m=(%d,%d,%d) r=(%d,%d,%d)\n",
             w, (int)s.kind, (unsigned)s.parent, lcenters[w][0],
             s.m[0], s.m[1], s.m[2],
             wrappedDelta((int)lcenters[0][0], (int)lcenters[w][0], LX),
             wrappedDelta((int)lcenters[0][1], (int)lcenters[w][1], LY),
             wrappedDelta((int)lcenters[0][2], (int)lcenters[w][2], LZ));
    }
    printf("INITIAL J = (%lld,%lld,%lld) |J|^2=%llu  N=%u\n",
           J0[0], J0[1], J0[2], J0m2, N);

    unsigned frame = 0, promotions = 0;
    std::vector<unsigned char> prevKind(W);
    for (unsigned w = 0; w < W; ++w) prevKind[w] = (unsigned char)source(w).kind;

    for (;;)
    {
      unsigned nk = 0, nd = 0, ns = 0;
      for (unsigned w = 0; w < W; ++w)
      {
        const SourceKind k = source(w).kind;
        nk += (k == SourceKind::K); nd += (k == SourceKind::D); ns += (k == SourceKind::S);
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
      long long J[3];
      chiefSpin(W, 0, J);
      const unsigned long long Jm2 =
          (unsigned long long)(J[0]*J[0] + J[1]*J[1] + J[2]*J[2]);
#ifdef SURFACE_ESCAPE_FSM
      const unsigned esc = surface_escapes;
#else
      const unsigned esc = 0u;
#endif
#ifdef SPIN_GATED_FSM
      const unsigned vet = spin_vetoes;
#else
      const unsigned vet = 0u;
#endif
      printf("FRAME %2u N=%u K=%u D=%u S=%u max_pop=%u J=(%lld,%lld,%lld) "
             "|J|^2=%llu escapes=%u vetoes=%u\n",
             frame, N, nk, nd, ns, maxPop, J[0], J[1], J[2], Jm2,
             esc, vet);

      if (frame >= frames) break;
      while (!simulation()) {}
      ++frame;
    }

    const std::vector<unsigned> pops = populations(W);
    const unsigned maxPop = pops.empty() ? 0u : pops.back();
    unsigned nk = 0, nd = 0, ns = 0;
    for (unsigned w = 0; w < W; ++w)
    {
      const SourceKind k = source(w).kind;
      nk += (k == SourceKind::K); nd += (k == SourceKind::D); ns += (k == SourceKind::S);
    }
#ifdef SURFACE_ESCAPE_FSM
    printf("surface_escapes_total=%u\n", surface_escapes);
#endif
#ifdef SPIN_GATED_FSM
    printf("spin_vetoes_total=%u\n", spin_vetoes);
#endif
    printf("MEASURED [%s arm=%s N=%u] promotions=%u max_pop=%u final=%uK+%uD+%uS\n",
           build, arm, N, promotions, maxPop, nk, nd, ns);
    const bool released = (nd == 0u);
    const bool held     = (nd == N);
    const char* verdict =
        (build[0] == 'g' && coherent)
          ? (held ? "J PROTECTS (all N held: escapes 0)"
                  : "J DOES NOT PROTECT (coherent arm lost members)")
          : (released ? "CONTROL OK (all released as predicted)"
                      : "ANOMALY (a J==0 / ungated arm kept members)");
    printf("VERDICT [%s arm=%s N=%u] final=%uK+%uD+%uS -> %s\n",
           build, arm, N, nk, nd, ns, verdict);
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}