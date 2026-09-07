#include "model/island_identity.h"
#include <cstdio>
#include <stdexcept>
namespace automaton { unsigned W_USED=243; }
using namespace automaton;
void check(bool c) {if(!c) throw std::runtime_error("island identity check failed");}
int main() {
 try {
  Cell k{},other{},d{},s{};
  k.w=3;k.kind=SourceKind::K;k.a=0;
  other.w=9;other.kind=SourceKind::K;other.a=0;
  d.w=4;d.kind=SourceKind::D;d.parent=3;d.a=17;
  s.w=5;s.kind=SourceKind::S;s.a=0;s.leader_w=3;
  check(shareChief(k,d)); // different affinity, same chief
  check(!shareChief(k,other)); // same affinity, different chief
  check(!shareChief(k,s)); // seeded leader is not a membership
  d.parent=9;check(shareChief(other,d));check(!shareChief(k,d));
  d.parent=NO_PARENT;check(!shareChief(k,d));
  Cell a{},b{};
  a.w=0;b.w=24;a.kind=b.kind=SourceKind::S;
  a.ch=b.ch=0;a.a=0;b.a=24;
  check(canElectChief(a,b)); // distinct seed families and affinities
  b.ch=1;check(!canElectChief(a,b));
  b.ch=0;b.kind=SourceKind::P;check(!canElectChief(a,b));
  b.kind=SourceKind::S;b.w=a.w;check(!canElectChief(a,b));
  puts("PASS: chief identity independent of affinity; S has no island membership.");
  return 0;
 }catch(const std::exception& e){fprintf(stderr,"%s\n",e.what());return 2;}
}
