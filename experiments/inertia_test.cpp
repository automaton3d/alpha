#include "inertia_fixture.h"
#include <stdexcept>

using namespace inertia_fixture;

static double run(unsigned pairs,int direction=1,bool balanced=false,bool foreign=false,
                  int axis=0,int spread=0,bool elect=false) {
  prepare(9,5,5,3,pairs,axis,direction,balanced,foreign,elect,spread);
  s2b_target=16384;
  Measurement measurement(3);
  double sum=0;
  for(unsigned frame=1;frame<=48;) {
    if(!simulation()) continue;
    auto v=measurement.sample();
    if(frame>12) measurement.checkIdentity(pairs,foreign);
    if(frame>12) sum+=v[axis];
    for(unsigned p=0;p<pairs;++p) {
      int expected=(int)RMAX*direction*(balanced&&(p%2)?-1:1);
      assert(source(3+2*p).m[axis]==expected);
    }
    ++frame;
  }
  assert(measurement.maxSpread<=3);
  assert(measurement.maxPairGap<=2*RMAX);
  double speed=sum/36;
  printf("CASE pairs=%u direction=%d balanced=%d foreign=%d axis=%d spread=%d elect=%d v=%.6f span=%u\n",
         pairs,direction,balanced,foreign,axis,spread,elect,speed,measurement.maxSpread);
  return speed;
}

static void lifecycle() {
  prepare(9,5,5,3,1,0,1,false,true);
  Measurement measurement(3);
  double displacement[3]={0,0,0};
  for(unsigned frame=0;frame<96;) {
    if(!simulation()) continue;
    auto v=measurement.sample();
    unsigned stage=frame<24?0:(frame<72?1:2);
    displacement[stage]+=v[0];
    ++frame;
    if(frame==24||frame==72) {
      unsigned a=frame==24?0:3;
      for(unsigned w=3;w<5;++w) {
        source(w).a=a;source(w).leader_w=a;source(w).parent=a;
      }
      replicate(); // Experimental addition/removal of affiliation, no position forcing.
    }
  }
  assert(displacement[0]==0 && displacement[1]>0 && displacement[2]==0);
  printf("LIFECYCLE foreign=%.6f attached=%.6f detached=%.6f\n",displacement[0],displacement[1],displacement[2]);
}

static void oblique() {
  prepare(9,5,5,3,3);
  for(unsigned w=3;w<W_USED;++w) {source(w).m[0]=2;source(w).m[1]=1;}
  replicate();
  Measurement measurement(3);
  std::array<double,3> sum{0,0,0};
  for(unsigned frame=0;frame<64;) {
    if(!simulation()) continue;
    auto v=measurement.sample();
    measurement.checkIdentity(3);
    if(frame>=16) for(int k=0;k<3;++k) sum[k]+=v[k];
    ++frame;
  }
  assert(sum[0]>0 && sum[1]>0 && std::abs(sum[0]-2*sum[1])<1e-9 && sum[2]==0);
  assert(measurement.maxSpread<=3 && measurement.maxPairGap<=2*RMAX);
  printf("OBLIQUE displacement=(%.6f,%.6f,%.6f) span=%u\n",sum[0],sum[1],sum[2],measurement.maxSpread);
}

int main() {
  double rest=run(0);
  assert(rest==0);
  assert(run(0,1,false,false,0,2)==0);
  assert(run(0,1,false,false,0,0,true)==0);
  double low=run(1),high=run(3);
  assert(low>0 && high>low && high<=1);
  double left=run(1,-1);
  assert(std::abs(left+low)<1e-9);
  assert(std::abs(run(2,1,true))<0.1);
  assert(run(1,1,false,true)==0);
  assert(std::abs(run(1,1,false,false,1)-low)<1e-9);
  lifecycle();
  oblique();
  puts("PASS: rest, cohesion, election, density response, reversal, cancellation, affinity, axis and identity");
  return 0;
}
