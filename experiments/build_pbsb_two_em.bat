@echo off
rem WP4.1 experiment: pbsb_two built WITH the EM_FIRST_FSM reordering.
rem Same two equal-charge S clouds (seeded polarisation axes) as pbsb_two,
rem but the encounter decides the EM (pB/sB) channel BEFORE the identity
rem merge.  Non-destructive variant; writes build\pbsb_two_em\.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_em mkdir build\pbsb_two_em
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EM_FIRST_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_em\ /Fe:build\pbsb_two_em\pbsb_two_em.exe
exit /b %errorlevel%
