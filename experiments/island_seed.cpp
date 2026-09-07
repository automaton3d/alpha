#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <map>
#include <set>
#ifndef ISLAND_SEED_EXPERIMENT
#error Build with build_island_seed.bat
#endif
using namespace inertia_fixture;
int main() {
  setvbuf(stdout,nullptr,_IONBF,0);
  try {
    constexpr unsigned L=9, families=9*L, copies=L/3;
    // The cubic allocator allocates storage only. Parameters and center
    // addresses must be initialized before initSimulation or source reads.
    if (!tryAllocate(L,3*L*L)) throw std::runtime_error(lastAllocationError);
    calculateParameters(L,3*L*L);
    if(W_USED!=3*L*L || FRAME==0 || RMAX==0 || lcenters.size()!=W_USED ||
       lattice_curr.size()!=BLOCK || lattice_draft.size()!=BLOCK ||
       lattice_partner.size()!=BLOCK)
      throw std::runtime_error("incomplete lattice initialization");
    for(const auto& center:lcenters)
      if(center[0]>=ELX || center[1]>=ELY || center[2]>=ELZ)
        throw std::runtime_error("source center outside lattice");
    initSimulation(0);
    replicate();
    const auto origin=lcenters.at(0);
    for(unsigned w=0;w<W_USED;++w) {
      if(lcenters[w]!=origin || source(w).kind!=SourceKind::S)
        throw std::runtime_error("seed is not superposed S sources");
    }
    printf("SEED: L=%u W=%u families=%u copies=%u K=0 D=0 S=%u\n",L,W_USED,families,copies,W_USED);
    for(unsigned ticks=1;ticks<=2*FRAME;++ticks) {
      simulation();
      if(ticks!=FRAME && ticks!=2*FRAME) continue;
      unsigned nk=0,nd=0,ns=0,distinct=0;
      std::map<WIndex,unsigned> population;
      std::map<WIndex,std::set<unsigned>> origins;
      unsigned unresolved=0;
      for(unsigned w=0;w<W_USED;++w) {
        const Cell& s=source(w);
        nk+=s.kind==SourceKind::K;nd+=s.kind==SourceKind::D;ns+=s.kind==SourceKind::S;
        if(s.w!=w || s.a!=(w/copies)*copies)
          throw std::runtime_error("intrinsic address or affinity changed");
        if(s.kind==SourceKind::P) throw std::runtime_error("pair formed");
        WIndex chief=islandChief(s);
        if(chief==NO_PARENT) continue;
        ++population[chief];origins[chief].insert(w/copies);
        if(source(chief).kind!=SourceKind::K) ++unresolved;
        if(source(chief).ch!=s.ch) throw std::runtime_error("mixed-charge election");
      }
      for(unsigned w=0;w<W_USED;++w) {
        bool seen=false;
        for(unsigned q=0;q<w;++q) if(lcenters[q]==lcenters[w]) seen=true;
        distinct+=!seen;
      }
      printf("MEASURE tick=%u K=%u D=%u S=%u distinct_centers=%u\n",ticks,nk,nd,ns,distinct);
      unsigned mixed=0;
      for(const auto& [chief,n]:population) {
        mixed+=origins[chief].size()>1;
        printf("GROUP chief=%u population=%u seed_families=%zu\n",chief,n,origins[chief].size());
      }
      printf("IDENTITY mixed_groups=%u unresolved_parents=%u\n",mixed,unresolved);
      if(ticks==FRAME && (nk || nd || ns!=W_USED))
        throw std::runtime_error("zero-radius journey elected a chief");
    }
    puts("PASS election only; spatial separation and long-term stability are not established.");
    return 0;
  } catch(const std::exception& e) { fprintf(stderr,"FAIL: %s\n",e.what()); return 2; }
}
