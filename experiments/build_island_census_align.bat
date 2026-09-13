@echo off
rem CONTROL for the formation test: the canonical superposed seed with the
rem model's own island election convention and the island-aligned W schedule,
rem but WITHOUT the address-supplies-the-place walk.  Expected: distinct_centers
rem stays 1 (nothing transports a source), which isolates the new rule's effect.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_align mkdir build\island_census_align
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D DD_INTRA_ISLAND_FIX /D ISLAND_ALIGNED_W_ROTATION ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_align\ /Fe:build\island_census_align\island_census_align.exe
exit /b %errorlevel%
