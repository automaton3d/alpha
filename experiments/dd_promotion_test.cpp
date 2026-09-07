#include "model/chief_transition.h"
#include <cstdio>
#include <stdexcept>
namespace automaton { unsigned W_USED=243; }
using namespace automaton;
void check(bool v) {if(!v) throw std::runtime_error("D x D transition failed");}
int main() {
 try {
  for(unsigned charge=0;charge<64;++charge) for(unsigned different=0;different<2;++different) {
    Cell a{},b{};a.w=17;b.w=81;a.kind=b.kind=SourceKind::D;
    a.ch=b.ch=charge;a.a=3;b.a=99;a.parent=2;b.parent=different?7:2;
    a.leader_w=a.parent;b.leader_w=b.parent;a.sB=false;b.sB=true;
    Cell da=a,db=b;
    chiefContact(a,b,da);chiefContact(b,a,db);
    check(da.kind==SourceKind::K && da.parent==NO_PARENT && da.leader_w==a.w);
    check(db.kind==SourceKind::D && db.parent==b.parent);
    check(da.a==a.a && db.a==b.a && da.ch==a.ch && db.ch==b.ch);
    // A later K x K clash demotes the larger address, preserving its charge/affinity.
    Cell old=a;old.w=2;old.kind=SourceKind::K;
    Cell preserved=da;chiefContact(da,old,preserved);
    check(preserved.kind==SourceKind::D && preserved.parent==2 && preserved.leader_w==2);
    check(preserved.ch==da.ch && preserved.a==da.a);
    Cell survivor=old;chiefContact(old,da,survivor);check(survivor.kind==SourceKind::K);
    old.ch=charge^1;preserved=da;chiefContact(da,old,preserved);check(preserved.kind==SourceKind::K);
    b.ch=charge^1;da=a;db=b;chiefContact(a,b,da);chiefContact(b,a,db);
    check(da.kind==SourceKind::D && db.kind==SourceKind::D);
  }
  Cell s{},k{};s.w=1;s.kind=SourceKind::S;s.ch=8;s.a=77;
  k.w=80;k.kind=SourceKind::K;k.ch=8;k.a=12;
  Cell ds=s;chiefContact(s,k,ds);
  check(ds.kind==SourceKind::D && ds.parent==80 && ds.a==77);
  Cell high=k,mid=k,low=k;high.w=80;mid.w=40;low.w=2;
  Cell forward=high,reverse=high;
  chiefContact(high,mid,forward);chiefContact(high,low,forward);
  chiefContact(high,low,reverse);chiefContact(high,mid,reverse);
  check(forward.kind==SourceKind::D && forward.parent==2 && reverse.parent==2);
  Cell self=high;chiefContact(high,high,self);check(self.kind==SourceKind::K);
  puts("PASS all 64 charges; D promotion; reciprocal K clash; charge/affinity preservation; unequal-charge exclusion.");
  return 0;
 }catch(const std::exception& e){fprintf(stderr,"%s\n",e.what());return 2;}
}
