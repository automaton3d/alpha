@echo off
rem Prepared-seed census: CORE RULES ONLY (every candidate macro of the
rem advanced channel off) plus the placed seed that puts each of the 9*EL
rem islands at its own site.  Purpose: see the quantised islands (at rest, one
rem per location) and read their population histogram from the census.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_placed mkdir build\island_census_placed
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PLACED_FAMILY_SEED ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_placed\ /Fe:build\island_census_placed\island_census_placed.exe
exit /b %errorlevel%
