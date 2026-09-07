@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\seed_asymmetry mkdir build\seed_asymmetry
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\seed_asymmetry.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\seed_asymmetry\ /Fe:build\seed_asymmetry\seed_asymmetry.exe
exit /b %errorlevel%
