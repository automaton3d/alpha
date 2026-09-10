/*
 * interaction.cpp
 *
 * This file contains important routines used to
 * evaluate interactions in the CA.
 */

#include <cassert>
#include <algorithm>
#include <utility>
#include "model/simulation.h"
#include "model/island_identity.h"
#include "model/chief_transition.h"
#include "globals.h"

namespace automaton
{
  extern unsigned EL;
  extern unsigned W_USED;

  int count = 0;

  bool ctrl = true; // debug

  // Diagnostic counters. Internal mechanical contacts count in enc_calls
  // but bypass the electroweak counters; historical rates are not comparable.
  long long enc_calls = 0;       // encounter() invocations that passed active checks
  long long enc_s2b   = 0;       // ... that also passed the s2B gate
  long long enc_pair  = 0;       // ... that formed a pair (canFormPair && samePos && sameT)
  long long enc_self  = 0;       // ... rejected by the same-W-island guard
  long long enc_collapse = 0;    // gate-passing contacts in the collapse (force) branch
  long long enc_adiah   = 0;     // gate-passing contacts in the adiabatic (attraction) branch
  long long enc_repel   = 0;     // gate-passing contacts that repel one light-step

  // Experimental P2 (macro ORPHAN_GUIDANCE_FSM): orphan-shell x free-photon
  // recruitment events.  Kept apart from the enc_* family so the campaign
  // logs stay comparable; always zero without the macro.
  long long recruit_events = 0;
  long long recruit_repel  = 0;   // ... whose impulse repelled the two islands
  long long recruit_attract = 0;  // ... whose impulse attracted the two islands
  long long annihilations  = 0;   // representative pairs annihilated (rule 5)

  // Backward-compatible aliases (deprecated; new code should use enc_*).
  // References, so the old conv_* readers (alpha_probe / campaign logs)
  // observe the same values with no further changes.
  long long& conv_calls    = enc_calls;
  long long& conv_s2b      = enc_s2b;
  long long& conv_pair     = enc_pair;
  long long& conv_self     = enc_self;
  long long& conv_collapse = enc_collapse;
  long long& conv_adiah    = enc_adiah;
  long long& conv_repel    = enc_repel;

  namespace
  {
    std::vector<Cell> sourceBefore, sourceAfter;
    std::vector<std::pair<WIndex, WIndex>> internalContacts;
    std::vector<unsigned char> contactSeen;
    std::vector<std::array<long long,3>> transportDebt;
    unsigned long long transportFrame = 0;
#ifdef ORPHAN_GUIDANCE_FSM
    // Experimental P2: island pairs whose mediator engaged a shell during this
    // frame, with the requested sign (+1 repel / -1 attract).  Collected by
    // encounter() and applied once per pair per frame by resolveRecruitPush()
    // at the frame edge (the internalContacts / resolveExclusionPush pattern).
    std::vector<std::array<int, 3>> recruitPushes;
#endif

    inline const std::array<unsigned, 3>& sourceCenter(const Cell& c)
    {
      return lcenters[c.x[3]];
    }

    inline Cell& sourceCenterDraft(const Cell& c)
    {
      return sourceAfter[c.x[3]];
    }

    inline Cell& sourceCenterCurr(const Cell& c)
    {
      return sourceBefore[c.x[3]];
    }

    inline int shortestDelta(int a, int b, int mod)
    {
      int d = b - a;
      if (mod > 0)
      {
        int half = mod / 2;
        if (d > half) d -= mod;
        else if (d < -half) d += mod;
      }
      return d;
    }

    inline int sign(int v)
    {
      return (v > 0) - (v < 0);
    }

    // Pair-formation test from the six superposing-bubble rules.
    bool canFormPair(const Cell& a, const Cell& b)
    {
      unsigned char ca = a.ch;
      unsigned char cb = b.ch;
      // Rule 3: both neutral (000000)
      if (ca == 0x00 && cb == 0x00) return true;
      // Rule 4: both anti-neutral (111111)
      if (ca == 0x3F && cb == 0x3F) return true;
      // Rule 1: every charge bit complementary
      if ((ca ^ cb) == 0x3F) return true;

      bool qa = (ca & 0x08) != 0;
      bool qb = (cb & 0x08) != 0;
      bool w1a = (ca & 0x20) != 0;
      bool w1b = (cb & 0x20) != 0;
      bool w0a = (ca & 0x10) != 0;
      bool w0b = (cb & 0x10) != 0;
      unsigned char cola = ca & 0x07;
      unsigned char colb = cb & 0x07;

      // Rule 2: q, w0 and color complementary; w1 identical
      if ((qa ^ qb) && (w1a == w1b) && (w0a ^ w0b) && ((cola ^ colb) == 0x07)) return true;
      // Rule 5: q=0, w1=0, w0=1, same non-neutral color
      if (!qa && !qb && !w1a && !w1b && w0a && w0b && cola == colb && cola != 0x00 && cola != 0x07) return true;
      // Rule 6: q=1, w1=1, w0=0, same non-neutral color
      if (qa && qb && w1a && w1b && !w0a && !w0b && cola == colb && cola != 0x00 && cola != 0x07) return true;
      return false;
    }

    // Blob-formation test (manuscript "Blob" and "Superposing bubbles").
    // A blob is a group of superposed equal-status pairs.  Only the R2
    // charge geometry qualifies (same sector w1 with q/w0/color
    // complementary -- gluon/photon).  The specialized pairs never blob:
    // neutrino/antineutrino (R3/R4), up quark (R5/R6), graviton (R1,
    // fully complementary charges) and propeller (a pair currently acting
    // as a momentum carrier -- one with a pending relocation impulse).
    bool canFormBlob(const Cell& a, const Cell& b)
    {
      const unsigned char ca = a.ch;
      const unsigned char cb = b.ch;

      // R3 / R4: fully neutral words are neutrinos / antineutrinos.
      if (ca == 0x00 && cb == 0x00) return false;
      if (ca == 0x3F && cb == 0x3F) return false;

      // R1: fully complementary charges are gravitons.
      if ((ca ^ cb) == 0x3F) return false;

      const bool qa  = (ca & 0x08) != 0, qb  = (cb & 0x08) != 0;
      const bool w1a = (ca & 0x20) != 0, w1b = (cb & 0x20) != 0;
      const bool w0a = (ca & 0x10) != 0, w0b = (cb & 0x10) != 0;
      const unsigned char cola = ca & 0x07u, colb = cb & 0x07u;

      // R5 / R6: same weak bits and the same non-trivial color -> up quark.
      if (qa == qb && w1a == w1b && w0a == w0b &&
          cola == colb && cola != 0x00u && cola != 0x07u)
        return false;

      // Affiliation and m identify a propeller, never a received impulse.
      if (isBoundPropeller(a) || isBoundPropeller(b))
        return false;

      // Remaining allowed geometry: same sector w1, complementary q/w0/color.
      return (w1a == w1b) && (qa ^ qb) && (w0a ^ w0b) &&
             ((cola ^ colb) == 0x07u);
    }

    // Add an impulse (dx, dy, dz) to the source-center cell and reemit phase 0.
    // The long-term momentum-direction vector m is preserved; the consumable
    // relocation vector reloc records the pending displacement.  The actual move
    // is performed after the FSM by applyMomentum().
    void reemitSourceAt(Cell& srcDraft, int dx, int dy, int dz)
    {
      srcDraft.reloc[0] += dx;
      srcDraft.reloc[1] += dy;
      srcDraft.reloc[2] += dz;
      srcDraft.t = 0;
      srcDraft.f = 0;
      // Fatia 1: ledger the reemission — island w just had its clock reset.
      // Every reemission path funnels through here (reemitAtContact,
      // moveOneStep, moveOneStepAway), so this is the single hook.
      chargesMarkInteraction((unsigned)srcDraft.x[3]);
    }

    // Move one light-step along the direction from 'from' to 'to'.
    void moveOneStep(Cell& srcDraft, const std::array<unsigned, 3>& from,
                     const std::array<unsigned, 3>& to)
    {
      int dx = shortestDelta((int)from[0], (int)to[0], (int)ELX);
      int dy = shortestDelta((int)from[1], (int)to[1], (int)ELY);
      int dz = shortestDelta((int)from[2], (int)to[2], (int)ELZ);
      reemitSourceAt(srcDraft, sign(dx), sign(dy), sign(dz));
    }

    // Move one light-step away from the other source center.
    void moveOneStepAway(Cell& srcDraft, const std::array<unsigned, 3>& selfCenter,
                         const std::array<unsigned, 3>& otherCenter)
    {
      int dx = shortestDelta((int)otherCenter[0], (int)selfCenter[0], (int)ELX);
      int dy = shortestDelta((int)otherCenter[1], (int)selfCenter[1], (int)ELY);
      int dz = shortestDelta((int)otherCenter[2], (int)selfCenter[2], (int)ELZ);
      reemitSourceAt(srcDraft, sign(dx), sign(dy), sign(dz));
    }

    // Reemit at the contact voxel (curr position) without changing kind.
    void reemitAtContact(Cell& srcDraft, const Cell& contact)
    {
      int dx = shortestDelta((int)srcDraft.x[0], (int)contact.x[0], (int)ELX);
      int dy = shortestDelta((int)srcDraft.x[1], (int)contact.x[1], (int)ELY);
      int dz = shortestDelta((int)srcDraft.x[2], (int)contact.x[2], (int)ELZ);
      reemitSourceAt(srcDraft, dx, dy, dz);
    }

    // Convenience: make a cell adopt a leader identity.
    inline void adoptLeader(Cell& dst, WIndex leader)
    {
      dst.leader_w = leader;
      dst.a = (unsigned)leader;
    }

    // Choose the dominant leader between two sources; if neither has one,
    // fall back to the smaller intrinsic W address.
    inline WIndex dominantLeader(const Cell& a, const Cell& b)
    {
      WIndex leader = std::min(a.leader_w, b.leader_w);
      if (leader == NO_LEADER_W)
        leader = std::min(a.w, b.w);
      return leader;
    }
  }

  bool isBoundPropeller(const Cell& s)
  {
    return s.kind == SourceKind::P && s.a < W_USED &&
           s.pair_idx < W_USED && s.pair_count > 0 &&
           (s.m[0] != 0 || s.m[1] != 0 || s.m[2] != 0);
  }

  bool hadInternalContact(WIndex a, WIndex b)
  {
    if (a >= W_USED || b >= W_USED || contactSeen.size() != (size_t)W_USED * W_USED)
      return false;
    if (a > b) std::swap(a, b);
    return contactSeen[(size_t)a * W_USED + b] != 0;
  }

  void resetSourceTransactions()
  {
    sourceBefore.clear();
    sourceAfter.clear();
    internalContacts.clear();
    contactSeen.clear();
    transportDebt.clear();
    transportFrame = 0;
  }

  void beginSourceTick()
  {
    sourceBefore.resize(W_USED);
    if (transportDebt.size() != W_USED) transportDebt.assign(W_USED, {0,0,0});
    for (unsigned w = 0; w < W_USED; ++w) {
      const auto& p = lcenters[w];
      sourceBefore[w] = getCell(lattice_curr, p[0], p[1], p[2], w);
    }
    sourceAfter = sourceBefore;
    if ((!lattice_curr.empty() && lattice_curr.front().k == 0) ||
        contactSeen.size() != (size_t)W_USED * W_USED) {
      internalContacts.clear();
#ifdef ORPHAN_GUIDANCE_FSM
      recruitPushes.clear();
#endif
      contactSeen.assign((size_t)W_USED * W_USED, 0);
    }
  }

  namespace {
    bool body(const Cell& s) {
      return s.kind == SourceKind::K || s.kind == SourceKind::D;
    }

    int delta(unsigned a, unsigned b, int axis) {
      const unsigned edges[] = {ELX, ELY, ELZ};
      return shortestDelta((int)lcenters[a][axis], (int)lcenters[b][axis],
                           (int)edges[axis]);
    }

    // Select a face step, not a displacement of |m| cells. Rotating the
    // tie breaker avoids privileging x for equal Cartesian components.
    std::array<int, 3> faceStep(const int* v, unsigned turn) {
      int axis = (int)(turn % 3);
      for (int j = 1; j < 3; ++j) {
        int k = (int)((turn + j) % 3);
        if (std::abs(v[k]) > std::abs(v[axis])) axis = k;
      }
      std::array<int, 3> step{0, 0, 0};
      step[axis] = sign(v[axis]);
      return step;
    }

    bool sameAffinity(const Cell& a, const Cell& b) {
      return a.a < W_USED && a.a == b.a;
    }

    std::array<int,3> propellerStep(unsigned p, const int* m) {
      // Integer DDA: distribute unit face-steps in proportion to |m|.
      // Magnitude changes cannot turn one encounter into a long jump.
      long long total=0;
      for(int k=0;k<3;++k) {
        const long long magnitude=m[k]<0?-(long long)m[k]:(long long)m[k];
        transportDebt[p][k]+=magnitude; total+=magnitude;
      }
      int axis=(int)(transportFrame%3);
      for(int k=0;k<3;++k)
        if(transportDebt[p][k]>transportDebt[p][axis]) axis=k;
      transportDebt[p][axis]-=total;
      std::array<int,3> step{0,0,0}; step[axis]=sign(m[axis]);
      return step;
    }

    void resolveInternalContacts()
    {
      // Contacts are voxel-independent events: one unordered source pair
      // per light frame, regardless of shell area and mirrored W visits.
      std::sort(internalContacts.begin(), internalContacts.end());
      internalContacts.erase(std::unique(internalContacts.begin(), internalContacts.end()),
                             internalContacts.end());
      std::vector<std::array<int, 3>> moves(W_USED, {0, 0, 0});
      std::vector<bool> moved(W_USED, false);

      // K-D and D-D cohesion: reciprocal face steps reduce a separation
      // greater than one cell without changing the body's centre of mass.
      for (const auto& [a, b] : internalContacts) {
        const Cell& sa = sourceAfter[a]; const Cell& sb = sourceAfter[b];
        if (!body(sa) || !body(sb) || !shareChief(sa, sb) || moved[a] || moved[b]) continue;
        int d[3] = {delta(a,b,0), delta(a,b,1), delta(a,b,2)};
        auto step = faceStep(d, (unsigned)transportFrame);
        int axis = step[0] ? 0 : (step[1] ? 1 : 2);
        if (std::abs(d[axis]) <= 1) continue;
        moves[a] = step;
        for (int k=0;k<3;++k) moves[b][k] = -step[k];
        moved[a] = moved[b] = true;
      }

      // Each reciprocal P pair owns one kick opportunity. Prefer the rear
      // eligible contact along m, so repeated kicks do not peel a leading
      // constituent away from its K-D/D-D contacts. Ties rotate by frame.
      for (unsigned p = 0; p < W_USED; ++p) {
        const Cell& prop = sourceAfter[p];
        if (!isBoundPropeller(prop) || p >= prop.pair_idx) continue;
        const unsigned half = prop.pair_idx;
        const Cell& other = sourceAfter[half];
        if (!isBoundPropeller(other) || other.pair_idx != p || !sameAffinity(prop, other)) continue;
        if (effective_t(prop.t) == 0) continue;
        unsigned target = W_USED;
        unsigned followTarget = W_USED;
        long long best = 0;
        for (const auto& [a,b] : internalContacts) {
          unsigned q = W_USED;
          if (a == p || a == half) q = b;
          else if (b == p || b == half) q = a;
          if (q == W_USED || !body(sourceAfter[q]) ||
              !sameAffinity(prop, sourceAfter[q])) continue;
          if (followTarget == W_USED) followTarget = q;
          if (moved[q]) continue;
          long long projection = 0;
          for (int k=0;k<3;++k) projection += (long long)delta(p,q,k) * prop.m[k];
          const auto rank = (q + W_USED - transportFrame % W_USED) % W_USED;
          const auto oldRank = (target + W_USED - transportFrame % W_USED) % W_USED;
          if (target == W_USED || projection < best || (projection == best && rank < oldRank)) {
            target = q; best = projection;
          }
        }
        if (target != W_USED) {
          moves[target] = propellerStep(p, prop.m);
          moved[target] = true;
          followTarget = target;
        }
        if (followTarget == W_USED) continue;
        // Recycle the bound photon pair toward the contacted constituent.
        // Both halves take the same step; affiliation and m are preserved.
        int follow[3];
        for (int k=0;k<3;++k) follow[k] = delta(p,followTarget,k) + moves[followTarget][k];
        moves[p] = moves[half] = faceStep(follow, (unsigned)transportFrame);
        moved[p] = moved[half] = true;
      }
      for (unsigned w = 0; w < W_USED; ++w)
        for (int k=0;k<3;++k) sourceAfter[w].reloc[k] += moves[w][k];
      ++transportFrame;
    }

#ifdef EXCLUSION_FSM
    // Frame-edge separation push for cross-family equal-charge pairs
    // recorded by the identity hard core.  One antisymmetric unit step per
    // pair per light frame, along the axis of largest shortest-torus
    // separation (W-derived axis when coincident).  Bound same-chief pairs
    // are never pushed.
    void resolveExclusionPush()
    {
      for (const auto& [a, b] : internalContacts) {
        if (a / 3u == b / 3u) continue;                 // same family
        const Cell& sa = sourceAfter[a];
        const Cell& sb = sourceAfter[b];
        if (sa.ch != sb.ch) continue;                   // equal charge only
        WIndex ca = islandChief(sa), cb = islandChief(sb);
        if (ca != NO_PARENT && ca == cb) continue;      // already one island
        int sep[3];
        int best = 0, bestAbs = -1;
        for (int axis = 0; axis < 3; ++axis) {
          const int d = delta(a, b, axis);
          sep[axis] = d;
          const int ad = d < 0 ? -d : d;
          if (ad > bestAbs) { bestAbs = ad; best = axis; }
        }
        int axis, sgn;
        if (bestAbs > 0) { axis = best; sgn = sep[axis] > 0 ? -1 : 1; }
        else { axis = (int)(((uint64_t)a + b) % 3u); sgn = a < b ? -1 : 1; }
        sourceAfter[a].reloc[axis] += sgn;
        sourceAfter[b].reloc[axis] -= sgn;
      }
    }
#endif

#ifdef ORPHAN_GUIDANCE_FSM
    // Frame-edge application of the recruit channel impulse (P2).  One
    // antisymmetric unit face step per engaged island pair per frame, along
    // the axis of largest shortest-torus separation, exactly like
    // resolveExclusionPush: the centre of mass is conserved and the frame-edge
    // commit machinery (commitSourceTick) consumes the reloc.  The sign comes
    // from the two islands' charge words (or is always attractive for the R1
    // graviton mediator) and was decided in encounter().
    void resolveRecruitPush()
    {
      if (recruitPushes.empty()) return;
      std::vector<std::array<int, 3>> list = recruitPushes;
      std::sort(list.begin(), list.end(),
                [](const std::array<int, 3>& x, const std::array<int, 3>& y)
                { return (x[0] != y[0]) ? (x[0] < y[0]) : (x[1] < y[1]); });
      list.erase(std::unique(list.begin(), list.end(),
                [](const std::array<int, 3>& x, const std::array<int, 3>& y)
                { return x[0] == y[0] && x[1] == y[1]; }), list.end());
      for (const auto& e : list)
      {
        const unsigned a = (unsigned)e[0], b = (unsigned)e[1];
        if (a >= W_USED || b >= W_USED || a == b) continue;
#ifdef ORPHAN_NO_PUSH_APPLY
        // Bisection build: record the engagement but never move anything.
        continue;
#endif
        const int sign = e[2];                 // +1 repel / -1 attract
        int sep[3];
        int best = 0, bestAbs = -1;
        for (int axis = 0; axis < 3; ++axis)
        {
          const int d = delta(a, b, axis);
          sep[axis] = d;
          const int ad = d < 0 ? -d : d;
          if (ad > bestAbs) { bestAbs = ad; best = axis; }
        }
        int axis, sgn;
        if (bestAbs > 0) { axis = best; sgn = sep[axis] > 0 ? -1 : 1; }
        else continue;      // coincident centres: no defined push direction
        sourceAfter[a].reloc[axis] += sign * sgn;
        sourceAfter[b].reloc[axis] -= sign * sgn;
        if (sign > 0) ++recruit_repel; else ++recruit_attract;
      }
    }
#endif
  }

  void commitSourceTick()
  {
    if (!lattice_draft.empty() && lattice_draft.front().k == 0) {
      resolveInternalContacts();
#ifdef EXCLUSION_FSM
      resolveExclusionPush();
#endif
#ifdef ORPHAN_GUIDANCE_FSM
      resolveRecruitPush();
#endif
    }
    for (unsigned w = 0; w < W_USED; ++w) {
      const auto& p = lcenters[w];
      Cell& dst = getCell(lattice_draft, p[0], p[1], p[2], w);
      const Cell& s = sourceAfter[w];
      dst.w = w; dst.ch = s.ch; dst.a = s.a; dst.leader_w = s.leader_w;
      dst.kind = s.kind; dst.parent = s.parent; dst.spin_target = s.spin_target;
      dst.pair_idx = s.pair_idx; dst.pair_count = s.pair_count; dst.bB = s.bB;
      for (int k=0;k<3;++k) { dst.m[k] = s.m[k]; dst.reloc[k] = s.reloc[k]; }
      if (s.t != sourceBefore[w].t) { dst.t = s.t; dst.f = s.f; }
      else if (body(s) || isBoundPropeller(s)) {
        dst.t = (s.t + (dst.k == 0 ? 1u : 0u)) % (2 * RMAX);
        dst.f = effective_t(dst.t);
      }
    }
  }

  /**
   * Handles active wavefronts from distinct W addresses at one voxel,
   * including equal-affinity internal mechanical contacts. Named encounter():
   * nothing is averaged here; the old name survives only in the conv_*
   * diagnostics counters so the campaign logs/scripts stay valid.
   *
   * @curr the current lattice
   * @draft the draft lattice
   * @partner the mirrored lattice
   */
  bool encounter(Cell& curr, Cell &draft, Cell &partner)
  {
    if (!curr.active || !partner.active || curr.r<=0 || partner.r<=0)
      return false;

    // A source does not interact with its own W address.
    if (curr.x[3] == partner.x[3])
    {
      ++enc_self;
      return false;
    }

    ++enc_calls;

    // Source state is stored in the source-center cell of each W-layer.
    Cell& currSrc  = sourceCenterCurr(curr);
    Cell& partnerSrc = sourceCenterCurr(partner);
    Cell& currDraft  = sourceCenterDraft(curr);
    Cell& partnerDraft = sourceCenterDraft(partner);

    const auto& currCenter  = sourceCenter(curr);
    const auto& partnerCenter = sourceCenter(partner);

#ifdef ORPHAN_GUIDANCE_FSM
    // ==================================================================
    // Experimental P2 (recruit / relay): a FREE photon half (P source with no
    // leader) that coincides with an island wavefront illuminates that
    // island's orphan shell when one of the contact site's neighbours in the
    // island layer sits on the thin shell (isOrphanShell: r == f + 1).  The
    // shell is one cell thick, so the event stays rare.  On recruitment the
    // photon half is reissued at the contact point (the relay step); the
    // island itself is NOT touched here.
    // ==================================================================
    {
      // A mediator half is any P source that is NOT part of the engaged
      // island: a free photon/graviton (no leader) or the OTHER island's
      // dress (leader = that island).  Using the dress leader as island B is
      // exact, unlike the nearest-source fallback used for free mediators.
      auto mediator = [](const Cell& c)
      {
        return c.kind == SourceKind::P;
      };
      const Cell* isl = nullptr;
      const Cell* pho = nullptr;
      WIndex mLead = NO_LEADER_W;      // the mediator's leader (bound dress)
      if (currSrc.kind != SourceKind::P && mediator(partnerSrc))
      { isl = &curr; pho = &partner; }
      else if (partnerSrc.kind != SourceKind::P && mediator(currSrc))
      { isl = &partner; pho = &curr; }

      if (isl && pho && isl->x[3] != pho->x[3])
      {
        const WIndex eChief = islandChief(sourceAfter[isl->x[3]]);
        mLead = sourceAfter[pho->x[3]].leader_w;
        const bool otherIsland = (mLead == NO_LEADER_W) ||
                                 (eChief == NO_PARENT) || (mLead != eChief);
        if (!otherIsland) { isl = nullptr; pho = nullptr; }
      }

      if (isl && pho && isl->x[3] != pho->x[3])
      {
        static const int off[6][3] = { {1,0,0}, {-1,0,0}, {0,1,0},
                                       {0,-1,0}, {0,0,1}, {0,0,-1} };
        const int cx = (int)isl->x[0], cy = (int)isl->x[1], cz = (int)isl->x[2];
        bool onShell = false;
        for (const auto& o : off)
        {
          const Cell& nb = getCell(lattice_curr, cx + o[0], cy + o[1],
                                   cz + o[2], (int)isl->x[3]);
          if (isOrphanShell(nb)) { onShell = true; break; }
        }
        if (onShell)
        {
          ++recruit_events;
          // Relay only for a FREE mediator AT REST: a bound dress is already
          // co-located with its body, and a driving pair (m != 0) must not have
          // its two halves' clocks desynchronised.
          {
            const Cell& medSrc = sourceAfter[pho->x[3]];
            const bool medFree = (medSrc.leader_w == NO_LEADER_W) &&
                                 medSrc.m[0] == 0 && medSrc.m[1] == 0 &&
                                 medSrc.m[2] == 0;
            if (medFree) reemitAtContact(sourceCenterDraft(*pho), *pho);
          }

          // ---------------------------------------------------------------
          // Sign and the dual impulse (design section 4, rules 5-7).  The
          // sign is decided by the TWO ISLANDS' charge words alone; the
          // mediator is only the vehicle, so the orientation of its two
          // halves cannot matter:
          //   R1 graviton pair (fully complementary words, charge-neutral)
          //       -> ALWAYS attractive;
          //   R2 photon pair (same sector w1, complementary q/w0/color)
          //       -> equal island charge repels, opposite charge attracts.
          // The impulse is ONE antisymmetric face step per island pair per
          // tick (centre of mass conserved), the same one-light-step
          // granularity as the existing electroweak branches, applied
          // through sourceAfter[] (the sanctioned remote-impulse channel).
          // ---------------------------------------------------------------
#ifndef ORPHAN_GATE_ONLY
          const unsigned aw = isl->x[3];
          const unsigned pw0 = pho->x[3];
          // The pair link is authoritative in the source centre, not on the
          // wavefront cell.
          const unsigned pw1 = (sourceAfter[pw0].pair_idx < W_USED &&
                                sourceAfter[pw0].pair_idx != pw0)
                                 ? (unsigned)sourceAfter[pw0].pair_idx : W_USED;
          const unsigned char aCh = sourceAfter[aw].ch;     // island A charge
          const unsigned char q0  = sourceAfter[pw0].ch;    // mediator words
          const unsigned char q1  = (pw1 < W_USED) ? sourceAfter[pw1].ch : 0x00;
          const bool graviton = (pw1 < W_USED) &&
                                (((q0 ^ q1) & 0x3Fu) == 0x3Fu);

          // The other island: the mediator's own leader when it is a bound
          // dress; otherwise the nearest unaffiliated source (free mediator).
          unsigned bw = W_USED;
          unsigned char bCh = 0;
          if (mLead != NO_LEADER_W && mLead < W_USED)
          {
            bw = mLead;
            bCh = sourceAfter[bw].ch;
          }
          else
          {
          int bestD = -1;
          for (unsigned w = 0; w < W_USED; ++w)
          {
            if (w == aw) continue;
            const Cell& sc = sourceAfter[w];
            if (sc.kind == SourceKind::P) continue;
            const WIndex chief = islandChief(sc);
            if (chief != NO_PARENT && chief == islandChief(sourceAfter[aw]))
              continue;                       // same island
            int d = 0;
            for (int axis = 0; axis < 3; ++axis)
            {
              const int dd = delta(aw, w, axis);
              d += dd * dd;
            }
            if (bestD < 0 || d < bestD) { bestD = d; bw = w; bCh = sc.ch; }
          }
          }

          if (bw < W_USED)
          {
            // The impulse acts on ISLANDS (any non-mediator source): a lone
            // source is an island of one until the chief election groups it.
            // It is only RECORDED here and applied once per island pair per
            // frame by resolveRecruitPush() at the frame edge, exactly like
            // the EXCLUSION push (no reloc writes in the middle of the phase
            // sequence).
            const bool islands = sourceAfter[aw].kind != SourceKind::P &&
                                 sourceAfter[bw].kind != SourceKind::P;
            if (islands)
            {
              const int pushSign = graviton ? -1 : (aCh == bCh ? +1 : -1);
              recruitPushes.push_back({ (int)aw, (int)bw, pushSign });
            }
          }
#endif
        }
      }
    }
#endif

#ifdef ORPHAN_GUIDANCE_FSM
#ifndef ORPHAN_NO_ANNIH
    // ==================================================================
    // Experimental P2 (annihilation; design section 4 rule 5, decision D):
    // two BODY representatives (K or D) of DIFFERENT islands that carry
    // OPPOSITE charges and overlap at the SAME site annihilate: both are
    // demoted to S and reissued at the contact point with the orphan affinity
    // (a = W).  Distinct parents only, so a member never annihilates its own
    // island, and there is no distant annihilation - the overlap must be here.
    // ==================================================================
    {
      auto antimatter = [](unsigned char ch)
      {
        const unsigned color = ch & 0x07u;
        const unsigned ones = (color & 1u) + ((color >> 1) & 1u) +
                              ((color >> 2) & 1u);
        return ones >= 2u;      // same convention as the charges census
      };
      const bool sameSite = (curr.x[0] == partner.x[0] &&
                             curr.x[1] == partner.x[1] &&
                             curr.x[2] == partner.x[2]);
      const WIndex ca = islandChief(currSrc);
      const WIndex cb = islandChief(partnerSrc);
      // Distinct islands: different chiefs once both exist, otherwise different
      // charge FAMILIES (w/3), which is the island grouping before the chief
      // election.  This is the reachable reading of "different parents".
      const bool diffParents =
          (ca != NO_PARENT && cb != NO_PARENT) ? (ca != cb)
                                               : (curr.x[3] / 3u != partner.x[3] / 3u);
      // DELEGATES only.  Root cause found (see the design note section 8):
      // demoting a CHIEF (K) to S leaves its island's members pointing at a
      // non-chief anchor, and the identity machinery then crashes a few frames
      // later (measured: ann = 18 then segfault in the anchor x anchor probe;
      // the D x D probe runs clean with ann = 354).  Restricting the branch to
      // D x D is also what the manuscript states ("only D x D overlap");
      // K-member annihilation needs island-dissolution semantics and stays open.
      const bool delegates = currSrc.kind == SourceKind::D &&
                             partnerSrc.kind == SourceKind::D;
      if (sameSite && delegates && diffParents &&
          antimatter(currSrc.ch) != antimatter(partnerSrc.ch))
      {
        ++annihilations;
        chargesMarkAnnihilation();
        for (Cell* d : { &currDraft, &partnerDraft })
        {
          d->kind        = SourceKind::S;
          d->parent      = NO_PARENT;
          d->leader_w    = NO_LEADER_W;
          d->a           = W_USED;
          d->pair_idx    = NO_PAIR;
          d->pair_count  = 0;
          d->spin_target = 0;
        }
        reemitAtContact(currDraft, curr);
        reemitAtContact(partnerDraft, partner);
        return false;
      }
    }
#endif
#endif

#ifdef EM_FIRST_FSM
    // ==============================================================
    // Experimental /D EM_FIRST_FSM: decide the electroweak channel BEFORE
    // the identity merge for equal-charge contacts that carry live pB/sB
    // and pass the s2B gate.  In the reference order the identity path
    // (chiefContact / K-K / D-D, and the EXCLUSION gate) preempts these
    // branches, so two equal-charge islands always merge before any EM
    // response can act (PBSB_ISLANDS.md).  This block mirrors the tail EM
    // semantics for the equal-charge subset (collapse vs adiabatic; S/S
    // same-Q repel; K/K repel; D/D cross-tribe repel; S attaches to a
    // K/D chief) and is compiled only under this macro.
    // ==============================================================
    if (currSrc.ch == partnerSrc.ch && curr.s2B &&
        (curr.pB || partner.pB || curr.sB || partner.sB))
    {
      const bool electricCollapse = curr.pB && partner.pB;
      const bool magneticCollapse = curr.sB && partner.sB;
      const bool collapse          = electricCollapse || magneticCollapse;

      if (collapse)
      {
        ++enc_collapse;
        draft.kB = true;            // collapse flag: inward reissue
        draft.cB = true;
      }
      else
      {
        ++enc_adiah;
        WIndex minLeader = dominantLeader(currSrc, partnerSrc);
        adoptLeader(currDraft, minLeader);
        adoptLeader(partnerDraft, minLeader);
        std::swap(currDraft.t, partnerDraft.t);
        moveOneStep(currDraft, currCenter, partnerCenter);
        moveOneStep(partnerDraft, partnerCenter, currCenter);
        return false;
      }

      if (currSrc.kind == SourceKind::K && partnerSrc.kind == SourceKind::K)
      {
        ++enc_repel;
        moveOneStepAway(currDraft, currCenter, partnerCenter);
        return false;
      }

      if (currSrc.kind == SourceKind::S && partnerSrc.kind == SourceKind::S)
      {
        if (currSrc.Q() == partnerSrc.Q())
        {
          ++enc_repel;               // equal field sign -> repel one step
          moveOneStepAway(currDraft, currCenter, partnerCenter);
        }
        else
        {
          WIndex leader = dominantLeader(currSrc, partnerSrc);
          currDraft.kind  = SourceKind::D;
          partnerDraft.kind = SourceKind::D;
          currDraft.parent  = leader;
          partnerDraft.parent = leader;
          adoptLeader(currDraft, leader);
          adoptLeader(partnerDraft, leader);
        }
        return false;
      }

      if (currSrc.kind == SourceKind::D && partnerSrc.kind == SourceKind::D)
      {
        if (currSrc.parent != partnerSrc.parent)
        {
          ++enc_repel;
          moveOneStepAway(currDraft, currCenter, partnerCenter);
          currDraft.reloc[0] += partnerSrc.m[0];
          currDraft.reloc[1] += partnerSrc.m[1];
          currDraft.reloc[2] += partnerSrc.m[2];
        }
        return false;
      }

      // S x K / S x D and mirrors: the S attaches as a D of the chief.
      if ((currSrc.kind == SourceKind::S &&
           (partnerSrc.kind == SourceKind::K || partnerSrc.kind == SourceKind::D)) ||
          (partnerSrc.kind == SourceKind::S &&
           (currSrc.kind == SourceKind::K || currSrc.kind == SourceKind::D)))
      {
        Cell& sDraft  = (currSrc.kind == SourceKind::S) ? currDraft : partnerDraft;
        Cell& dSrc    = (currSrc.kind == SourceKind::S) ? partnerSrc : currSrc;
        Cell& dDraft  = (currSrc.kind == SourceKind::S) ? partnerDraft : currDraft;
        const auto& sCenter = (currSrc.kind == SourceKind::S) ? currCenter : partnerCenter;
        const auto& dCenter = (currSrc.kind == SourceKind::S) ? partnerCenter : currCenter;
        WIndex leader = (dSrc.leader_w == NO_LEADER_W ? dSrc.parent : dSrc.leader_w);
        if (leader == NO_LEADER_W) leader = dSrc.w;
        sDraft.kind = SourceKind::D;
        sDraft.parent = dSrc.parent == NO_PARENT ? leader : dSrc.parent;
        adoptLeader(sDraft, leader);
        moveOneStep(sDraft, sCenter, dCenter);
        moveOneStep(dDraft, dCenter, sCenter);
        return false;
      }

      return false;
    }
#endif

    // Role transitions precede internal-contact early returns. Only the
    // current draft is updated; reciprocal encounters update the other side.
#ifdef EXCLUSION_FSM
    // Pauli-like identity hard core (candidate, ported from the colour FSM):
    // equal-charge sources from DIFFERENT seed families never share identity
    // and are not allowed to merge through chiefContact/promotion/clash.
    // The pair is recorded (deduplicated per light frame) for the separation
    // push applied at the frame edge; everything else is skipped.
    if (currSrc.ch == partnerSrc.ch && currSrc.w / 3u != partnerSrc.w / 3u) {
      const WIndex a = std::min(currSrc.w, partnerSrc.w);
      const WIndex b = std::max(currSrc.w, partnerSrc.w);
      const size_t index = (size_t)a * W_USED + b;
      if (!contactSeen[index]) {
        contactSeen[index] = 1;
        internalContacts.emplace_back(a, b);
      }
      return false;
    }
#endif
    chiefContact(currSrc,partnerSrc,currDraft);
    if(currSrc.kind==SourceKind::K && partnerSrc.kind==SourceKind::K && currSrc.ch==partnerSrc.ch)
      return false; // Preserve the clash transition through the remaining branches.
    if(currSrc.kind==SourceKind::D && partnerSrc.kind==SourceKind::D && currSrc.ch==partnerSrc.ch)
      return false; // The older generic branches must not overwrite this transition.

    const bool internal =
      (body(currDraft) && body(partnerDraft) && shareChief(currDraft, partnerDraft)) ||
      (sameAffinity(currSrc, partnerSrc) &&
      ((isBoundPropeller(currSrc) && body(partnerDraft)) ||
       (isBoundPropeller(partnerSrc) && body(currDraft)) ||
       (currSrc.kind == SourceKind::P && partnerSrc.kind == SourceKind::P)));
    if (internal) {
      const WIndex a = std::min(currSrc.w, partnerSrc.w);
      const WIndex b = std::max(currSrc.w, partnerSrc.w);
      const size_t index = (size_t)a * W_USED + b;
      if (!contactSeen[index]) {
        contactSeen[index] = 1;
        internalContacts.emplace_back(a, b);
      }
      return false;
    }

#ifdef ISLAND_SEED_EXPERIMENT
    // Reduced experiment: retain contact-driven election and internal
    // cohesion above; omit inter-family scattering and pair formation.
    return false;
#endif
    // The electroweak sieve does not gate same-affinity mechanical contacts.
    if (!curr.s2B) return false;
    ++enc_s2b;

    // A free pair may join a contacted island. Acquiring affinity is not
    // an immediate kick, and receiving a kick never turns a K/D into P.
    if ((currSrc.kind == SourceKind::P && body(partnerSrc)) ||
        (partnerSrc.kind == SourceKind::P && body(currSrc))) {
      const Cell& p = currSrc.kind == SourceKind::P ? currSrc : partnerSrc;
      const Cell& target = currSrc.kind == SourceKind::P ? partnerSrc : currSrc;
      if (p.a == W_USED && p.pair_idx < W_USED &&
          sourceBefore[p.pair_idx].pair_idx == p.w) {
        for (unsigned w : {p.w, p.pair_idx}) {
          sourceAfter[w].a = target.a;
          sourceAfter[w].leader_w = target.leader_w;
          sourceAfter[w].parent = target.leader_w;
        }
      }
      return false;
    }

    bool samePos = (curr.x[0] == partner.x[0] &&
                    curr.x[1] == partner.x[1] &&
                    curr.x[2] == partner.x[2]);
    bool sameT   = (currSrc.t == partnerSrc.t);

    // ---------------------------------------------------------------
    // Pair formation (photon-like P sources).
    // Two overlapping bubbles with the same wavefront time and complementary
    // charges can form a pair. The pair is "dressing" if both bubbles already
    // share the same leader; otherwise it is a free photon.
    // ---------------------------------------------------------------
    if (samePos && sameT && currSrc.kind == SourceKind::S &&
        partnerSrc.kind == SourceKind::S && canFormPair(currSrc, partnerSrc))
    {
      ++enc_pair;
      bool dressing = (currSrc.leader_w != NO_LEADER_W &&
                       currSrc.leader_w == partnerSrc.leader_w);
      WIndex newLeader = dressing ? currSrc.leader_w : NO_LEADER_W;
      unsigned newA    = dressing ? (unsigned)newLeader : W_USED;
      WIndex parent    = dressing ? currSrc.leader_w : NO_PARENT;

      // If both sources are already a pair with each other, do not re-form.
      bool alreadyPaired = (currSrc.kind == SourceKind::P &&
                            partnerSrc.kind == SourceKind::P &&
                            currSrc.pair_idx == partnerSrc.w &&
                            partnerSrc.pair_idx == currSrc.w);
      if (alreadyPaired)
        return false;

      chargesMarkPair();          // idea B: count this registered formation

      uint8_t newCount = 1;
      if (currSrc.kind == SourceKind::P) newCount += currSrc.pair_count;
      if (partnerSrc.kind == SourceKind::P) newCount += partnerSrc.pair_count;

      currDraft.kind  = SourceKind::P;
      partnerDraft.kind = SourceKind::P;
      currDraft.pair_idx   = partnerSrc.w;
      partnerDraft.pair_idx = currSrc.w;
      currDraft.pair_count = newCount;
      partnerDraft.pair_count = newCount;
      currDraft.leader_w  = newLeader;
      partnerDraft.leader_w = newLeader;
      currDraft.a  = newA;
      partnerDraft.a = newA;
      currDraft.parent  = parent;
      partnerDraft.parent = parent;

      // Move both source centers to the contact point and reset their clocks.
      reemitAtContact(currDraft, curr);
      reemitAtContact(partnerDraft, partner);

      // -----------------------------------------------------------------
      // Blob formation (manuscript "Blob" and "Superposing bubbles"): a
      // group of superposed equal-status pairs aggregates into a blob when
      // the charge geometry is R2 (gluon/photon, see canFormBlob) and both
      // halves carry the same (active) polarization bits pB/sB and the same
      // bB flag.  The common affinity is already enforced by newA above.
      // Specialized pairs (neutrino/antineutrino, up quark, graviton,
      // propeller) never blob; a single fresh pair (newCount == 1) is not
      // yet a group.
      // -----------------------------------------------------------------
      if (newCount > 1 && canFormBlob(currSrc, partnerSrc) &&
          currSrc.pB == partnerSrc.pB &&
          currSrc.sB == partnerSrc.sB &&
          currSrc.bB == partnerSrc.bB)
      {
        currDraft.bB  = true;
        partnerDraft.bB = true;
        chargesMarkBlob();
      }

      return false;
    }

    // pB triggers the electric channel, sB the magnetic channel.
    bool electricContact   = curr.pB || partner.pB;
    bool magneticContact   = curr.sB || partner.sB;
    bool electricCollapse  = curr.pB && partner.pB;
    bool magneticCollapse  = curr.sB && partner.sB;
    bool collapse          = electricCollapse || magneticCollapse;

    if (!electricContact && !magneticContact)
      return false;

    if (collapse)
    {
      ++enc_collapse;
      // Collapse flag propagates inward and triggers reissue.
      draft.kB = true;
      draft.cB = true;
    }
    else
    {
      ++enc_adiah;
      // Adiabatic: no collapse, but the two sources exchange
      // leader identity, affinity and light clock, then drift one step
      // toward each other.
      WIndex minLeader = dominantLeader(currSrc, partnerSrc);
      adoptLeader(currDraft, minLeader);
      adoptLeader(partnerDraft, minLeader);
      std::swap(currDraft.t, partnerDraft.t);
      moveOneStep(currDraft, currCenter, partnerCenter);
      moveOneStep(partnerDraft, partnerCenter, currCenter);
      return false;
    }

    // 1. K x K
    if (currSrc.kind == SourceKind::K && partnerSrc.kind == SourceKind::K)
    {
      ++enc_repel;
      moveOneStepAway(currDraft, currCenter, partnerCenter);
      return false;
    }

    // 2. K x S (current = K, partner = S): K does not move; the S becomes a
    //    D delegate of K when it is its turn to be the current cell.
    //    The S-side is handled below.

    // 3. S x K (current = S, partner = K)
    if (currSrc.kind == SourceKind::S && partnerSrc.kind == SourceKind::K)
    {
      currDraft.kind = SourceKind::D;
      currDraft.parent = partnerSrc.w;
      adoptLeader(currDraft, partnerSrc.leader_w == NO_LEADER_W ? partnerSrc.w : partnerSrc.leader_w);
      // S vector direction relative to K is approximated as outward for now.
      currDraft.spin_target = 1;
      moveOneStep(currDraft, currCenter, partnerCenter);
      return false;
    }

    // 4. S x S
    if (currSrc.kind == SourceKind::S && partnerSrc.kind == SourceKind::S)
    {
      if (currSrc.Q() == partnerSrc.Q())
      {
        // Same field sign: repel one light-step.
        ++enc_repel;
        moveOneStepAway(currDraft, currCenter, partnerCenter);
      }
      else
      {
        // Opposite field sign: both become D delegates of the dominant leader.
        WIndex leader = dominantLeader(currSrc, partnerSrc);
        currDraft.kind  = SourceKind::D;
        partnerDraft.kind = SourceKind::D;
        currDraft.parent  = leader;
        partnerDraft.parent = leader;
        adoptLeader(currDraft, leader);
        adoptLeader(partnerDraft, leader);
      }
      return false;
    }

    // 5. S x D / D x S
    if ((currSrc.kind == SourceKind::S && partnerSrc.kind == SourceKind::D) ||
        (currSrc.kind == SourceKind::D && partnerSrc.kind == SourceKind::S))
    {
      Cell& sSrcDraft  = (currSrc.kind == SourceKind::S ? currDraft : partnerDraft);
      Cell& dSrc       = (currSrc.kind == SourceKind::S ? partnerSrc : currSrc);
      Cell& dSrcDraft  = (currSrc.kind == SourceKind::S ? partnerDraft : currDraft);
      const auto& sCenter = (currSrc.kind == SourceKind::S ? currCenter : partnerCenter);
      const auto& dCenter = (currSrc.kind == SourceKind::S ? partnerCenter : currCenter);

      // S becomes a D delegate of the K parent of the D.
      WIndex leader = (dSrc.leader_w == NO_LEADER_W ? dSrc.parent : dSrc.leader_w);
      if (leader == NO_LEADER_W) leader = dSrc.w;
      sSrcDraft.kind = SourceKind::D;
      sSrcDraft.parent = dSrc.parent == NO_PARENT ? leader : dSrc.parent;
      adoptLeader(sSrcDraft, leader);

      // Both reemit and move one light-step toward each other.
      moveOneStep(sSrcDraft, sCenter, dCenter);
      moveOneStep(dSrcDraft, dCenter, sCenter);
      return false;
    }

    // 6. D x D
    if (currSrc.kind == SourceKind::D && partnerSrc.kind == SourceKind::D)
    {
      if (currSrc.parent != partnerSrc.parent)
      {
        // Different tribes: reemit, repel one light-step, exchange momentum.
        ++enc_repel;
        moveOneStepAway(currDraft, currCenter, partnerCenter);
        // Momentum exchange via inertia path: add partner's m into reloc.
        // m itself is immutable here; applyMomentum only consumes reloc.
        currDraft.reloc[0] += partnerSrc.m[0];
        currDraft.reloc[1] += partnerSrc.m[1];
        currDraft.reloc[2] += partnerSrc.m[2];
      }
      else
      {
        // Same tribe: outer delegate imposes spin_target on inner one.
        // (Tangential/radial orbital motion is left as a refinement.)
        currDraft.spin_target = partnerSrc.spin_target;
        // Enforce a common leader identity.
        WIndex leader = dominantLeader(currSrc, partnerSrc);
        adoptLeader(currDraft, leader);
      }
      return false;
    }

    // P x K/D transport and free-pair affiliation were handled above.
    // A raw P/S contact does not confer the internal propeller role.
    return false;
  }

  void diffuse(Cell& curr, Cell &draft, Cell &forward,
               Cell &north, Cell &west, Cell &down,
               Cell &south, Cell &east, Cell &up)
  {
	  /****** SLOT I ******/
	  if (curr.k < SLOT1)
	  {
	    /*--- Orphan propagation ---*/
	    if ((north.a == W_USED && curr.r2 >= north.r2) ||
	        (west.a  == W_USED && curr.r2 >= west.r2)  ||
	        (down.a  == W_USED && curr.r2 >= down.r2)  ||
	        (south.a == W_USED && curr.r2 >= south.r2) ||
	        (east.a  == W_USED && curr.r2 >= east.r2)  ||
	        (up.a    == W_USED && curr.r2 >= up.r2))
	    {
	      draft.a = W_USED;
	      draft.leader_w = NO_LEADER_W;
	    }
	  }
	  /****** SLOT II ******/
    if (curr.k < SLOT2)
    {
      /*--- Orphan propagation ---*/
      if ((north.a == W_USED && curr.r2 >= north.r2) ||
          (west.a  == W_USED && curr.r2 >= west.r2)  ||
          (down.a  == W_USED && curr.r2 >= down.r2)  ||
          (south.a == W_USED && curr.r2 >= south.r2) ||
          (east.a  == W_USED && curr.r2 >= east.r2)  ||
          (up.a    == W_USED && curr.r2 >= up.r2))
      {
        draft.a = W_USED;
        draft.leader_w = NO_LEADER_W;
      }
      /*--- Homing using homB ---*/
      if (curr.active)
      {
        if (north.homB) { draft.c[0] = (north.c[0] + 1) % ELX; curr.sB = !draft.homB; }
        else if (west.homB)  { draft.c[1] = (west.c[1] + 1) % ELY; curr.sB = !draft.homB; }
        else if (down.homB)  { draft.c[2] = (down.c[2] + 1) % ELZ; curr.sB = !draft.homB; }
        else if (south.homB) { draft.c[1] = (south.c[1] + 1) % ELY; curr.sB = !draft.homB; }
        else if (east.homB)  { draft.c[0] = (east.c[0] + 1) % ELX; curr.sB = !draft.homB; }
        else if (up.homB)    { draft.c[2] = (up.c[2] + 1) % ELZ; curr.sB = !draft.homB; }
      }
    }
    /****** SLOT III ******/
    else if (curr.k < SLOT3)
    {
      if (!ZERO(north.c))
      {
        draft.c[0] = north.c[0];
        draft.c[1] = north.c[1];
        draft.c[2] = north.c[2];
        if (north.kB) draft.kB = north.kB;
      }
      if (!ZERO(west.c))
      {
        draft.c[0] = west.c[0];
        draft.c[1] = west.c[1];
        draft.c[2] = west.c[2];
        if (west.kB) draft.kB = west.kB;
      }
      if (!ZERO(down.c))
      {
        draft.c[0] = down.c[0];
        draft.c[1] = down.c[1];
        draft.c[2] = down.c[2];
        if (down.kB) draft.kB = down.kB;
      }
      // f is the local triangular breathing phase f = effective_t(t)
      // (manuscript Sect. "The light frame"); it is not diffused.
      // Diffuse CB toward center (r2=0)
      if (!curr.cB)
      {
        if (north.cB && north.r2 > curr.r2)
        {
          draft.cB = true;
          if (north.a != W_USED) {
            draft.a = north.a;
            draft.leader_w = (WIndex)north.a; }
        }
        else if (south.cB && south.r2 > curr.r2)
        {
          draft.cB = true;
          if (south.a != W_USED) {
            draft.a = south.a;
            draft.leader_w = (WIndex)south.a; }
        }
        else if (east.cB && east.r2 > curr.r2)
        {
          draft.cB = true;
          if (east.a != W_USED) {
            draft.a = east.a;
            draft.leader_w = (WIndex)east.a; }
        }
        else if (west.cB && west.r2 > curr.r2)
        {
          draft.cB = true;
          if (west.a != W_USED) {
            draft.a = west.a;
            draft.leader_w = (WIndex)west.a; }
        }
        else if (down.cB && down.r2 > curr.r2)
        {
          draft.cB = true;
          if (down.a != W_USED) {
            draft.a = down.a;
            draft.leader_w = (WIndex)down.a; }
        }
        else if (up.cB && up.r2 > curr.r2)
        {
          draft.cB = true;
          if (up.a != W_USED) {
            draft.a = up.a;
            draft.leader_w = (WIndex)up.a; }
        }
      }
    }
    /****** SLOT IV ******/
    else if (curr.k < SLOT4)
    {
      if (forward.kB && forward.a == curr.a)
      {
        int delta_x = (curr.x[0] - forward.x[0] + ELX) % ELX;
        int delta_y = (curr.x[1] - forward.x[1] + ELY) % ELY;
        int delta_z = (curr.x[2] - forward.x[2] + ELZ) % ELZ;

        draft.c[0] = (forward.c[0] + delta_x) % ELX;
        draft.c[1] = (forward.c[1] + delta_y) % ELY;
        draft.c[2] = (forward.c[2] + delta_z) % ELZ;
        draft.kB = forward.kB;
        draft.cB = forward.cB;
      }
      // f is the local triangular breathing phase; it is not diffused.
    }
    /****** SLOT V ******/
    else if (curr.k < SLOT5)
    {
      if (curr.a == W_USED)
      {
        if (curr.r2 < curr.t * curr.t)
        {
      	  draft.a = curr.x[3];
        }
      }
    }
  }

  /**
   * Grid relocation.
   */
  void relocate(Cell& curr, Cell &draft, Cell &north, Cell &west, Cell &down)
  {
    // Save the 3D address
    unsigned x, y, z;
    x = curr.x[0];
    y = curr.x[1];
    z = curr.x[2];
    /****** SLOT VI ******/
    if (curr.k < SLOT6)
    {
      if (north.c[0] > 0)
      {
        draft = north;
        draft.c[0]--;
      }
    }
    /****** SLOT VII ******/
    else if (curr.k < SLOT7)
    {
      if (west.c[1] > 0)
      {
        draft = west;
        draft.c[1]--;
      }
    }
    /****** SLOT VIII ******/
    else if (curr.k < SLOT8)
    {
      if (down.c[2] > 0)
      {
        draft = down;
        draft.c[2]--;
      }
    }
    // Recover 3D address
    draft.x[0] = x;
    draft.x[1] = y;
    draft.x[2] = z;
  }

  /**
   * Prepares new wavefront.
   *
   * @curr the current lattice
   * @draft the draft lattice
   * @partner the mirrored lattice
   */
  void reissue(Cell& curr, Cell &draft, Cell &forward,
               Cell &north, Cell &west, Cell &down,
               Cell &south, Cell &east, Cell &up)
  {
      // Reset propagation status
      draft.kB = false;
      draft.homB = false;
      draft.bB = false;
      // Propagate normal affinity outward, overwriting normal or orphan
      if (curr.active)
      {
          if (north.r2 > curr.r2)
          {
              // Copy a from inner to outer cell
              draft.a = north.a;
              draft.leader_w = (north.a == W_USED ? NO_LEADER_W : (WIndex)north.a);
          }
          if (south.r2 > curr.r2)
          {
              draft.a = south.a;
              draft.leader_w = (south.a == W_USED ? NO_LEADER_W : (WIndex)south.a);
          }
          if (east.r2 > curr.r2)
          {
              draft.a = east.a;
              draft.leader_w = (east.a == W_USED ? NO_LEADER_W : (WIndex)east.a);
          }
          if (west.r2 > curr.r2)
          {
              draft.a = west.a;
              draft.leader_w = (west.a == W_USED ? NO_LEADER_W : (WIndex)west.a);
          }
          if (up.r2 > curr.r2)
          {
              draft.a = up.a;
              draft.leader_w = (up.a == W_USED ? NO_LEADER_W : (WIndex)up.a);
          }
          if (down.r2 > curr.r2)
          {
              draft.a = down.a;
              draft.leader_w = (down.a == W_USED ? NO_LEADER_W : (WIndex)down.a);
          }
      }
      if (curr.cB)
      {
        // Consume cB
        draft.cB = false;
        if (curr.a != W_USED && curr.r2 < 4)
        {
          draft.t = 0;
        }
      }
  }

  void flood(Cell& curr, Cell &draft, Cell &forward,
               Cell &north, Cell &west, Cell &down,
               Cell &south, Cell &east, Cell &up)
  {
	if (curr.a != W_USED)
      draft.t = min({ north.t, south.t, east.t, west.t, down.t, up.t });
  }
}
