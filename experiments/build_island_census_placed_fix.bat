@echo off
rem Prepared-seed census + the model's OWN island convention in the election:
rem DD_INTRA_ISLAND_FIX elects the first layer of each ISLAND_SIZE group as the
rem single chief (isIslandChief), so a 3-layer island becomes 1K + 2D instead of
rem cascading to 2K + 1D.  This is the quantisation test.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_placed_fix mkdir build\island_census_placed_fix
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PLACED_FAMILY_SEED /D DD_INTRA_ISLAND_FIX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_placed_fix\ /Fe:build\island_census_placed_fix\island_census_placed_fix.exe
exit /b %errorlevel%
