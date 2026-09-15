@echo off
rem WP3.3 full-scale confirmation build: production-path EXCLUSION_FSM (Pauli-like
rem identity hard core + frame-edge separation push) TOGETHER WITH the intra-family
rem election fix (DD_INTRA_ISLAND_FIX), so a 3-copy seed family elects exactly one
rem chief (1 K + 2 D) instead of the reference 2 K + 1 D.
rem Candidate macros only: the reference build and the transition rule are unaffected.
rem Run from the repository root, e.g.
rem   build\island_census\island_census_exc_fix.exe 12 16384 build/island_census/prod_exc_fix
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census mkdir build\island_census
if not exist build\island_census\obj_exc_fix mkdir build\island_census\obj_exc_fix
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EXCLUSION_FSM /D DD_INTRA_ISLAND_FIX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census\obj_exc_fix\ /Fe:build\island_census\island_census_exc_fix.exe
exit /b %errorlevel%
