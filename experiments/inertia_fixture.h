#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <vector>
#include "model/simulation.h"
#include "config.h"

// Standalone CPU harness; the simulation and encounters are production code.
std::vector<unsigned int> voxels;
Config gConfig;
namespace automaton {
bool convol_delay=false, diffuse_delay=false, reloc_delay=false;
bool tryEnableCuda() { return false; }
void disableCuda() {}
bool isCudaEnabled() { return false; }
bool swap_lattices_gpu() { return false; }
}

namespace inertia_fixture {
using namespace automaton;

inline Cell& source(unsigned w) {
  const auto& p=lcenters[w];
  return getCell(lattice_curr,p[0],p[1],p[2],w);
}
inline int wrappedDelta(int a,int b,unsigned edge) {
  int d=b-a;
  if(d>(int)edge/2) d-=(int)edge;
  if(d<-(int)edge/2) d+=(int)edge;
  return d;
}

// A prepared island is an initial-condition experiment, not a claim that
// the Platonic seed has already thermalized or produced an electron.
inline void prepare(unsigned lx,unsigned ly,unsigned lz,unsigned n,unsigned pairs,
                    int axis=0,int direction=1,bool opposed=false,bool foreign=false,
                    bool singletons=false,int spread=0) {
  assert(n>=1);
  if(!tryAllocateTube(lx,ly,lz,n+2*pairs)) throw std::runtime_error(lastAllocationError);
  initSimulation(0);
  for(unsigned w=0;w<W_USED;++w) {
    bool prop=w>=n;
    unsigned affinity=(foreign&&prop)?n:0;
    auto center=std::array<unsigned,3>{lx/2,ly/2,lz/2};
    const unsigned edges[]={lx,ly,lz};
    if(!prop && spread && w==n-1) center[axis]=(center[axis]+spread+edges[axis])%edges[axis];
    lcenters[w]=center;
    const unsigned char charge=prop?((w-n)%2?0x1f:0x00):0x08;
    for(unsigned x=0;x<lx;++x)
    for(unsigned y=0;y<ly;++y)
    for(unsigned z=0;z<lz;++z) {
      Cell& c=getCell(lattice_curr,x,y,z,w);
      c=Cell{}; c.w=w; c.ch=charge;
      c.x[0]=x;c.x[1]=y;c.x[2]=z;c.x[3]=w;
      c.a=affinity;c.leader_w=affinity;
    }
    Cell& s=source(w);
    s.r2=0;s.r=0;s.u=2048;
    s.kind=prop?SourceKind::P:(singletons?SourceKind::S:(w==0?SourceKind::K:SourceKind::D));
    s.parent=prop||w>0?affinity:NO_PARENT;
    if(prop) {
      unsigned p=(w-n)/2;
      s.pair_idx=n+2*p+((w-n)%2?0:1);
      s.pair_count=1;
      s.m[axis]=(int)RMAX*direction*(opposed&&(p%2)?-1:1);
    }
  }
  replicate();
}

struct Measurement {
  unsigned n;
  std::vector<std::array<unsigned,3>> previous;
  std::vector<std::array<long long,3>> unwrapped;
  unsigned maxSpread=0;
  unsigned maxPairGap=0;
  explicit Measurement(unsigned bodyCount):n(bodyCount),previous(lcenters),unwrapped(W_USED) {
    for(unsigned w=0;w<W_USED;++w)
      for(int k=0;k<3;++k) unwrapped[w][k]=lcenters[w][k];
  }
  std::array<double,3> sample() {
    const unsigned edges[]={ELX,ELY,ELZ};
    std::array<double,3> velocity{0,0,0};
    for(unsigned w=0;w<W_USED;++w) {
      int length=0;
      for(int k=0;k<3;++k) {
        int d=wrappedDelta(previous[w][k],lcenters[w][k],edges[k]);
        length+=std::abs(d);unwrapped[w][k]+=d;
        if(w<n) velocity[k]+=(double)d/n;
      }
      assert(length<=1); // Every constituent, including both P halves.
      previous[w]=lcenters[w];
    }
    for(unsigned a=0;a<n;++a) for(unsigned b=a+1;b<n;++b) {
      unsigned d=0;
      for(int k=0;k<3;++k) d+=std::abs(wrappedDelta(lcenters[a][k],lcenters[b][k],edges[k]));
      maxSpread=std::max(maxSpread,d);
    }
    for(unsigned p=n;p<W_USED;p+=2) {
      unsigned nearest=ELX+ELY+ELZ;
      for(unsigned q=0;q<n;++q) {
        unsigned d=0;
        for(int k=0;k<3;++k) d+=std::abs(wrappedDelta(lcenters[p][k],lcenters[q][k],edges[k]));
        nearest=std::min(nearest,d);
      }
      maxPairGap=std::max(maxPairGap,nearest);
    }
    return velocity;
  }
  void checkIdentity(unsigned pairs,bool foreign=false) const {
    for(unsigned w=0;w<n;++w) {
      const Cell& s=source(w);
      if(s.kind!=(w==0?SourceKind::K:SourceKind::D))
        fprintf(stderr,"identity w=%u kind=%u leader=%u a=%u clock=%u\n",w,(unsigned)s.kind,s.leader_w,s.a,s.t);
      assert(s.kind==(w==0?SourceKind::K:SourceKind::D));
      assert(s.ch==0x08 && s.a==0 && s.leader_w==0);
      assert(s.parent==(w==0?NO_PARENT:0u));
    }
    for(unsigned p=0;p<pairs;++p) {
      unsigned a=n+2*p,b=a+1;
      assert(source(a).kind==SourceKind::P && source(b).kind==SourceKind::P);
      assert(source(a).pair_idx==b && source(b).pair_idx==a);
      assert(source(a).a==(foreign?n:0) && source(b).a==source(a).a);
      assert(lcenters[a]==lcenters[b]);
    }
  }
};
}
