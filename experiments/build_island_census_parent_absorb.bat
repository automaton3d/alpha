@echo off
rem PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB -- the COMPLETED rule set of 19 Sep 2026.
rem   * D x D: same island -> cohesion (T2 disabled); different islands -> one step apart.
rem   * K x K: one chief demoted AND the demoted chief's delegation ABSORBED by the survivor
rem     (re-pointed at the frame edge, iterated to a fixed point), so a fusion grows an island
rem     instead of orphaning a delegation.
rem The ablation partner is build_island_census_parent.bat (same fsm, no absorb), which
rem collapsed to 1 chief + 242 delegates with 241 unresolved: this build tests whether the
rem absorption restores the structure and lets populations exceed the old cap of 2.
rem
rem Usage: build_island_census_parent_absorb.bat
rem        build\island_census_parent\island_census_parent_absorb.exe [frames] [sieve] [outdir] [EL] [mm_eps]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_parent mkdir build\island_census_parent
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_parent\absorb\ /Fe:build\island_census_parent\island_census_parent_absorb.exe
exit /b %errorlevel%
