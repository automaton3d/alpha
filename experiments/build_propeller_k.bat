@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist "build\propeller_k" mkdir "build\propeller_k"
rem Separate objects: never reuse the ordinary GUI/inertia kernel objects.
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
  /I src\include /I src\include\zlib /I src ^
  experiments\propeller_k.cpp src\model\charges.cpp src\model\initSim.cpp ^
  src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
  src\model\utils.cpp /Fobuild\propeller_k\ /Fe:build\propeller_k\propeller_k.exe
exit /b %errorlevel%
