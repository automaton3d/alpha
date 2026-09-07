#include "inertia_fixture.h"
using namespace inertia_fixture;
int main() {
 try {
  if(!tryAllocate(9,243)) throw std::runtime_error(lastAllocationError);
  calculateParameters(9,243);initSimulation(0);replicate();
  FILE* out=std::fopen("build/seed_asymmetry/trace.csv","w");
  if(!out) throw std::runtime_error("cannot open trace");
  fprintf(out,"tick,layer_field_differences,inversion_differences,xy_differences,polarized_cells,moved_sources,pairs\n");
  unsigned firstDifference=0,firstMotion=0;
  for(unsigned tick=0;tick<=ENCOUNTER;++tick) {
   if(tick) simulation();
   unsigned layers=0,inversions=0,xy=0,polarized=0,moved=0,pairs=0;
   for(unsigned w=0;w<W_USED;++w) {
    for(unsigned x=0;x<EL;++x) for(unsigned y=0;y<EL;++y) for(unsigned z=0;z<EL;++z) {
     const Cell& c=getCell(lattice_curr,x,y,z,w);
     const Cell& ref=getCell(lattice_curr,x,y,z,0);
     const Cell& inv=getCell(lattice_curr,EL-1-x,EL-1-y,EL-1-z,w);
     const Cell& perm=getCell(lattice_curr,y,x,z,w);
     auto differs=[&](const Cell& b) {return c.u!=b.u || c.v!=b.v || c.r2!=b.r2 || c.active!=b.active;};
     layers+=differs(ref);inversions+=differs(inv);xy+=differs(perm);
     polarized+=c.pol_u!=0 || c.pol_v!=0;
    }
    moved+=lcenters[w]!=std::array<unsigned,3>{CENTER,CENTER,CENTER};
    pairs+=source(w).kind==SourceKind::P;
   }
   if(layers && !firstDifference) firstDifference=tick;
   if(moved && !firstMotion) firstMotion=tick;
   fprintf(out,"%u,%u,%u,%u,%u,%u,%u\n",tick,layers,inversions,xy,polarized,moved,pairs);
  }
  fclose(out);
  printf("OBSERVATION ticks=%u first_layer_difference=%u first_motion=%u (0 means none observed)\n",ENCOUNTER,firstDifference,firstMotion);
  return 0;
 } catch(const std::exception& e) {fprintf(stderr,"%s\n",e.what());return 2;}
}
