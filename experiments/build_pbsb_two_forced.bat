@echo off
rem WP4.2 spike: pbsb_two with EM_FIRST_FSM AND EM_FORCE_PREREQ.
rem EM_FORCE_PREREQ forces the reorder's prerequisite (s2B+pB) at each contact,
rem isolating whether the EM-first reorder is mechanically correct from whether
rem the (dormant) polarization broadcast can supply the flags.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_forced mkdir build\pbsb_two_forced
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EM_FIRST_FSM /D EM_FORCE_PREREQ ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_forced\ /Fe:build\pbsb_two_forced\pbsb_two_forced.exe
exit /b %errorlevel%
