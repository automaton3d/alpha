@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_seed mkdir build\island_seed
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D ISLAND_SEED_EXPERIMENT ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_seed.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_seed\ /Fe:build\island_seed\island_seed.exe
exit /b %errorlevel%
