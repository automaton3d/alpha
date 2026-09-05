// Prepared 1K+nD island with reciprocal, affinity-matched photon pairs.
#include "inertia_fixture.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

int main(int argc,char** argv) {
  using namespace inertia_fixture;
  if(argc<7) {
    fprintf(stderr,"usage: inertia_probe LX LY LZ N PAIRS FRAMES [SIEVE] [right|left|balanced|foreign|up] [CSV] [BURN]\n");
    return 1;
  }
  unsigned lx=atoi(argv[1]),ly=atoi(argv[2]),lz=atoi(argv[3]);
  unsigned n=atoi(argv[4]),pairs=atoi(argv[5]),frames=atoi(argv[6]);
  const char* mode=argc>8?argv[8]:"right";
  const char* path=argc>9?argv[9]:"inertia_run.csv";
  unsigned burn=argc>10?atoi(argv[10]):frames/4;
  if(lx<5||ly<5||lz<5||!(lx%2)||!(ly%2)||!(lz%2)||n<2||n>64||pairs>64||frames<=burn) return 2;
  bool foreign=strcmp(mode,"foreign")==0,balanced=strcmp(mode,"balanced")==0;
  int axis=strcmp(mode,"up")==0?1:0,direction=strcmp(mode,"left")==0?-1:1;
  if(strcmp(mode,"right")&&strcmp(mode,"left")&&strcmp(mode,"balanced")&&strcmp(mode,"foreign")&&strcmp(mode,"up")) return 2;
  s2b_target=argc>7?atoi(argv[7]):16384;
  prepare(lx,ly,lz,n,pairs,axis,direction,balanced,foreign);
  Measurement measurement(n);
  FILE* f=fopen(path,"w");
  if(!f) { perror(path); return 3; }
  fprintf(f,"frame,tick,measured,x,y,z,vx,vy,vz,max_body_span,max_pair_gap\n");
  std::array<double,3> sum{0,0,0};
  std::array<double,3> first{0,0,0},second{0,0,0};
  unsigned long long ticks=0;
  for(unsigned frame=1;frame<=frames;) {
    ++ticks;
    if(!simulation()) continue;
    auto velocity=measurement.sample();
    measurement.checkIdentity(pairs,foreign);
    std::array<double,3> position{0,0,0};
    for(unsigned w=0;w<n;++w) for(int k=0;k<3;++k)
      position[k]+=(double)measurement.unwrapped[w][k]/n;
    if(frame>burn) for(int k=0;k<3;++k) {
      sum[k]+=velocity[k];
      (frame<=burn+(frames-burn)/2?first:second)[k]+=velocity[k];
    }
    fprintf(f,"%u,%llu,%u,%.9f,%.9f,%.9f,%.9f,%.9f,%.9f,%u,%u\n",frame,ticks,
            frame>burn,position[0],position[1],position[2],velocity[0],velocity[1],velocity[2],measurement.maxSpread,measurement.maxPairGap);
    ++frame;
  }
  fclose(f);
  printf("INERTIA N=%u pairs=%u mode=%s burn=%u samples=%u mean_v=(%.6f,%.6f,%.6f) max_body_span=%u\n",
         n,pairs,mode,burn,frames-burn,sum[0]/(frames-burn),sum[1]/(frames-burn),sum[2]/(frames-burn),measurement.maxSpread);
  unsigned firstN=(frames-burn)/2,secondN=frames-burn-firstN;
  if(firstN) printf("WINDOWS first=(%.6f,%.6f,%.6f) second=(%.6f,%.6f,%.6f) max_pair_gap=%u\n",
    first[0]/firstN,first[1]/firstN,first[2]/firstN,second[0]/secondN,second[1]/secondN,second[2]/secondN,measurement.maxPairGap);
}
