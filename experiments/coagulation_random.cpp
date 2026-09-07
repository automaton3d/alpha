#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <cstdint>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#ifndef COAGULATION_RANDOM_FSM
#error Requires build_coagulation_random.bat
#endif
using namespace inertia_fixture;
namespace {
// SplitMix64, fixed-width arithmetic and rejection sampling for reproducibility.
uint64_t rngState;
uint64_t random64() {
  uint64_t z=(rngState+=UINT64_C(0x9e3779b97f4a7c15));
  z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);
  z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);
  return z^(z>>31);
}
unsigned draw(unsigned bound) {
  const uint64_t threshold=(uint64_t(0)-bound)%bound;
  uint64_t x;do {x=random64();} while(x<threshold);
  return unsigned(x%bound);
}
unsigned frameNumber=0;
std::vector<std::array<unsigned,3>> pending,expected,previous;
std::vector<unsigned> witness,streak;
FILE* interventions=nullptr;
unsigned interventionCount=0;
void schedule() {
  pending.assign(W_USED,{0,0,0});
  for(unsigned w=0;w<W_USED;++w) {
    unsigned peer=W_USED;
    for(unsigned q=0;q<W_USED;++q) if(q!=w && lcenters[q]==lcenters[w]) {peer=q;break;}
    streak[w]=peer<W_USED?(witness[w]==peer?streak[w]+1:1):0;
    witness[w]=peer;
    // Four complete journeys first; interventions in journeys 5..8 only.
    // The final four journeys are unforced observations of coagulation.
    if(frameNumber<4 || frameNumber>=8 || streak[w]<3) continue;
    do { for(int axis=0;axis<3;++axis) pending[w][axis]=draw(EL); }
    while(pending[w]==std::array<unsigned,3>{0,0,0});
  }
}
}
namespace automaton {
void coagulationIntervene() {
  for(unsigned w=0;w<W_USED;++w) {
    const auto& p=lcenters[w];const Cell& s=getCell(lattice_draft,p[0],p[1],p[2],w);
    expected[w]={s.c[0],s.c[1],s.c[2]};
    if(pending[w]==std::array<unsigned,3>{0,0,0}) continue;
    fprintf(interventions,"%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
      frameNumber+1,w,witness[w],streak[w],p[0],p[1],p[2],
      unsigned(s.kind),unsigned(s.parent),s.c[0],s.c[1],s.c[2],
      pending[w][0],pending[w][1],pending[w][2]);
    expected[w]=pending[w];++interventionCount;
  }
  // Replaces c only for selected layers. Transport and reissue still execute
  // in the FSM; charge, affinity, phase, role and parent are not reset here.
  for(Cell& s:lattice_draft) if(pending[s.w]!=std::array<unsigned,3>{0,0,0})
    for(int axis=0;axis<3;++axis) s.c[axis]=pending[s.w][axis];
}
}
int main(int argc,char** argv) {
 try {
  if(argc>2) throw std::runtime_error("usage: coagulation_random [seed]");
  size_t used=0;
  const uint64_t seed=argc==2?std::stoull(argv[1],&used):1;
  if(argc==2 && used!=std::string(argv[1]).size()) throw std::runtime_error("invalid seed");
  rngState=seed;
  #if defined(KK_CLASH_FSM)
  const std::string directory="build/coagulation_clash/seed_"+std::to_string(seed);
  #elif defined(DD_PROMOTION_FSM)
  const std::string directory="build/coagulation_dd/seed_"+std::to_string(seed);
#else
  const std::string directory="build/coagulation_random/seed_"+std::to_string(seed);
#endif
  std::filesystem::create_directories(directory);
  if(!tryAllocate(9,243)) throw std::runtime_error(lastAllocationError);
  calculateParameters(9,243);initSimulation(0);replicate();
  FILE* trace=fopen((directory+"/trace.csv").c_str(),"w");
  FILE* members=fopen((directory+"/members.csv").c_str(),"w");
  FILE* groups=fopen((directory+"/groups.csv").c_str(),"w");
  interventions=fopen((directory+"/interventions.csv").c_str(),"w");
  if(!trace || !members || !groups || !interventions) throw std::runtime_error("cannot open diagnostics");
  for(FILE* f:{trace,members,groups,interventions}) setvbuf(f,nullptr,_IONBF,0);
  fprintf(trace,"frame,K,D,S,centers,chief_centers,groups_of_three,unresolved,interventions\n");
  fprintf(members,"frame,w,kind,parent,charge,affinity,x,y,z\n");
  fprintf(groups,"frame,chief,population,families,centers,max_distance_to_chief\n");
  fprintf(interventions,"journey,w,witness,streak,x,y,z,kind,parent,natural_cx,natural_cy,natural_cz,forced_cx,forced_cy,forced_cz\n");
  witness.assign(W_USED,W_USED);streak.assign(W_USED,0);
  expected.resize(W_USED);previous=lcenters;
  std::vector<unsigned> charges;
  for(unsigned w=0;w<W_USED;++w) charges.push_back(source(w).ch);
  for(;;) {
    unsigned nk=0,nd=0,ns=0,unresolved=0,three=0;
    std::set<std::array<unsigned,3>> centers,chiefCenters;
    std::map<WIndex,std::vector<unsigned>> population;
    std::set<WIndex> chiefs;
    for(unsigned w=0;w<W_USED;++w) {
      const Cell& s=source(w);
      if(s.w!=w || s.ch!=charges[w] || s.a!=(w/3)*3 || s.kind==SourceKind::P ||
         s.pair_idx!=NO_PAIR || s.m[0] || s.m[1] || s.m[2]) throw std::runtime_error("identity or no-inertia invariant failed");
      nk+=s.kind==SourceKind::K;nd+=s.kind==SourceKind::D;ns+=s.kind==SourceKind::S;
      centers.insert(lcenters[w]);
      if(s.kind==SourceKind::K) {chiefs.insert(w);chiefCenters.insert(lcenters[w]);}
      const WIndex chief=islandChief(s);
      if(chief!=NO_PARENT) {
        population[chief].push_back(w);
        unresolved+=source(chief).kind!=SourceKind::K;
        if(source(chief).ch!=s.ch) throw std::runtime_error("mixed-charge parent");
      }
      if(frameNumber) for(int axis=0;axis<3;++axis)
        if(lcenters[w][axis]!=(previous[w][axis]+expected[w][axis])%EL)
          throw std::runtime_error("c-driven displacement mismatch");
      fprintf(members,"%u,%u,%u,%u,%u,%u,%u,%u,%u\n",frameNumber,w,unsigned(s.kind),unsigned(s.parent),unsigned(s.ch),s.a,lcenters[w][0],lcenters[w][1],lcenters[w][2]);
    }
    for(const auto& [chief,ws]:population) {
      std::set<unsigned> families;std::set<std::array<unsigned,3>> sites;
      unsigned radius=0;
      for(unsigned w:ws) {
        families.insert(w/3);sites.insert(lcenters[w]);unsigned d=0;
        for(int axis=0;axis<3;++axis) d+=std::abs(wrappedDelta(lcenters[chief][axis],lcenters[w][axis],EL));
        radius=std::max(radius,d);
      }
      three+=ws.size()==EL/3;
      fprintf(groups,"%u,%u,%zu,%zu,%zu,%u\n",frameNumber,chief,ws.size(),families.size(),sites.size(),radius);
    }
    fprintf(trace,"%u,%u,%u,%u,%zu,%zu,%u,%u,%u\n",frameNumber,nk,nd,ns,centers.size(),chiefCenters.size(),three,unresolved,interventionCount);
    printf("FRAME %u K=%u D=%u S=%u centers=%zu size3=%u interventions=%u\n",frameNumber,nk,nd,ns,centers.size(),three,interventionCount);fflush(stdout);
    // Chief births are allowed outcomes. The intended D x D promotion must
    // not be rejected by a test that codifies the incomplete min-only rule.
    if(frameNumber==12) break;
    previous=lcenters;interventionCount=0;schedule();
    while(!simulation()) {}
    ++frameNumber;
  }
  for(FILE* f:{trace,members,groups,interventions}) fclose(f);
  puts("PASS transport and identity checks; island count and localization are measured outcomes.");
  return 0;
 } catch(const std::exception& e) {fprintf(stderr,"FAIL: %s\n",e.what());return 2;}
}
