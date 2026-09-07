#pragma once
#include "model/island_identity.h"
namespace automaton {
inline bool promotesDelegate(const Cell& main,const Cell& mirror) {
  // Active/radius contact gates belong to encounter. No affinity, parent,
  // spin, separation, or population condition is imposed here.
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
