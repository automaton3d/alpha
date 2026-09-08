@echo off
rem Minimal production-path pB/sB preemption check (two equal-charge S clouds
rem with seeded polarisation axes).  Ordinary simulation() path.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pbsb_two mkdir build\pbsb_two
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two\ /Fe:build\pbsb_two\pbsb_two.exe
exit /b %errorlevel%
