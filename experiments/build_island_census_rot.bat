@echo off
rem CANDIDATE: island-aligned W rotation + placed seed + the model's own island
rem election.  The reference rotates the partner lattice by ONE slice per frame,
rem so at ISLAND_SIZE = 3 two of every three pairings are between DIFFERENT
rem islands and the election cascades on the W address.  This build rotates
rem cyclically WITHIN each ISLAND_SIZE block instead, so a slot's partner always
rem belongs to the same island (island = w / ISLAND_SIZE, the model's own
rem partition; chief = first layer of the block).
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_rot mkdir build\island_census_rot
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PLACED_FAMILY_SEED /D DD_INTRA_ISLAND_FIX /D ISLAND_ALIGNED_W_ROTATION ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_rot\ /Fe:build\island_census_rot\island_census_rot.exe
exit /b %errorlevel%
