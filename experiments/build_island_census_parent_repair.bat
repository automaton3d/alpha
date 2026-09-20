@echo off
rem PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR -- the COMPLETED
rem rule set of 19 Sep 2026.
rem   D x D: same island -> cohesion (T2 disabled); different islands -> one step apart.
rem   K x K: one chief demoted and the demoted chief's delegation absorbed by the survivor.
rem   Nominations: any source NAMED as a parent by a delegate but not a chief is promoted at the
rem     frame edge -- the deterministic replacement for the global repair that the T2 promotion
rem     cascade used to perform (measured: without it, 1 chief + 241 delegates naming a non-K).
rem Ablation partners: build_island_census_parent.bat (fsm only) and
rem build_island_census_parent_absorb.bat (fsm + absorb, no nomination repair).
rem
rem Usage: build_island_census_parent_repair.bat
rem        build\island_census_parent\island_census_parent_repair.exe [frames] [sieve] [outdir] [EL] [mm_eps]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_parent mkdir build\island_census_parent
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_parent\repair\ /Fe:build\island_census_parent\island_census_parent_repair.exe
exit /b %errorlevel%
