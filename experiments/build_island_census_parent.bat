@echo off
rem PARENT_SELECTIVE_FSM -- the candidate of 19 Sep 2026: D x D stops promoting, so the
rem population cap of 2 disappears; same-island delegates cohere, delegates of DIFFERENT
rem islands repel by one step per light frame, and K x K keeps its existing demotion+merge.
rem The discriminator is the dynamical parent (islandChief), never the seed family.
rem
rem This is a NEW programme, not one of the retired L/3 candidate routes (the eleven
rem build_island_census_* scripts retired into attic/retired_experiments).  It is OFF in
rem the reference build; the reference source is unchanged with this macro undefined.
rem
rem Usage: build_island_census_parent.bat
rem        build\island_census_parent\island_census_parent.exe [frames] [sieve] [outdir] [EL] [mm_eps]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_parent mkdir build\island_census_parent
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PARENT_SELECTIVE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_parent\ /Fe:build\island_census_parent\island_census_parent.exe
exit /b %errorlevel%
