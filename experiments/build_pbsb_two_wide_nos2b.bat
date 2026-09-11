@echo off
rem Route B: pbsb_two_wide with EM_FIRST_FSM + EM_NOS2B_FSM (no sieve gate).
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_wide\obj_nos2b mkdir build\pbsb_two_wide\obj_nos2b
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EM_FIRST_FSM /D EM_NOS2B_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two_wide.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_wide\obj_nos2b\ /Fe:build\pbsb_two_wide\pbsb_two_wide_nos2b.exe
exit /b %errorlevel%
