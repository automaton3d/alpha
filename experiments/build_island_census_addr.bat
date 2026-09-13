@echo off
rem CANDIDATE: FORMATION from the canonical superposed seed -- the address
rem supplies the place (ADDRESS_TARGET_FSM).  Core rules plus the model's own
rem island election convention and the island-aligned W schedule.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_addr mkdir build\island_census_addr
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D DD_INTRA_ISLAND_FIX /D ISLAND_ALIGNED_W_ROTATION /D ADDRESS_TARGET_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_addr\ /Fe:build\island_census_addr\island_census_addr.exe
exit /b %errorlevel%
