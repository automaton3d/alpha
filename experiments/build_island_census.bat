@echo off
rem Builds the production-path island census harness (L=9 W=243 canonical
rem superposed seed, ordinary simulation(), per-frame chief census).
rem The reduced ISLAND_SEED_EXPERIMENT macro is intentionally NOT defined:
rem this run exercises the full encounter dynamics.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census mkdir build\island_census
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census\ /Fe:build\island_census\island_census.exe
exit /b %errorlevel%
