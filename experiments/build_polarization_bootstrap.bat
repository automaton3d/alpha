@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\polarization_bootstrap mkdir build\polarization_bootstrap
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D ISLAND_SEED_EXPERIMENT ^
 /I src\include /I src\include\zlib /I src ^
 experiments\polarization_bootstrap.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\polarization_bootstrap\ /Fe:build\polarization_bootstrap\polarization_bootstrap.exe
exit /b %errorlevel%
