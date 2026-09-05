// Regression checks for the standard Platonic seed and its distance propagation.
#include <cassert>
#include <cstdio>
#include <vector>
#include "model/simulation.h"
#include "config.h"

std::vector<unsigned int> voxels;
Config gConfig;
namespace automaton {
bool convol_delay = false, diffuse_delay = false, reloc_delay = false;
bool tryEnableCuda() { return false; }
void disableCuda() {}
bool isCudaEnabled() { return false; }
bool swap_lattices_gpu() { return false; }
}

using namespace automaton;

static void checkSeed(const std::vector<Cell>& grid)
{
    unsigned sources = 0;
    for (unsigned w = 0; w < W_USED; ++w)
    for (unsigned x = 0; x < ELX; ++x)
    for (unsigned y = 0; y < ELY; ++y)
    for (unsigned z = 0; z < ELZ; ++z)
    {
        const Cell& c = getCell(grid, x, y, z, w);
        const bool source = x == lcenters[w][0] &&
                            y == lcenters[w][1] && z == lcenters[w][2];
        sources += source;
        assert(c.r2 == (source ? 0u : INF_R2));
        assert(c.r == (source ? 0 : -1));
        assert(c.u == (source ? 2048 : 0) && c.v == 0);
        assert(c.kind == SourceKind::S && !c.is_core);
        assert(c.a == firstWOfIsland(islandOf(w)) && c.leader_w == c.a);
        assert(c.k == 0 && c.t == 0 && c.f == 0 && c.active == 0);
        assert(c.bstamp == 0 && c.pol_u == 0 && c.pol_v == 0);
        assert(!c.pB && !c.sB && !c.s2B && !c.gB);
        assert(c.parent == NO_PARENT && c.pair_idx == NO_PAIR && c.pair_count == 0);
        for (int axis = 0; axis < 3; ++axis)
            assert(c.m[axis] == 0 && c.reloc[axis] == 0 && c.g[axis] == 0);
    }
    assert(sources == W_USED);
}

static void checkPropagation()
{
    // A synchronous six-neighbour update reaches exactly one Manhattan
    // layer per tick; reached cells must have the Euclidean squared radius.
    for (unsigned tick = 1; tick <= 3 * CENTER; ++tick)
    {
        update_pulsating_wavefront();
        for (unsigned x = 0; x < EL; ++x)
        for (unsigned y = 0; y < EL; ++y)
        for (unsigned z = 0; z < EL; ++z)
        {
            const int dx = (int)x - (int)CENTER;
            const int dy = (int)y - (int)CENTER;
            const int dz = (int)z - (int)CENTER;
            const unsigned hops = (dx < 0 ? -dx : dx) +
                                  (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
            const Cell& c = getCell(lattice_curr, x, y, z, 0);
            if (hops > tick) { assert(c.r2 == INF_R2 && c.r == -1); }
            else {
                const unsigned d2 = dx*dx + dy*dy + dz*dz;
                assert(c.r2 == d2 && c.r == isqrt((int)d2));
            }
        }
    }
}

int main()
{
    // Includes the canonical topology with multiple copies per W-island.
    for (unsigned L : {5u, 9u})
    {
        calculateParameters(L, 3 * L * L);
        assert(tryAllocate(L, W_USED));
        initSimulation(0);
        replicate();
        checkSeed(lattice_curr);
        checkSeed(lattice_draft);
        checkSeed(lattice_partner);
        checkPropagation();
        for (Cell& c : lattice_curr) {
            c.kind = SourceKind::K;
            c.k = 17; c.gB = true; c.g[0] = 2;
        }
        initSimulation(0);
        replicate();
        checkSeed(lattice_curr);
        checkSeed(lattice_draft);
        checkSeed(lattice_partner);
    }
    puts("PASS: seed, replication, reinitialization and local distance propagation");
}
