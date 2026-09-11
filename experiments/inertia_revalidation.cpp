// Read-only diagnostics around the current production/matched kernels.
// Role transitions are observed, not frozen to the prepared K/D identities.
#include "inertia_fixture.h"
#include "model/island_identity.h"
#include <string>
#include <set>
#include <limits>
#include <climits>
using namespace inertia_fixture;
int main(int argc,char** argv) { try {
  if(argc!=3)throw std::runtime_error("usage: diagnostic CASE FRAMES");
  const std::string name=argv[1];const unsigned frames=std::stoul(argv[2]);
  const std::set<std::string> names={"rest","cohesion","right","left","island","island_left","many","balanced","foreign","broken","zero","up","oblique","lifecycle","large_one","large_many","saturated","election"};
  if(!names.count(name)||frames<24||frames>256)throw std::runtime_error("invalid case/frames");
  if(name=="lifecycle" && frames!=96)throw std::runtime_error("lifecycle requires 96 frames");
  const bool large=name=="large_one"||name=="large_many";
  const unsigned n=(name=="right"||name=="left")?1:(large?5:3);
  const unsigned pairs=(name=="rest"||name=="cohesion"||name=="election")?0:(name=="many"||name=="oblique"?3:(name=="balanced"?2:(name=="large_many"?4:(name=="saturated"?8:1))));
  const int direction=(name=="left"||name=="island_left")?-1:1;
  const int axis=name=="up"?1:0;
  const bool foreign=name=="foreign"||name=="lifecycle";
#ifdef INERTIA_MATCHED_FSM
  const std::string root="build/inertia_revalidation/matched/";
  if(!tryAllocate(large?15:9,n+2*pairs))throw std::runtime_error(lastAllocationError);
  calculateParameters(large?15:9,n+2*pairs);initSimulation(0);
  for(Cell& c:lattice_curr) {
    const unsigned w=c.w;const bool p=w>=n;
    c.kind=p?SourceKind::P:(name=="election"?SourceKind::S:(w?SourceKind::D:SourceKind::K));
    c.a=p&&foreign?n:0;c.leader_w=0;c.parent=w?0:NO_PARENT;
    c.ch=p?((w-n)%2?0x1f:0):0x08;
    if(p){c.pair_idx=n+2*((w-n)/2)+((w-n)%2?0:1);c.pair_count=1;
      c.m[axis]=direction*int(RMAX)*(name=="balanced"&&((w-n)/2)%2?-1:1);}
  }
  if(name=="cohesion")throw std::runtime_error("cohesion displacement is covered by production fixture only");
  replicate();
#else
  const std::string root="build/inertia_revalidation/production/";
  prepare(large?15:9,large?9:5,large?9:5,n,pairs,axis,direction,name=="balanced",foreign,name=="election",name=="cohesion"?2:0);
#endif
  for(Cell& c:lattice_curr)if(c.w>=n) {
    if(name=="broken"&&c.w==n+1)c.pair_idx=NO_PAIR;
    if(name=="zero")c.m[0]=c.m[1]=c.m[2]=0;
    if(name=="oblique"){c.m[0]=2;c.m[1]=1;}
  }
  replicate();s2b_target=16384;
  std::vector<Cell> seed;for(unsigned w=0;w<W_USED;++w)seed.push_back(source(w));
  FILE* trace=fopen((root+name+".csv").c_str(),"w");if(!trace)throw std::runtime_error("trace open failed");
  fprintf(trace,"frame,w,kind,parent,affinity,charge,pair,mx,my,mz,x,y,z,ux,uy,uz\n");
  std::vector<std::array<long long,3>> pos(W_USED),last(W_USED);
  for(unsigned w=0;w<W_USED;++w)for(int a=0;a<3;++a)pos[w][a]=last[w][a]=lcenters[w][a];
  const auto origin=pos;
  std::vector<unsigned> steps(W_USED,0);
  unsigned frame=0,errors=0,maxSteps=0,splitFrames=0,unresolvedFrames=0,changedRoles=0;
  long long maxSpan=0,maxGap=0;double sums[3]={0,0,0},first=0,second=0;
  const unsigned burn=frames/4,edges[]={ELX,ELY,ELZ};
  auto record=[&](){for(unsigned w=0;w<W_USED;++w){const Cell& c=source(w);fprintf(trace,"%u,%u,%u,%u,%u,%u,%u,%d,%d,%d,%u,%u,%u,%lld,%lld,%lld\n",frame,w,unsigned(c.kind),c.parent,c.a,unsigned(c.ch),c.pair_idx,c.m[0],c.m[1],c.m[2],lcenters[w][0],lcenters[w][1],lcenters[w][2],pos[w][0],pos[w][1],pos[w][2]);}};
  record();
  while(frame<frames) {
    const auto before=lcenters;const bool edge=simulation();
    for(unsigned w=0;w<W_USED;++w)for(int a=0;a<3;++a){int d=wrappedDelta(before[w][a],lcenters[w][a],edges[a]);pos[w][a]+=d;steps[w]+=std::abs(d);}
    if(!edge)continue;
    ++frame;
    std::vector<unsigned> sources(W_USED,0);
    for(const Cell& c:lattice_curr)if(c.r2==0){if(c.w>=W_USED){++errors;continue;}++sources[c.w];if(c.x[0]!=lcenters[c.w][0]||c.x[1]!=lcenters[c.w][1]||c.x[2]!=lcenters[c.w][2])++errors;}
    std::set<WIndex> chiefs;bool unresolved=false,rolesChanged=false;
    for(unsigned w=0;w<W_USED;++w) {
      const Cell& c=source(w);maxSteps=std::max(maxSteps,steps[w]);steps[w]=0;
      if(sources[w]!=1||c.w!=w||c.ch!=seed[w].ch)++errors;
      if(w<n) {
        if(c.a!=0)++errors;
        const auto chief=islandChief(c);
        if(chief<W_USED && source(chief).kind==SourceKind::K)chiefs.insert(chief);else unresolved=true;
        rolesChanged|=c.kind!=seed[w].kind || c.parent!=seed[w].parent;
        for(int a=0;a<3;++a)if(frame>burn)sums[a]+=double(pos[w][a]-last[w][a])/n;
        const double dx=double(pos[w][0]-last[w][0])/n;
        if(frame>burn && frame<=burn+(frames-burn)/2)first+=dx;
        else if(frame>burn)second+=dx;
      } else {
        const unsigned affinity=name=="lifecycle"?(frame<=24||frame>72?n:0):(foreign?n:0);
        if(c.kind!=SourceKind::P||c.a!=affinity||c.pair_idx!=seed[w].pair_idx||!c.pair_count)++errors;
        for(int a=0;a<3;++a)if(c.m[a]!=seed[w].m[a])++errors;
      }
      last[w]=pos[w];
    }
    if(frame>burn){splitFrames+=chiefs.size()!=1;unresolvedFrames+=unresolved;changedRoles+=rolesChanged;}
    for(unsigned a=0;a<n;++a)for(unsigned b=0;b<a;++b){long long d=0;for(int k=0;k<3;++k)d+=std::llabs(pos[a][k]-pos[b][k]);maxSpan=std::max(maxSpan,d);}
    for(unsigned p=n;p<W_USED;p+=2){long long nearest=LLONG_MAX;
      for(unsigned b=0;b<n;++b){long long d=0;for(int k=0;k<3;++k)d+=std::llabs(pos[p][k]-pos[b][k]);nearest=std::min(nearest,d);}
      maxGap=std::max(maxGap,nearest);
      if(pos[p]!=pos[p+1])++errors;
    }
    record();
    if(name=="lifecycle"&&(frame==24||frame==72)) {
      const unsigned affinity=frame==24?0:n;
#ifdef INERTIA_MATCHED_FSM
      for(Cell& c:lattice_curr)if(c.w>=n){c.a=affinity;c.leader_w=affinity;c.parent=affinity;}
#else
      for(unsigned w=n;w<W_USED;++w){source(w).a=affinity;source(w).leader_w=affinity;source(w).parent=affinity;}
#endif
      replicate(); // Controlled affiliation intervention, never position forcing.
    }
  }
  fclose(trace);
  FILE* f=fopen((root+name+".summary.csv").c_str(),"w");if(!f)throw std::runtime_error("summary open failed");
  fprintf(f,"case,frames,burn,N,pairs,W,frame_ticks,errors,max_steps,max_span,max_gap,split_frames,unresolved_frames,changed_role_frames,vx,vy,vz,first_dx,second_dx\n");
  fprintf(f,"%s,%u,%u,%u,%u,%u,%u,%u,%u,%lld,%lld,%u,%u,%u,%.12f,%.12f,%.12f,%.12f,%.12f\n",name.c_str(),frames,burn,n,pairs,W_USED,FRAME,errors,maxSteps,maxSpan,maxGap,splitFrames,unresolvedFrames,changedRoles,sums[0]/(frames-burn),sums[1]/(frames-burn),sums[2]/(frames-burn),first,second);fclose(f);
  printf("MEASURED %s integrity_errors=%u max_steps=%u span=%lld gap=%lld split=%u unresolved=%u vx=%.6f\n",name.c_str(),errors,maxSteps,maxSpan,maxGap,splitFrames,unresolvedFrames,sums[0]/(frames-burn));
  return errors?2:0; // Physical acceptance is evaluated separately by the auditor.
}catch(const std::exception& e){fprintf(stderr,"FAIL %s\n",e.what());return 2;}}
