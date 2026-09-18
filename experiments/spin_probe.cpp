// spin_probe.cpp -- J1 of the spin programme (J0 is recorded in J0_SPIN.md).
//
// QUESTION.  Does a group's total circulation protect it against the surface
// escape threat that already exists in the model?
//
// The threat: `SURFACE_ESCAPE_FSM` (candidate of item 4, PREDICTED_NS_RULE.md)
// releases a delegate that shares no same-charge shell overlap with its own
// group for one full partner rotation (W_USED light frames).
//
// The quantity: the J-programme's per-chief circulation
//
//     J = sum over the members   r x m
//
// with r the toroidal offset from the chief (delta(g, w, axis)) and m the
// immutable member momentum.  It reads only w, parent, lcenters and m: no L, no
// ISLAND_SIZE, no family index.
//
// Configuration (the p7 `spread` regime, where the escape rule bites hardest --
// measured escapes = 2, promotions = 0):
//
//   tube 15x5x5, W = 3, RMAX = 2, contact range 2*RMAX = 4
//   K at x = 2, D1 at x = 7, D2 at x = 12, all at y = z = 2
//   every pairwise shortest distance is 5 > 4, so NO member ever contacts
//   another: membership persists (nothing promotes, nothing else releases) and
//   the escape rule is the ONLY agent that can act.
//
// Arms (the second argument):
//   j0  : every m = 0                       -> J = (0,0,0)
//   jnz : D1 m = (0,+1,0), D2 m = (0,-1,0)  -> J = (0,0,+10), sum(m) = 0
//         i.e. a frozen rotor: a non-zero circulation with ZERO net linear
//         momentum, so the geometry cannot drift and the comparison is exact.
//         Note the toroidal offsets: with ELX = 15 the chief at x = 2 sees
//         D1 at r = +5 (x = 7) and D2 at r = -5 (x = 12 wraps, since 10 > 15/2),
//         so the two contributions ADD: r1 x m1 = (0,0,+5), r2 x m2 = (0,0,+5).
//
// Builds (experiments\build_spin_probe.bat):
//   spin_probe_ref.exe  : SURFACE_ESCAPE_FSM                   (threat alone)
//   spin_probe_gate.exe : SURFACE_ESCAPE_FSM + SPIN_GATED_FSM  (threat + S1/S3)
//
// REGISTERED PREDICTION (printed before the run, not rationalised after):
//   ref , j0  -> escapes 2, final 1K+2S   (baseline; matches p7 spread)
//   ref , jnz -> escapes 2, final 1K+2S   (m alone gates nothing: J is inert)
//   gate, j0  -> escapes 2, final 1K+2S   (no J to conserve)
//   gate, jnz -> escapes 0, final 1K+2D   (S3: the spin cannot leave)
// The decisive cell is gate+jnz: survival there, with escapes = 0 and
// spin_vetoes > 0, is the first evidence that circulation is load bearing.
//
// Build: experiments\build_spin_probe.bat
// Run:   build\spin_probe\spin_probe_<ref|gate>.exe [frames] [j0|jnz]

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

static void placeLayer(unsigned w, unsigned x, unsigned y, unsigned z,
                       unsigned char ch)
{
  lcenters[w] = {x, y, z};
  for (unsigned xx = 0; xx < ELX; ++xx)
    for (unsigned yy = 0; yy < ELY; ++yy)
      for (unsigned zz = 0; zz < ELZ; ++zz)
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
    const unsigned fam = 3u * (w / 3u);
    c.kind = SourceKind::K; c.parent = NO_PARENT;
    c.leader_w = (WIndex)fam; c.a = fam;
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = c.m[1] = c.m[2] = 0;
  }
}

static void markDelegate(unsigned w, unsigned chief, unsigned char ch,
                         int mx, int my, int mz)
{
  for (auto* lat : { &lattice_curr, &lattice_draft, &lattice_partner })
  {
    const auto& ctr = lcenters[w];
    Cell& c = getCell(*lat, (int)ctr[0], (int)ctr[1], (int)ctr[2], (int)w);
    c.kind = SourceKind::D; c.parent = chief; c.leader_w = (WIndex)chief;
    c.a = 3u * (w / 3u);
    c.pair_idx = NO_PAIR; c.pair_count = 0; c.ch = ch;
    c.m[0] = mx; c.m[1] = my; c.m[2] = mz;
  }
}

// The probe's own reading of the chief circulation, computed exactly as the
// S1 block of applySurfaceEscape() does.
static void chiefSpin(unsigned W, unsigned g, long long J[3])
{
  J[0] = J[1] = J[2] = 0;
  for (unsigned w = 0; w < W; ++w)
  {
    const Cell& s = source(w);
    if (s.kind != SourceKind::D) continue;
    if (s.parent != (WIndex)g) continue;
    if (source(g).kind != SourceKind::K) continue;
    const long long rx = wrappedDelta((int)lcenters[g][0], (int)lcenters[w][0], ELX);
    const long long ry = wrappedDelta((int)lcenters[g][1], (int)lcenters[w][1], ELY);
    const long long rz = wrappedDelta((int)lcenters[g][2], (int)lcenters[w][2], ELZ);
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
    const char* arm = (argc > 2) ? argv[2] : "j0";
    const bool rotor = (std::strcmp(arm, "jnz") == 0);
    if (!rotor && std::strcmp(arm, "j0") != 0)
      throw std::runtime_error("arm must be j0 or jnz");

    constexpr unsigned LX = 15, LY = 5, LZ = 5, W = 3;
    if (!tryAllocateTube(LX, LY, LZ, W))
      throw std::runtime_error(lastAllocationError);
    initSimulation(0);
    const unsigned y = (LY - 1) / 2, z = (LZ - 1) / 2;
    const unsigned xK = 2, xD1 = 7, xD2 = 12;   // pairwise 5 > 2*RMAX = 4
    placeLayer(0, xK, y, z, 0x08);
    placeLayer(1, xD1, y, z, 0x08);
    placeLayer(2, xD2, y, z, 0x08);
    replicate();
    markChief(0, 0x08);
    // The rotor: opposite transverse momenta, sum(m) = 0, J = (0,0,-5) != 0.
    markDelegate(1, 0, 0x08, 0, rotor ? +1 : 0, 0);
    markDelegate(2, 0, 0x08, 0, rotor ? -1 : 0, 0);
    replicate();

#if defined(SPIN_GATED_FSM)
    const char* build = "gate";
#else
    const char* build = "ref";
#endif

    long long J0[3];
    chiefSpin(W, 0, J0);
    printf("SPIN_PROBE build=%s arm=%s RMAX=%u contact-range=%u W=%u "
           "sites(K/D1/D2)=%u/%u/%u\n", build, arm, RMAX, 2u * RMAX, W,
           xK, xD1, xD2);
    printf("PREDICTION: %s\n",
           (build[0] == 'g')
             ? (rotor
                  ? "gate + J!=0 -> delegates held: escapes 0, spin_vetoes > 0, final 1K+2D"
                  : "gate + J==0 -> no circulation to conserve: escapes 2, final 1K+2S")
             : "escape rule alone -> escapes 2, final 1K+2S (m alone gates nothing)");
    printf("INITIAL J = (%lld,%lld,%lld) |J|^2=%lld  sum(m) = (0,0,0)\n",
           J0[0], J0[1], J0[2], J0[0]*J0[0] + J0[1]*J0[1] + J0[2]*J0[2]);
    // Per-member evidence, so the log shows why J takes the value it does.
    for (unsigned w = 0; w < W; ++w)
    {
      const Cell& s = source(w);
      printf("MEMBER w=%u kind=%d parent=%u x=%u m=(%d,%d,%d) r=(%d,%d,%d)\n",
             w, (int)s.kind, (unsigned)s.parent, lcenters[w][0],
             s.m[0], s.m[1], s.m[2],
             wrappedDelta((int)lcenters[0][0], (int)lcenters[w][0], ELX),
             wrappedDelta((int)lcenters[0][1], (int)lcenters[w][1], ELY),
             wrappedDelta((int)lcenters[0][2], (int)lcenters[w][2], ELZ));
    }

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
      printf("FRAME %2u K=%u D=%u S=%u groups=%zu max_pop=%u J=(%lld,%lld,%lld) "
             "|J|^2=%llu escapes=%u vetoes=%u promotions=%u\n",
             frame, nk, nd, ns, pops.size(), maxPop, J[0], J[1], J[2], Jm2,
             esc, vet, promotions);

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
    printf("MEASURED [%s arm=%s] promotions=%u max_pop=%u final=%uK+%uD+%uS\n",
           build, arm, promotions, maxPop, nk, nd, ns);
    const bool held = (nd == 2u && maxPop == 3u);
    const char* verdict =
        (build[0] == 'g' && rotor)
          ? (held ? "SPIN PROTECTS (J!=0 held the group: escapes 0)"
                  : "SPIN DOES NOT PROTECT (J!=0 group still released)")
          : (held ? "ANOMALY (a J==0 / ungated arm survived; expected release)"
                  : "CONTROL OK (released as predicted)");
    printf("VERDICT [%s arm=%s] final=%uK+%uD+%uS -> %s\n",
           build, arm, nk, nd, ns, verdict);
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}