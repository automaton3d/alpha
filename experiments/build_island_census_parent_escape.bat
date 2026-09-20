@echo off
rem PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR + SURFACE_ESCAPE_FSM
rem -- the completed rule set with the release channel ON, so that RELEASE (surface escape) and
rem RECRUITMENT (T4: an S joins a chief/delegate) compete.  Without release the repaired build has
rem max_population = 2 and zero escapes: no turnover at all, so no rate balance can be measured.
rem With it, this is the first configuration of the programme with turnover AND a bounded structure,
rem i.e. the first on which quantization/quantize_stdlib.py can look for a real N* (b < 0).
rem
rem Usage: build_island_census_parent_escape.bat
rem        build\island_census_parent\island_census_parent_escape.exe [frames] [sieve] [outdir] [EL] [mm_eps]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_parent mkdir build\island_census_parent
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR /D SURFACE_ESCAPE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_parent\escape\ /Fe:build\island_census_parent\island_census_parent_escape.exe
exit /b %errorlevel%
