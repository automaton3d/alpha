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
#include "globals.h"

namespace automaton
{
  extern unsigned EL;
  extern unsigned W_USED;

  int count = 0;

  bool ctrl = true; // debug

  // Diagnostic counters for the controlled-scattering study (headless runner).
  // Exposed so tests/scatter_main.cpp can print them.
  long long enc_calls = 0;       // encounter() invocations that passed active checks
  long long enc_s2b   = 0;       // ... that also passed the s2B gate
  long long enc_pair  = 0;       // ... that formed a pair (canFormPair && samePos && sameT)
  long long enc_self  = 0;       // ... rejected by the same-W-island guard
  long long enc_collapse = 0;    // gate-passing contacts in the collapse (force) branch
  long long enc_adiah   = 0;     // gate-passing contacts in the adiabatic (attraction) branch
  long long enc_repel   = 0;     // gate-passing contacts that repel one light-step

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
    inline const std::array<unsigned, 3>& sourceCenter(const Cell& c)
    {
      return lcenters[c.x[3]];
    }

    inline Cell& sourceCenterDraft(const Cell& c)
    {
      const auto& p = sourceCenter(c);
      return getCell(lattice_draft, p[0], p[1], p[2], c.x[3]);
    }

    inline Cell& sourceCenterCurr(const Cell& c)
    {
      const auto& p = sourceCenter(c);
      return getCell(lattice_curr, p[0], p[1], p[2], c.x[3]);
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

      // Propeller: a pair with a pending relocation impulse reloc (nonzero)
      // is currently transferring momentum and cannot join a blob.  The
      // intrinsic direction m is ignored here: every source centre carries
      // a fixed unit step (initSim.cpp), so it cannot identify the
      // transient propeller role.
      if ((a.reloc[0] | a.reloc[1] | a.reloc[2]) != 0 ||
          (b.reloc[0] | b.reloc[1] | b.reloc[2]) != 0)
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

  /**
   * Handles the pairwise encounter of two active wavefronts of distinct
   * W-islands that coincide at one voxel.  Named encounter(), not convolution:
   * nothing is averaged here; the old name survives only in the conv_*
   * diagnostics counters so the campaign logs/scripts stay valid.
   *
   * @curr the current lattice
   * @draft the draft lattice
   * @partner the mirrored lattice
   */
  bool encounter(Cell& curr, Cell &draft, Cell &partner)
  {
    if (!curr.active || !partner.active)
      return false;

    // A source does not interact with itself (same W-island).
    if (curr.x[3] == partner.x[3])
    {
      ++enc_self;
      return false;
    }

    // Sieve: the electroweak interaction channel is only active where s2B is set.
    ++enc_calls;
    if (!curr.s2B)
      return false;
    ++enc_s2b;

    // Source state is stored in the source-center cell of each W-layer.
    Cell& currSrc  = sourceCenterCurr(curr);
    Cell& partnerSrc = sourceCenterCurr(partner);
    Cell& currDraft  = sourceCenterDraft(curr);
    Cell& partnerDraft = sourceCenterDraft(partner);

    const auto& currCenter  = sourceCenter(curr);
    const auto& partnerCenter = sourceCenter(partner);

    bool samePos = (curr.x[0] == partner.x[0] &&
                    curr.x[1] == partner.x[1] &&
                    curr.x[2] == partner.x[2]);
    bool sameT   = (curr.t == partner.t);

    // ---------------------------------------------------------------
    // Pair formation (photon-like P sources).
    // Two overlapping bubbles with the same wavefront time and complementary
    // charges can form a pair. The pair is "dressing" if both bubbles already
    // share the same leader; otherwise it is a free photon.
    // ---------------------------------------------------------------
    if (samePos && sameT && canFormPair(currSrc, partnerSrc))
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
        // Simplified momentum exchange: add the partner's momentum direction
        // to the target's relocation impulse. applyMomentum will update m.
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

    // 7. P x K / P x D / P x S (current = P, partner = ordinary source)
    if (currSrc.kind == SourceKind::P &&
        (partnerSrc.kind == SourceKind::K ||
         partnerSrc.kind == SourceKind::S ||
         partnerSrc.kind == SourceKind::D))
    {
      // P pair reemits at the contact point with phase 0.
      reemitAtContact(currDraft, curr);

      // Target receives a momentum impulse in the direction of P's momentum.
      partnerDraft.reloc[0] += currSrc.m[0];
      partnerDraft.reloc[1] += currSrc.m[1];
      partnerDraft.reloc[2] += currSrc.m[2];
      return false;
    }

    // 8. K/D/S x P (current = ordinary source, partner = P)
    if ((currSrc.kind == SourceKind::K ||
         currSrc.kind == SourceKind::S ||
         currSrc.kind == SourceKind::D) &&
        partnerSrc.kind == SourceKind::P)
    {
      // Target reemits on its own surface at the contact point and gets P's momentum.
      reemitAtContact(currDraft, curr);
      currDraft.reloc[0] += partnerSrc.m[0];
      currDraft.reloc[1] += partnerSrc.m[1];
      currDraft.reloc[2] += partnerSrc.m[2];
      return false;
    }

    // P x P is suppressed.
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
      /*--- Hunting using hB ---*/
      if (curr.active)
      {
        if (north.hB) { draft.c[0] = (north.c[0] + 1) % ELX; curr.sB = !draft.hB; }
        else if (west.hB)  { draft.c[1] = (west.c[1] + 1) % ELY; curr.sB = !draft.hB; }
        else if (down.hB)  { draft.c[2] = (down.c[2] + 1) % ELZ; curr.sB = !draft.hB; }
        else if (south.hB) { draft.c[1] = (south.c[1] + 1) % ELY; curr.sB = !draft.hB; }
        else if (east.hB)  { draft.c[0] = (east.c[0] + 1) % ELX; curr.sB = !draft.hB; }
        else if (up.hB)    { draft.c[2] = (up.c[2] + 1) % ELZ; curr.sB = !draft.hB; }
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
      draft.hB = false;
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
