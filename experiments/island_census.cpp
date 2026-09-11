// island_census.cpp — per-light-frame island census on the PRODUCTION path.
//
// Runs the ordinary CPU simulation() (the reference dynamics, NOT the
// COLOR_ENCOUNTER_FSM candidate) on the canonical Platonic superposed seed
// L=9, W=243 (9L=81 charge families of L/3=3 copies), for a configurable
// number of light-frame journeys, and writes, at every frame boundary:
//   census.csv      one row per frame  (K/D/S/P, groups, spatial spread,
//                                       capture/escape, births/deaths)
//   groups.csv      one row per group per frame (population, mixed families,
//                   distinct centres, span to the chief)
//   summary.txt     per-chief statistics over the run + final report card.
//
// This is measurement campaign 1A: it checks whether the production dynamics
// convert the seed's chief associations into localised, persistent 1K+nD
// groups (manuscript: 9L spatial islands of ~L/3 constituents is a hypothesis
// to test), and whether the restored D x D promotion creates new chiefs after
// spatial separation.  It imposes no island count and no population quota.
//
// Invariants checked at every frame boundary:
//   - exactly one r2==0 source centre per W layer, located at lcenters[w];
//   - intrinsic address preserved (s.w == w) on the source-centre cell;
//   - a D parent naming a non-K (or absent) W is counted as unresolved;
//   - groups are reported by identity (islandChief/shareChief), never by the
//     affinity field, because the full dynamics legitimately reassigns a.

#include "inertia_fixture.h"
#include "model/island_identity.h"
#include "model/attractor.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

using namespace inertia_fixture;
using namespace automaton;

namespace
{
  // Shortest toroidal distance on one axis (centres are inside [0,edge)).
  inline int axisDist(int a, int b, unsigned edge)
  {
    int d = b - a;
    if (d > (int)(edge / 2)) d -= (int)edge;
    if (d < -(int)(edge / 2)) d += (int)edge;
    return d;
  }

  inline unsigned manhattan(const std::array<unsigned, 3>& a,
                            const std::array<unsigned, 3>& b)
  {
    return (unsigned)(std::abs(axisDist((int)a[0], (int)b[0], ELX)) +
                      std::abs(axisDist((int)a[1], (int)b[1], ELY)) +
                      std::abs(axisDist((int)a[2], (int)b[2], ELZ)));
  }

  // Per-chief statistics accumulated across sampled frame boundaries.
  struct ChiefStat
  {
    WIndex    chief          = 0;
    unsigned  firstFrame     = 0;
    unsigned  lastFrame      = 0;
    unsigned  framesPresent  = 0;
    unsigned  populationLast = 0;
    unsigned  populationMax  = 0;
    unsigned  spanLast       = 0;
    unsigned  spanMax        = 0;
    unsigned  familiesMax    = 0;
    unsigned  centersLast    = 0;
    unsigned  curStable      = 0;  // consecutive frames with identical member set
    unsigned  maxStable      = 0;  // longest such run
  };

  struct Snapshot
  {
    unsigned frame = 0;
    unsigned tick  = 0;

    unsigned k = 0, d = 0, s = 0, p = 0;
    unsigned distinctCenters = 0;
    unsigned groups          = 0;   // distinct chiefs with members
    unsigned unresolved      = 0;   // D naming a non-K (or absent) parent
    unsigned mixedCharge     = 0;   // groups whose members carry >1 charge word

    unsigned captures = 0, escapes = 0, births = 0, deaths = 0;

    unsigned maxPopulation = 0;
    unsigned maxSpan       = 0;
    unsigned maxFamilies   = 0;

    // chief -> sorted member W list (membership signature for this frame)
    std::map<WIndex, std::vector<unsigned>> members;
  };

  unsigned copiesPerFamily = 0;   // ISLAND_SIZE = L/3 (3 for L=9, W=243)
  std::map<WIndex, std::vector<unsigned>> prevMembers;
  std::map<WIndex, ChiefStat> stats;

  void checkLatticeSources()
  {
    // Exactly one r2==0 centre per layer, located at lcenters[w].
    std::vector<unsigned> count(W_USED, 0);
    std::vector<std::array<unsigned, 3>> where(W_USED, {0u, 0u, 0u});
    for (const Cell& c : lattice_curr)
    {
      if (c.r2 != 0u) continue;
      if (c.w >= W_USED) throw std::runtime_error("source centre outside W");
      if (++count[c.w] > 1u) throw std::runtime_error("duplicated source centre");
      where[c.w] = {c.x[0], c.x[1], c.x[2]};
    }
    for (unsigned w = 0; w < W_USED; ++w)
    {
      if (count[w] != 1u)
        throw std::runtime_error("source centre lost during translation");
      if (where[w] != lcenters[w])
        throw std::runtime_error("source centre disagrees with lcenters");
    }
  }

  Snapshot sample(unsigned frame, unsigned tick, FILE* groups)
  {
    Snapshot sn;
    sn.frame = frame;
    sn.tick  = tick;

    checkLatticeSources();

    std::map<WIndex, std::set<unsigned char>> chargeSets;
    std::set<std::array<unsigned, 3>> allCentres;

    for (unsigned w = 0; w < W_USED; ++w)
    {
      const Cell& s = source(w);
      if (s.w != w) throw std::runtime_error("intrinsic W address changed");
      allCentres.insert(lcenters[w]);

      switch (s.kind)
      {
        case SourceKind::K: ++sn.k; break;
        case SourceKind::D: ++sn.d; break;
        case SourceKind::S: ++sn.s; break;
        case SourceKind::P: ++sn.p; break;
      }

      const WIndex chief = islandChief(s);
      if (chief == NO_PARENT)
      {
        if (s.kind == SourceKind::D) ++sn.unresolved;  // orphaned delegate
        continue;
      }
      if (source(chief).kind != SourceKind::K) { ++sn.unresolved; continue; }

      sn.members[chief].push_back(w);
      chargeSets[chief].insert(s.ch);
    }

    sn.distinctCenters = (unsigned)allCentres.size();
    sn.groups = (unsigned)sn.members.size();

    for (auto& [chief, vec] : sn.members)
    {
      std::sort(vec.begin(), vec.end());

      std::set<unsigned> families;
      std::set<unsigned> centres;
      unsigned span = 0;
      for (unsigned w : vec)
      {
        families.insert(w / copiesPerFamily);
        span = std::max(span, manhattan(lcenters[chief], lcenters[w]));
      }
      for (unsigned w : vec)
        centres.insert((lcenters[w][0] * ELY + lcenters[w][1]) * ELZ +
                       lcenters[w][2]);

      const unsigned population = (unsigned)vec.size();
      const unsigned nFamilies  = (unsigned)families.size();
      const unsigned nCentres   = (unsigned)centres.size();

      sn.maxPopulation = std::max(sn.maxPopulation, population);
      sn.maxSpan       = std::max(sn.maxSpan, span);
      sn.maxFamilies   = std::max(sn.maxFamilies, nFamilies);
      if (chargeSets[chief].size() > 1u) ++sn.mixedCharge;

      ChiefStat& st = stats[chief];
      if (st.framesPresent == 0) { st.firstFrame = frame; st.chief = chief; }
      st.lastFrame = frame;
      ++st.framesPresent;
      st.populationLast = population;
      st.populationMax  = std::max(st.populationMax, population);
      st.spanLast       = span;
      st.spanMax        = std::max(st.spanMax, span);
      st.familiesMax    = std::max(st.familiesMax, nFamilies);
      st.centersLast    = nCentres;

      const auto it = prevMembers.find(chief);
      const bool sameAsPrev = (it != prevMembers.end()) && it->second == vec;
      st.curStable = sameAsPrev ? st.curStable + 1u : 1u;
      st.maxStable = std::max(st.maxStable, st.curStable);

      if (groups)
        fprintf(groups, "%u,%u,%u,%u,%u,%u,%u,%u,%d\n", frame, tick, chief,
                population, nFamilies, nCentres, span,
                (unsigned)chargeSets[chief].size(),
                source(chief).kind == SourceKind::K ? 1 : 0);
    }

    // Capture/escape events plus chief births/deaths vs the previous frame.
    for (const auto& [chief, vec] : sn.members)
    {
      const auto prev = prevMembers.find(chief);
      if (prev == prevMembers.end())
      {
        ++sn.births;
        sn.captures += (unsigned)vec.size();
        continue;
      }
      for (unsigned w : vec)
        if (!std::binary_search(prev->second.begin(), prev->second.end(), w))
          ++sn.captures;
    }
    for (const auto& [chief, vec] : prevMembers)
    {
      const auto cur = sn.members.find(chief);
      if (cur == sn.members.end())
      {
        ++sn.deaths;
        sn.escapes += (unsigned)vec.size();
        continue;
      }
      for (unsigned w : vec)
        if (!std::binary_search(cur->second.begin(), cur->second.end(), w))
          ++sn.escapes;
    }
    prevMembers = sn.members;
    return sn;
  }

  void writeCensusRow(FILE* f, const Snapshot& sn)
  {
    fprintf(f,
            "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
            sn.frame, sn.tick, sn.k, sn.d, sn.s, sn.p,
            sn.distinctCenters, sn.groups, sn.unresolved, sn.mixedCharge,
            sn.maxPopulation, sn.maxSpan, sn.maxFamilies,
            sn.captures, sn.escapes, sn.births, sn.deaths);
  }
}  // namespace

int main(int argc, char** argv)
{
  setvbuf(stdout, nullptr, _IONBF, 0);
  try
  {
    unsigned frames = 128;        // default: 128 light-frame journeys
    int sieve = 16384;            // reference electroweak sieve modulus
    const char* outdir = "build/island_census";

    if (argc > 4)
      throw std::runtime_error("usage: island_census [frames] [sieve] [outdir]");
    if (argc >= 2)
    {
      char* end = nullptr;
      unsigned long v = strtoul(argv[1], &end, 10);
      if (end == argv[1] || *end || v < 1 || v > 100000)
        throw std::runtime_error("frames must be 1..100000");
      frames = (unsigned)v;
    }
    if (argc >= 3)
    {
      char* end = nullptr;
      long v = strtol(argv[2], &end, 10);
      if (end == argv[2] || *end || v < 0)
        throw std::runtime_error("sieve must be >= 0");
      sieve = (int)v;
    }
    if (argc == 4) outdir = argv[3];

    constexpr unsigned L = 9;
    if (!tryAllocate(L, 3u * L * L)) throw std::runtime_error(lastAllocationError);
    calculateParameters(L, 3u * L * L);
    if (W_USED != 3u * L * L || FRAME == 0 || RMAX == 0 || ISLAND_SIZE == 0)
      throw std::runtime_error("incomplete lattice initialization");
    copiesPerFamily = ISLAND_SIZE;
    if (9u * L * copiesPerFamily != W_USED)
      throw std::runtime_error("topology is not 9L families of L/3 copies");

    initSimulation(0);
    replicate();
    attractor::begin();

    char censusPath[512], groupsPath[512], summaryPath[512];
    snprintf(censusPath, sizeof censusPath, "%s/census.csv", outdir);
    snprintf(groupsPath, sizeof groupsPath, "%s/groups.csv", outdir);
    snprintf(summaryPath, sizeof summaryPath, "%s/summary.txt", outdir);

    FILE* census = fopen(censusPath, "w");
    FILE* groups = fopen(groupsPath, "w");
    FILE* sumf   = fopen(summaryPath, "w");
    if (!census || !groups || !sumf)
      throw std::runtime_error("cannot open output files");
    setvbuf(census, nullptr, _IONBF, 0);
    setvbuf(groups, nullptr, _IONBF, 0);

    fprintf(census,
            "frame,tick,K,D,S,P,distinct_centers,groups,unresolved,"
            "mixed_charge,max_population,max_span,max_families,"
            "captures,escapes,births,deaths\n");
    fprintf(groups,
            "frame,tick,chief,population,seed_families,distinct_centers,"
            "max_distance_to_chief,charge_words,valid_chief\n");

    printf("ISLAND_CENSUS L=%u W=%u families=%lu copies=%u sieve=%d frames=%u out=%s\n",
           L, W_USED, (unsigned long)(9u * L), copiesPerFamily, sieve, frames,
           outdir);
    printf("FRAME ticks/frame=%u tick budget=%llu\n", FRAME,
           (unsigned long long)FRAME * frames);

    s2b_target = sieve;

    // Baseline snapshot before any journey (frame 0).
    {
      Snapshot base = sample(0u, 0u, groups);
      writeCensusRow(census, base);
      printf("FRAME %3u K=%u D=%u S=%u P=%u centers=%u groups=%u unresolved=%u\n",
             base.frame, base.k, base.d, base.s, base.p, base.distinctCenters,
             base.groups, base.unresolved);
    }

    const auto t0 = std::chrono::steady_clock::now();
    unsigned frame = 0;
    while (frame < frames)
    {
      if (simulation())   // one light-frame journey completed
      {
        ++frame;
        Snapshot sn = sample(frame, pulse_tick, groups);
        attractor::sampleFrame(frame);
        const auto measured=attractor::summarize().census;
        if(measured.chiefs!=sn.k || measured.delegates!=sn.d ||
           measured.singletons!=sn.s || measured.pairs!=sn.p ||
           measured.unresolved!=sn.unresolved || measured.occupiedCenters!=sn.distinctCenters || sn.groups!=sn.k)
          throw std::runtime_error("independent chief censuses disagree");
        writeCensusRow(census, sn);
        if ((frame % 16u) == 0u || frame == frames)
        {
          const auto now = std::chrono::steady_clock::now();
          const double sec = std::chrono::duration<double>(now - t0).count();
          printf("FRAME %3u K=%u D=%u S=%u P=%u centers=%u groups=%u "
                 "unresolved=%u captures=%u escapes=%u births=%u deaths=%u "
                 "(%.1f s total, %.2f s/frame)\n",
                 frame, sn.k, sn.d, sn.s, sn.p, sn.distinctCenters, sn.groups,
                 sn.unresolved, sn.captures, sn.escapes, sn.births, sn.deaths,
                 sec, sec / frame);
        }
      }
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double totalSec = std::chrono::duration<double>(t1 - t0).count();

    // Final report card over the whole run.
    unsigned presentAtEnd = 0, stable5 = 0, localizedLast = 0, popTargetLast = 0;
    fprintf(sumf, "ISLAND_CENSUS L=%u W=%u sieve=%d frames=%u\n", L, W_USED,
            sieve, frames);
    fprintf(sumf,
            "frame,chief,first,last,present,pop_last,pop_max,span_last,"
            "span_max,families_max,centers_last,max_stable\n");
    for (const auto& [chief, st] : stats)
    {
      fprintf(sumf, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
              frames, chief, st.firstFrame, st.lastFrame, st.framesPresent,
              st.populationLast, st.populationMax, st.spanLast, st.spanMax,
              st.familiesMax, st.centersLast, st.maxStable);
      if (st.lastFrame == frames) ++presentAtEnd;
      if (st.maxStable >= 5u) ++stable5;
      if (st.lastFrame == frames && st.spanLast <= RMAX && st.populationLast >= 2u) ++localizedLast;
      if (st.lastFrame == frames && st.populationLast == copiesPerFamily) ++popTargetLast;
    }

    fprintf(sumf,
            "\nSUMMARY frames=%u chiefs_total=%u chiefs_at_end=%u "
            "stable>=5frames=%u localized_at_end=%u pop==L/3_at_end=%u\n",
            frames, (unsigned)stats.size(), presentAtEnd, stable5,
            localizedLast, popTargetLast);
    fprintf(sumf, "wall_seconds=%.2f seconds_per_frame=%.3f\n",
            totalSec, totalSec / frames);

    printf("\nRESULT frames=%u chiefs_total=%u chiefs_at_end=%u "
           "stable>=5frames=%u localized_at_end=%u pop==L/3_at_end=%u "
           "wall=%.1fs (%.2f s/frame)\n",
           frames, (unsigned)stats.size(), presentAtEnd, stable5,
           localizedLast, popTargetLast, totalSec, totalSec / frames);
    printf("See %s/census.csv, groups.csv, summary.txt.\n", outdir);

    const auto report=attractor::summarize();
    const std::string base=std::string(outdir)+"/";
    if(!attractor::writeCSV(base+"constituents.csv",report) ||
       !attractor::writeSectorCSV(base+"sector_flux.csv") ||
       !attractor::writeCensusCSV(base+"chief_census.csv"))
      throw std::runtime_error("cannot write chief instrumentation");
    attractor::printReport(report);
    fclose(census); fclose(groups); fclose(sumf);
    puts("PASS census recorded; independent chief counts agree.");
    return 0;
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "FAIL: %s\n", e.what());
    return 2;
  }
}

