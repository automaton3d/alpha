#include "inertia_fixture.h"
#include <cstdlib>
#include <map>
#include <set>
#include "model/island_identity.h"
#ifdef COLOR_MATCHED_FSM
#define COLOR_OUTPUT "build/color_matched/"
#elif defined(COLOR_COHESION_FSM)
#define COLOR_OUTPUT "build/color_cohesion/"
#elif defined(COLOR_CHIEF_FSM)
#define COLOR_OUTPUT "build/color_chief/"
#else
#define COLOR_OUTPUT "build/color_fsm/"
#endif
#include "model/color_contact.h"
namespace automaton { bool colorFsmContact(unsigned,unsigned); bool colorFsmReemitted(unsigned); }
#ifdef COLOR_COHESION_FSM
namespace automaton { int colorCohesionStep(unsigned,unsigned,int); }
#endif
#ifdef COLOR_MATCHED_FSM
namespace automaton { int colorMatchedStep(unsigned,unsigned,int); }
#endif
using namespace inertia_fixture;
#ifndef COLOR_ENCOUNTER_FSM
#error Requires build_color_fsm.bat
#endif
int main(int argc,char** argv) {
 try {
  unsigned frames=4;
  if(argc>2) throw std::runtime_error("usage: color_fsm [frames]");
  if(argc==2) {
    char* end;unsigned long n=strtoul(argv[1],&end,10);
    if(*end || n<1 || n>100) throw std::runtime_error("frames must be 1..100");
    frames=unsigned(n);
  }
  if(!tryAllocate(9,243)) throw std::runtime_error(lastAllocationError);
  calculateParameters(9,243);initSimulation(0);replicate();
  FILE* f=fopen(COLOR_OUTPUT "trace.csv","w");
  if(!f) throw std::runtime_error("cannot open trace");
  setvbuf(f,nullptr,_IONBF,0);
  fprintf(f,"tick,frame,k,distinct_centers,positive_sources\n");
  FILE* paths=fopen(COLOR_OUTPUT "trajectories.csv","w");
  FILE* contacts=fopen(COLOR_OUTPUT "contacts.csv","w");
  if(!paths || !contacts) throw std::runtime_error("diagnostic file open failed");
  setvbuf(paths,nullptr,_IONBF,0);setvbuf(contacts,nullptr,_IONBF,0);
  fprintf(paths,"tick,frame,k,w,x,y,z,t,cx,cy,cz,dcx,dcy,dcz,reemitted\n");
  fprintf(contacts,"tick,frame,a,b,reciprocal,dx,dy,dz\n");
  std::vector<std::array<long long,3>> impulses(W_USED);
#ifdef COLOR_CHIEF_FSM
  FILE* groups=fopen(COLOR_OUTPUT "groups.csv","w");
  if(!groups) throw std::runtime_error("cannot open groups");
  setvbuf(groups,nullptr,_IONBF,0);
  fprintf(groups,"tick,frame,chief,population,seed_families,distinct_centers,max_distance_to_chief,valid_chief\n");
#endif
  unsigned missingReciprocity=0;
  unsigned frame=0;
  for(unsigned tick=0;frame<frames;) {
    if(tick) {if(simulation()) ++frame;}
    unsigned k=lattice_curr.front().k;
    if(!tick || k==0 || k==ENCOUNTER || k==DIFFUSION || k==RELOC || k==REISSUE) {
      std::vector<unsigned> occupied(ELX*ELY*ELZ),sources(W_USED);
      unsigned distinct=0,positive=0;
      for(const Cell& c:lattice_curr) if(c.r2==0) {
        ++sources[c.w];
        unsigned code=(c.x[0]*ELY+c.x[1])*ELZ+c.x[2];
        distinct+=occupied[code]++==0;
        positive+=effective_t(c.t)>0;
#ifndef COLOR_CHIEF_FSM
        if(c.kind!=SourceKind::S) throw std::runtime_error("unexpected election");
#endif
        if(c.w>=W_USED || c.a!=(c.w/3)*3) throw std::runtime_error("affinity changed");
        if(c.kind==SourceKind::P || c.pair_idx!=NO_PAIR || c.m[0] || c.m[1] || c.m[2])
          throw std::runtime_error("unexpected pair, election or momentum");
      }
      for(unsigned n:sources) if(n!=1) throw std::runtime_error("source count failed");
      if(k==DIFFUSION) {
        impulses.assign(W_USED,{0,0,0});
        for(unsigned a=0;a<W_USED;++a) for(unsigned b=0;b<W_USED;++b) {
          if(!colorFsmContact(a,b)) continue;
          bool reciprocal=colorFsmContact(b,a);
          missingReciprocity+=!reciprocal;
          auto d=colorContact(a,b,W_USED,source(a).ch,source(b).ch,1,1);
#ifdef COLOR_COHESION_FSM
          for(int j=0;j<3;++j) {
            const int step=colorCohesionStep(a,b,j);
            if(step!=-colorCohesionStep(b,a,j)) throw std::runtime_error("nonreciprocal cohesion impulse");
            d[j]+=step;
          }
#endif
#ifdef COLOR_MATCHED_FSM
          for(int j=0;j<3;++j) {
            d[j]=colorMatchedStep(a,b,j);
            if(d[j]!=-colorMatchedStep(b,a,j)) throw std::runtime_error("nonreciprocal matching");
          }
#endif
          for(int j=0;j<3;++j) impulses[a][j]+=d[j];
          fprintf(contacts,"%u,%u,%u,%u,%d,%d,%d,%d\n",tick,frame,a,b,reciprocal,d[0],d[1],d[2]);
        }
#ifdef COLOR_MATCHED_FSM
        for(unsigned w=0;w<W_USED;++w) {
          unsigned movingPartners=0;
          for(unsigned b=0;b<W_USED;++b)
            movingPartners+=colorMatchedStep(w,b,0)!=0 || colorMatchedStep(w,b,1)!=0 || colorMatchedStep(w,b,2)!=0;
          if(movingPartners>1) throw std::runtime_error("multiple accepted movement partners");
          for(int j=0;j<3;++j) if(std::abs(impulses[w][j])>1) throw std::runtime_error("unbounded matched movement");
        }
#endif
        for(unsigned w=0;w<W_USED;++w) for(int j=0;j<3;++j)
          if(source(w).c[j]!=(impulses[w][j]%EL+EL)%EL)
            throw std::runtime_error("observed contacts disagree with c");
      }
      for(unsigned w=0;w<W_USED;++w) {
        const Cell& s=source(w);const auto& p=lcenters[w];
        fprintf(paths,"%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%lld,%lld,%lld,%d\n",
          tick,frame,k,w,p[0],p[1],p[2],s.t,s.c[0],s.c[1],s.c[2],
          impulses[w][0],impulses[w][1],impulses[w][2],
          (k==REISSUE || k==0) && tick>0 && colorFsmReemitted(w));
      }
      fprintf(f,"%u,%u,%u,%u,%u\n",tick,frame,k,distinct,positive);
#ifdef COLOR_CHIEF_FSM
      if(k==0) {
        std::map<WIndex,std::vector<unsigned>> members;
        unsigned nk=0,nd=0,ns=0,unresolved=0;
        for(unsigned w=0;w<W_USED;++w) {
          const Cell& s=source(w);
          nk+=s.kind==SourceKind::K;nd+=s.kind==SourceKind::D;ns+=s.kind==SourceKind::S;
          WIndex chief=islandChief(s);
          if(chief==NO_PARENT) continue;
          if(source(chief).ch!=s.ch) throw std::runtime_error("mixed-charge chief");
          unresolved+=source(chief).kind!=SourceKind::K;
          members[chief].push_back(w);
        }
        for(const auto& [chief,ws]:members) {
          std::set<unsigned> families;
          std::set<std::array<unsigned,3>> centers;
          unsigned radius=0;
          for(unsigned w:ws) {
            families.insert(w/3);centers.insert(lcenters[w]);
            unsigned distance=0;
            for(int j=0;j<3;++j) distance+=std::abs(wrappedDelta(lcenters[chief][j],lcenters[w][j],EL));
            radius=std::max(radius,distance);
          }
          fprintf(groups,"%u,%u,%u,%zu,%zu,%zu,%u,%d\n",tick,frame,chief,ws.size(),families.size(),centers.size(),radius,source(chief).kind==SourceKind::K);
        }
        printf("FRAME %u K=%u D=%u S=%u centers=%u unresolved=%u\n",frame,nk,nd,ns,distinct,unresolved);
        fflush(stdout);
        if(frame==1 && (nk || nd || distinct!=1)) throw std::runtime_error("zero-radius journey interacted");
      }
#endif
    }
    ++tick;
  }
#ifdef COLOR_CHIEF_FSM
  fclose(groups);
#endif
  fclose(f);fclose(paths);fclose(contacts);
  printf("OBSERVATION nonreciprocal_directed_contacts=%u\n",missingReciprocity);
  puts("PASS structural checks; see trace for spreading (not an acceptance condition).");
  return 0;
 } catch(const std::exception& e) {fprintf(stderr,"FAIL: %s\n",e.what());return 2;}
}
