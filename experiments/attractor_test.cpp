#include "model/attractor.h"
#include "model/simulation.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdio>
namespace automaton {
unsigned W_USED=8, EL=9, ELX=9, ELY=9, ELZ=9, ISLAND_SIZE=3, RMAX=4;
std::vector<Cell> lattice_curr;
}
using namespace automaton;
namespace a=automaton::attractor;
void check(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
Cell& source(unsigned w){return lattice_curr[w*2];}
std::string read(const char* path){std::ifstream f(path);std::ostringstream s;s<<f.rdbuf();return s.str();}
int main(){try{
 lattice_curr.resize(16);
 for(unsigned w=0;w<8;++w)for(unsigned v=0;v<2;++v){auto& c=lattice_curr[2*w+v];c.w=w;c.x[0]=v;c.x[1]=c.x[2]=0;c.x[3]=w;c.r2=v;c.a=0;c.ch=0;c.kind=SourceKind::S;}
 source(0).kind=source(4).kind=SourceKind::K;
 source(1).kind=source(5).kind=SourceKind::D;source(1).parent=0;source(5).parent=4;
 source(1).a=7;source(5).ch=0x27;
 a::begin();a::sampleFrame(1);auto r=a::summarize();
 check(r.nIslands==2 && r.meanTotalPop==4 && r.meanCaptures==0 && !r.pooledHasFit,"seed baseline or short report");
 source(2).kind=SourceKind::D;source(2).parent=0;source(2).t=RMAX;
 source(1).parent=4;source(5).kind=SourceKind::S;
 source(3).kind=SourceKind::D;source(3).parent=2;
 a::sampleFrame(2);r=a::summarize();
 check(r.meanCaptures==1 && r.meanEscapes==1 && r.census.unresolved==1,"capture/switch/escape");
 check(a::writeSectorCSV("build/attractor_test/flux.csv"),"write flux");
 check(read("build/attractor_test/flux.csv").find("2,2,0,0,0,1,0,0,1,0,0")!=std::string::npos,"sector signs / previous charge");
 source(4).kind=SourceKind::D;source(4).parent=0;a::sampleFrame(3);
 check(a::summarize().census.unresolved==2 && a::summarize().nIslands==1,"demoted chief / unresolved delegates");
 source(2).ch=7;a::sampleFrame(4);
 a::writeSectorCSV("build/attractor_test/flux.csv");
 check(read("build/attractor_test/flux.csv").find("4,0,0,0,0,0,0,0,0,-2,0")!=std::string::npos,"conversion is not capture");
 check(a::saveSeries("build/attractor_test/state.bin"),"save");
 a::sampleFrame(5);a::writeCSV("build/attractor_test/continuous.csv",a::summarize());
 a::begin();check(a::loadSeries("build/attractor_test/state.bin"),"load");a::resyncPrev();a::sampleFrame(5);
 a::writeCSV("build/attractor_test/resumed.csv",a::summarize());
 check(read("build/attractor_test/resumed.csv")==read("build/attractor_test/continuous.csv"),"resume continuity");
 {std::ofstream f("build/attractor_test/old.bin",std::ios::binary);f<<"old";}
 check(!a::loadSeries("build/attractor_test/old.bin") && a::framesSampled()==5,"reject invalid without changing history");
 bool rejected=false;try{a::sampleFrame(7);}catch(const std::runtime_error&){rejected=true;}check(rejected,"frame gaps");
 a::begin(a::Observable::AffinityCells);check(!a::loadSeries("build/attractor_test/state.bin"),"observable mismatch");
 a::sampleFrame(1);r=a::summarize();check(r.meanTotalPop==16 && r.nIslands==0,"affinity cells distinct from sources");
 source(0).a=W_USED;source(0).t=RMAX;a::sampleFrame(2);r=a::summarize();check(r.meanTotalPop==15.5 && r.meanEscapes==0.5,"turnaround balance");
 for(unsigned f=3;f<=12;++f)a::sampleFrame(f);
 a::printReport(a::summarize());
 for(auto& c:lattice_curr)c.a=W_USED;
 for(unsigned i=0;i<4;++i)lattice_curr[i].ch=(i/2)*32+(i%2)*7;
 a::begin(a::Observable::AffinityCells);
 for(unsigned i=0;i<4;++i)lattice_curr[i].a=0;
 a::sampleFrame(1);a::writeSectorCSV("build/attractor_test/four.csv");
 check(read("build/attractor_test/four.csv").find("1,1,1,1,1,0,0,0,0,0,0")!=std::string::npos,"all four capture classes");
 for(unsigned i=0;i<4;++i){lattice_curr[i].a=W_USED;lattice_curr[i].ch^=7;}
 a::sampleFrame(2);a::writeSectorCSV("build/attractor_test/four.csv");
 check(read("build/attractor_test/four.csv").find("2,0,0,0,0,1,1,1,1,0,0")!=std::string::npos,"escape uses old class");
 a::begin(a::Observable::AffinityCells);for(unsigned f=1;f<=12;++f)a::sampleFrame(f);
 check(!a::summarize().pooledHasFit,"constant empty series has no fit");
 puts("PASS baseline, chief identity across affinities, demotion, flux signs, conversion, turnaround, short series, checkpoint continuity and rejection.");return 0;
}catch(const std::exception& e){fprintf(stderr,"FAIL %s\n",e.what());return 1;}}
