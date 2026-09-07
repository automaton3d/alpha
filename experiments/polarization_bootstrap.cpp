#include "inertia_fixture.h"
#include "model/polarization.h"
#include "model/polarization_candidate.h"
using namespace inertia_fixture;
void check(bool ok) { if(!ok) throw std::runtime_error("bootstrap check failed"); }
int main() {
 try {
  check(tryAllocate(9,243));calculateParameters(9,243);initSimulation(0);replicate();
  unsigned nonzero=0;
  polarization::PolarizationCandidate candidate;
  for(const Cell& c:lattice_curr) {
    int u,v;polarization::reconstructPair(c.bstamp,2,u,v);
    nonzero+=u!=0 || v!=0;
    candidate.consider(u,v);
  }
  check(nonzero==0 && !candidate.unique());
  for(bool reverse:{false,true}) {
    polarization::PolarizationCandidate c;
    const int values[3][2]={{1,2},{3,1},{2,4}};
    for(int j=0;j<3;++j) { int i=reverse?2-j:j;c.consider(values[i][0],values[i][1]); }
    check(c.unique() && c.u==3 && c.v==1);
    c.consider(3,1);check(!c.unique());
  }
  puts("PASS checks: seed reconstruction is zero; selector is order independent and rejects tied maxima.");
  puts("RESULT: no initial polarization candidate. This is not a time-evolved spreading test.");
  return 0;
 } catch(const std::exception& e) { fprintf(stderr,"%s\n",e.what());return 2; }
}
