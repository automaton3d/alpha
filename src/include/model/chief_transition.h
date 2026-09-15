#pragma once
#include "model/island_identity.h"
namespace automaton {
#ifdef PHASE_DISTINCT_FSM
  // Candidate (see experiments/EMERGENCE_SEARCH.md): identity is the pair
  // (charge word, breathing phase).  The reference already refuses merges
  // between different charge words; this refuses them also between sources that
  // agree in word AND phase, because such bubbles are indistinguishable in the
  // model's own sense.  Reads only ch and t: no ISLAND_SIZE, no L.
  inline bool phaseConflict(const Cell& a, const Cell& b) {
    return a.ch == b.ch && a.t == b.t;
  }
#endif
inline bool promotesDelegate(const Cell& main,const Cell& mirror) {
  // Active/radius contact gates belong to encounter. No affinity, parent,
  // spin, separation, or population condition is imposed here.
#ifdef DD_INTRA_ISLAND_FIX
  // Candidate (WP3.3): a D x D contact promotes only across DISTINCT islands.
  // Within one island (same chief / parent) the two delegates must stay
  // delegates, so a 3-constituent family keeps 1 K + 2 D instead of cascading
  // to 2 K + 1 D.  Macro-guarded; OFF in the reference build.
  if (main.parent < W_USED && main.parent == mirror.parent) return false;
#endif
  return main.kind==SourceKind::D && mirror.kind==SourceKind::D &&
         main.ch==mirror.ch && main.w<mirror.w;
}
inline void makeChief(Cell& draft) {
  draft.kind=SourceKind::K;draft.parent=NO_PARENT;draft.leader_w=draft.w;
}
inline bool demotesChief(const Cell& main,const Cell& mirror) {
  return main.kind==SourceKind::K && mirror.kind==SourceKind::K &&
         main.ch==mirror.ch && main.w>mirror.w;
}
inline void makeDelegate(Cell& draft,WIndex chief) {
  draft.kind=SourceKind::D;draft.parent=chief;draft.leader_w=chief;
}
inline void chiefContact(const Cell& main,const Cell& mirror,Cell& draft) {
  if(main.w==mirror.w || main.ch!=mirror.ch) return;
#ifdef PHASE_DISTINCT_FSM
  // Candidate: the reference refuses merges between different words; this also
  // refuses them between sources that agree in word AND breathing phase, i.e.
  // between indistinguishable bubbles.  Covers (T1), (T3) and (T4) here and
  // (T2) through promotesDelegate below.
  if(phaseConflict(main,mirror)) return;
#endif
#ifdef DD_INTRA_ISLAND_FIX
  // Candidate (WP3.3): within one seed FAMILY (w / ISLAND_SIZE) the chief is the
  // family-minimum address, so a 3-constituent family elects exactly ONE chief
  // (1 K + 2 D) instead of a pairwise cascade to 2 K + 1 D.  Only same-family
  // S x S election is redirected; other contacts keep the minimum-address rule.
  // Macro-guarded; OFF in the reference build.
  if (main.kind==SourceKind::S && mirror.kind==SourceKind::S &&
      ISLAND_SIZE > 0 && main.w / ISLAND_SIZE == mirror.w / ISLAND_SIZE) {
    const WIndex famMin = (main.w / ISLAND_SIZE) * ISLAND_SIZE;
    if (main.w == famMin) makeChief(draft);
    else { draft.kind = SourceKind::D; draft.parent = famMin; draft.leader_w = famMin; }
    return;
  }
#endif
  if(promotesDelegate(main,mirror)) {makeChief(draft);return;}
  if(demotesChief(main,mirror)) {
    const WIndex chief=draft.kind==SourceKind::D?std::min(draft.parent,mirror.w):mirror.w;
    makeDelegate(draft,chief);return;
  }
  // Existing K/D identities are not overwritten by generic minimum merging.
  if(main.kind!=SourceKind::S || draft.kind!=SourceKind::S) return;
  WIndex chief=NO_PARENT;
  if(mirror.kind==SourceKind::K) chief=mirror.w;
  else if(mirror.kind==SourceKind::D) chief=mirror.parent;
  else if(mirror.kind==SourceKind::S) chief=main.w<mirror.w?main.w:mirror.w;
  if(chief>=W_USED) return;
  if(chief==main.w) makeChief(draft);
  else {draft.kind=SourceKind::D;draft.parent=chief;draft.leader_w=chief;}
}
}
