// Conditional first-election geometry, NOT a time-evolved CA simulation.
#include "model/election_payload.h"
#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
using automaton::polarization::electionPayload;
int main(int argc,char** argv) {
 try {
  if(argc!=3) throw std::runtime_error("usage: first_election L output.csv");
  char* end=nullptr; unsigned L=std::strtoul(argv[1],&end,10);
  if(*end || L<9 || L>255 || L%2==0 || L%3) throw std::runtime_error("L must be odd, divisible by 3, in [9,255]");
  unsigned R=L/2, W=3*L*L, families=9*L, copies=L/3;
  std::vector<unsigned> shell;
  for(unsigned x=0;x<L;++x) for(unsigned y=0;y<L;++y) for(unsigned z=0;z<L;++z) {
   int dx=int(x)-R,dy=int(y)-R,dz=int(z)-R;
   unsigned d2=dx*dx+dy*dy+dz*dz;
   // Exactly the reference integer-radius active-shell predicate.
   if(d2>=R*R && d2<(R+1)*(R+1)) shell.push_back((x*L+y)*L+z);
  }
  // Identical candidate shell: only 9L distinct election keys exist.
  std::vector<unsigned> winner(families);
  for(unsigned key=0;key<families;++key) {
   unsigned best=0; bool any=false;
   for(unsigned code:shell) {
    unsigned p=electionPayload(key,code);
    if(!any || p>best) { any=true;best=p;winner[key]=code; }
   }
  }
  FILE* out=std::fopen(argv[2],"w");
  if(!out) throw std::runtime_error("cannot open CSV");
  std::fprintf(out,"w,family,key,x,y,z,r_after,reverse_x,reverse_y,reverse_z\n");
  std::map<unsigned,unsigned> occupancy;
  unsigned changed=0; long long sx=0,sy=0,sz=0;
  for(unsigned w=0;w<W;++w) {
   unsigned family=w/copies;
   unsigned key=(family+w+1)%families; // dial increments before election
   unsigned code=winner[key],rev=winner[(family+W-w)%families];
   unsigned x=code/(L*L),y=(code/L)%L,z=code%L;
   ++occupancy[code]; changed+=code!=rev;
   sx+=int(x)-int(R);sy+=int(y)-int(R);sz+=int(z)-int(R);
   std::fprintf(out,"%u,%u,%u,%u,%u,%u,0,%u,%u,%u\n",w,family,key,x,y,z,rev/(L*L),(rev/L)%L,rev%L);
  }
  std::fclose(out);
  unsigned maximum=0;for(auto [c,n]:occupancy) maximum=std::max(maximum,n);
  std::printf("L=%u W=%u shell_sites=%zu distinct_centers=%zu max_occupancy=%u reverse_changed=%u mean_offset=(%.6f,%.6f,%.6f)\n",L,W,shell.size(),occupancy.size(),maximum,changed,double(sx)/W,double(sy)/W,double(sz)/W);
  std::puts("Conditional reemission at exact winner, radius zero. No propagation, local relocation or subsequent aggregation simulated.");
  return 0;
 } catch(const std::exception& e) { std::fprintf(stderr,"%s\n",e.what());return 2; }
}
