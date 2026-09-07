#include "inertia_fixture.h"
#include "model/color_contact.h"
using namespace inertia_fixture;
void check(bool c,const char* message) {if(!c) throw std::runtime_error(message);}
int main() {
 try {
  // Independent reciprocity and exclusion checks over every ordered pair.
  for(unsigned a=0;a<243;++a) for(unsigned b=0;b<243;++b) for(unsigned color=0;color<8;++color) {
    auto d=colorContact(a,b,243,color,color,1,1);
    auto e=colorContact(b,a,243,color,color,1,1);
    for(int k=0;k<3;++k) check(d[k]==-e[k],"reciprocity failed");
    check(colorContact(a,b,243,color,color,0,1)==std::array<int,3>{0,0,0},"zero radius interacted");
    check(colorContact(a,b,243,color,color^8,1,1)==std::array<int,3>{0,0,0},"different charge interacted");
  }
  constexpr unsigned L=9,W=243;
  check(tryAllocate(L,W),"allocation failed");calculateParameters(L,W);initSimulation(0);replicate();
  // Reach the first positive shell by the existing distance update. This
  // prepared checkpoint isolates one encounter journey, not an era run.
  for(unsigned tick=0;tick<3*(L/2);++tick) update_pulsating_wavefront();
  for(Cell& c:lattice_curr) c.active=(c.r==1);
  replicate();
  for(unsigned shift=0;shift<W;++shift) {
    lattice_draft=lattice_curr;
    for(size_t i=0;i<lattice_curr.size();++i) {
      const Cell& a=lattice_curr[i];const Cell& b=lattice_partner[i];
      if(!a.active || !b.active) continue;
      auto d=colorContact(a.w,b.w,W,a.ch,b.ch,a.r,b.r);
      for(int k=0;k<3;++k)
        lattice_draft[i].c[k]=(int(a.c[k])+d[k]+int(L))%L;
    }
    lattice_curr.swap(lattice_draft);
    rotatePartners(); // frozen mirror state, one parallel W shift
  }
  // Read-only signed prediction for comparison, never used to move sources.
  std::vector<std::array<long long,3>> expected(W);
  for(unsigned a=0;a<W;++a) for(unsigned b=0;b<W;++b) {
    auto d=colorContact(a,b,W,source(a).ch,source(b).ch,1,1);
    for(int k=0;k<3;++k) expected[a][k]+=d[k];
  }
  // Transport the accumulated value from the shell, without adding copies.
  // A valid marker is initially present only at contact voxels.
  std::vector<unsigned char> valid(BLOCK),nextValid(BLOCK);
  for(size_t i=0;i<BLOCK;++i) valid[i]=lattice_curr[i].active;
  for(unsigned step=0;step<3*(L/2);++step) {
    lattice_draft=lattice_curr;nextValid=valid;
    for(unsigned w=0;w<W;++w) for(unsigned x=0;x<L;++x) for(unsigned y=0;y<L;++y) for(unsigned z=0;z<L;++z) {
      Cell& c=getCell(lattice_curr,x,y,z,w);
      size_t i=&c-lattice_curr.data();Cell& dst=lattice_draft[i];
      for(int dir=0;dir<6;++dir) {
        Cell& n=c.getNeighbor(dir);size_t j=&n-lattice_curr.data();
        if(!valid[j]) continue;
        if(nextValid[i]) for(int k=0;k<3;++k) check(dst.c[k]==n.c[k],"conflicting contact accumulators");
        else for(int k=0;k<3;++k) dst.c[k]=n.c[k];
        nextValid[i]=1;
      }
    }
    lattice_curr.swap(lattice_draft);valid.swap(nextValid);
  }
  for(size_t i=0;i<BLOCK;++i) {
    check(valid[i]!=0,"diffusion incomplete");
    for(int k=0;k<3;++k) check(lattice_curr[i].c[k]==(expected[lattice_curr[i].w][k]%L+L)%L,"accumulator mismatch");
  }
  // Local pull translation: every cell reads the previous tick only.
  // Residue L-1 realizes a negative unit displacement through L-1 positive
  // neighbor copies. No claim of physical speed is made by this schedule.
  for(int axis=0;axis<3;++axis) for(unsigned step=0;step<L-1;++step) {
    lattice_draft=lattice_curr;
    for(unsigned w=0;w<W;++w) for(unsigned x=0;x<L;++x) for(unsigned y=0;y<L;++y) for(unsigned z=0;z<L;++z) {
      const Cell& c=getCell(lattice_curr,x,y,z,w);
      if(!c.c[axis]) continue;
      unsigned p[3]={x,y,z};p[axis]=(p[axis]+L-1)%L;
      Cell& dst=getCell(lattice_draft,x,y,z,w);
      dst=getCell(lattice_curr,p[0],p[1],p[2],w);--dst.c[axis];
      dst.x[0]=x;dst.x[1]=y;dst.x[2]=z;dst.x[3]=w;
    }
    lattice_curr.swap(lattice_draft);
  }
  FILE* f=fopen("build/color_encounter/result.csv","w");check(f!=nullptr,"CSV failed");
  fprintf(f,"w,charge,dc_x,dc_y,dc_z,x,y,z\n");
  std::vector<unsigned> counts(W),occupied(L*L*L);unsigned distinct=0;
  long long total[3]={0,0,0};
  for(const Cell& c:lattice_curr) if(c.r2==0) {
    ++counts[c.w];unsigned code=(c.x[0]*L+c.x[1])*L+c.x[2];
    if(!occupied[code]++) ++distinct;
    for(int k=0;k<3;++k) {
      check(c.x[k]==(expected[c.w][k]%L+L+CENTER)%L,"translation mismatch");
      total[k]+=expected[c.w][k];
    }
    fprintf(f,"%u,%u,%lld,%lld,%lld,%u,%u,%u\n",c.w,c.ch,expected[c.w][0],expected[c.w][1],expected[c.w][2],c.x[0],c.x[1],c.x[2]);
  }
  fclose(f);for(unsigned n:counts) check(n==1,"missing or duplicate source");
  for(auto n:total) check(n==0,"net displacement is not zero");
  printf("RESULT W=%u distinct_centers=%u net_signed_displacement=(%lld,%lld,%lld)\n",W,distinct,total[0],total[1],total[2]);
  puts("PASS reciprocal rule, exclusions, voxel accumulation, local diffusion and local translation. Prepared positive-radius checkpoint; not a full simulation.");
  return 0;
 }catch(const std::exception& e){fprintf(stderr,"FAIL: %s\n",e.what());return 2;}
}
