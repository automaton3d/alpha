#include "inertia_fixture.h"
#include <string>
#ifndef INERTIA_MATCHED_FSM
#error Requires build_inertia_matched.bat
#endif
namespace automaton { int colorMatchedStep(unsigned,unsigned,int); }
namespace automaton { bool colorInertiaGrant(unsigned,unsigned); }
using namespace inertia_fixture;
int main(int argc,char** argv) {
 try {
  const std::string name=argc>=2?argv[1]:"island";
  const unsigned frames=argc>=3?unsigned(std::stoul(argv[2])):16;
  if(argc>3 || frames<2 || frames>256) throw std::runtime_error("frames must be 2..256");
  if(name!="rest" && name!="right" && name!="left" && name!="island" &&
     name!="foreign" && name!="broken" && name!="zero" && name!="many" && name!="island_left")
    throw std::runtime_error("cases: rest right left island island_left foreign broken zero many");
  const unsigned n=(name=="right" || name=="left")?1:3;
  const unsigned pairs=name=="rest"?0:(name=="many"?3:1);
  const int direction=(name=="left" || name=="island_left")?-1:1;
  if(!tryAllocate(9,n+2*pairs)) throw std::runtime_error(lastAllocationError);
  calculateParameters(9,n+2*pairs);initSimulation(0);
  for(Cell& s:lattice_curr) {
    const unsigned w=s.w;const bool prop=w>=n;
    s.kind=prop?SourceKind::P:(w==0?SourceKind::K:SourceKind::D);
    s.a=prop && name=="foreign"?n:0;s.leader_w=0;
    s.parent=w==0?NO_PARENT:0;
    s.ch=prop?((w-n)%2?0x1f:0x00):0x08;
    if(prop) {
      s.pair_idx=n+2*((w-n)/2)+((w-n)%2?0:1);s.pair_count=1;
      s.m[0]=name=="zero"?0:direction*int(RMAX);
      if(name=="broken" && w==n+1) s.pair_idx=NO_PAIR;
    }
  }
  replicate();
  std::vector<Cell> seed;
  for(unsigned w=0;w<W_USED;++w) seed.push_back(source(w));
  const std::string path="build/inertia_matched/"+name+".csv";
  FILE* f=fopen(path.c_str(),"w");if(!f) throw std::runtime_error("cannot open trace");
  fprintf(f,"frame,w,kind,x,y,z,unwrapped_x,impulse_x\n");
  std::vector<long long> unwrapped(W_USED,4);
  std::vector<std::array<unsigned,3>> previous=lcenters;
  std::vector<std::array<int,3>> impulses(W_USED);
  std::vector<std::array<unsigned,3>> journeySteps(W_USED);
  unsigned frame=0,maxSpread=0;long long kicksK=0,kicksD=0;
  for(unsigned tick=1;frame<frames;++tick) {
    const auto beforeTick=lcenters;
    const bool edge=simulation();
    for(unsigned w=0;w<W_USED;++w) for(int axis=0;axis<3;++axis) {
      journeySteps[w][axis]+=std::abs(wrappedDelta(beforeTick[w][axis],lcenters[w][axis],9));
      if(journeySteps[w][axis]>1) throw std::runtime_error("more than one actual step on an axis per journey");
    }
    if(lattice_curr.front().k==DIFFUSION) {
      for(unsigned w=0;w<W_USED;++w) {
        unsigned partners=0;
        for(unsigned b=0;b<W_USED;++b) {
          bool moving=false;
          for(int axis=0;axis<3;++axis) moving |= colorMatchedStep(w,b,axis)!=0;
          if(moving) {
            ++partners;
            if(w<n && b>=n) {
              kicksK+=w==0;kicksD+=w!=0;
              const unsigned half=seed[b].pair_idx;
              if(half>=W_USED) throw std::runtime_error("invalid pair granted a kick");
              if(!colorInertiaGrant(w,b) || !colorInertiaGrant(b,w) || !colorInertiaGrant(half,w))
                throw std::runtime_error("incomplete three-way agreement");
              for(int axis=0;axis<3;++axis)
                if(colorMatchedStep(b,w,axis)!=colorMatchedStep(half,w,axis))
                  throw std::runtime_error("pair halves disagree on reemission");
            }
          }
        }
        if(partners>1) throw std::runtime_error("multiple moving partners");
        for(int axis=0;axis<3;++axis) {
          const int c=source(w).c[axis];
          impulses[w][axis]=c>4?c-9:c;
          if(std::abs(impulses[w][axis])>1) throw std::runtime_error("unbounded impulse");
        }
      }
    }
    if(!edge) continue;
    ++frame;
    for(unsigned w=0;w<W_USED;++w) {
      const Cell& s=source(w);const Cell& initial=seed[w];
      if(s.kind!=initial.kind || s.parent!=initial.parent || s.w!=w ||
         s.a!=initial.a || s.ch!=initial.ch || s.pair_idx!=initial.pair_idx)
        throw std::runtime_error("source identity changed");
      for(int axis=0;axis<3;++axis) {
        if(s.m[axis]!=initial.m[axis]) throw std::runtime_error("momentum overwritten");
        const int delta=wrappedDelta(previous[w][axis],lcenters[w][axis],9);
        if(delta!=impulses[w][axis]) throw std::runtime_error("transport disagrees with c");
        if(frame==1 && delta) throw std::runtime_error("zero-radius kick");
        if(axis==0) unwrapped[w]+=delta;
      }
      previous[w]=lcenters[w];
      fprintf(f,"%u,%u,%u,%u,%u,%u,%lld,%d\n",frame,w,unsigned(s.kind),lcenters[w][0],lcenters[w][1],lcenters[w][2],unwrapped[w],impulses[w][0]);
    }
    for(unsigned p=n;p<W_USED;p+=2)
      if(lcenters[p]!=lcenters[p+1]) throw std::runtime_error("propeller split");
    for(unsigned a=0;a<n;++a) for(unsigned b=0;b<a;++b)
      maxSpread=std::max(maxSpread,unsigned(std::abs(wrappedDelta(lcenters[a][0],lcenters[b][0],9))));
    journeySteps.assign(W_USED,{0,0,0});
  }
  double displacement=0;
  for(unsigned w=0;w<n;++w) displacement+=double(unwrapped[w]-4)/n;
  fclose(f);
  if((name=="rest" || name=="foreign" || name=="broken" || name=="zero") && displacement!=0)
    throw std::runtime_error("negative control moved");
  printf("RESULT %s frames=%u body_displacement=%.6f mean_cells_per_frame=%.6f max_body_spread=%u PK=%lld PD=%lld\n",
         name.c_str(),frame,displacement,displacement/frame,maxSpread,kicksK,kicksD);
  puts("PASS integrity and controls; motion and persistence are measured outcomes.");
  return 0;
 } catch(const std::exception& e) {fprintf(stderr,"FAIL: %s\n",e.what());return 2;}
}
