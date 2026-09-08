@echo off
rem Production-path EXCLUSION_FSM build: ordinary simulation() + the
rem Pauli-like identity hard core and frame-edge separation push (candidate,
rem macro-guarded in interaction.cpp).  Default builds are unaffected.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census mkdir build\island_census
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EXCLUSION_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census\ /Fe:build\island_census\island_census_exc.exe
exit /b %errorlevel%
