@echo off
rem Emergence search (EMERGENCE_SEARCH.md): the production-path island census with the
rem PHASE_DISTINCT_FSM candidate (identity by charge word and breathing phase).
rem Canonical superposed seed, reference sieve; the candidate only changes the
rem membership transitions, so the reference binary is unaffected.
rem Run from the repository root, e.g.
rem   build\island_census\island_census_phase.exe 4 16384 build/island_census/phase EL
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census mkdir build\island_census
if not exist build\island_census\obj_phase mkdir build\island_census\obj_phase
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PHASE_DISTINCT_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census\obj_phase\ /Fe:build\island_census\island_census_phase.exe
exit /b %errorlevel%
