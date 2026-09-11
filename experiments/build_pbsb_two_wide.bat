@echo off
rem WP4.3: pbsb_two on a WIDE tube so reconstructPair(RMAX-2) is non-degenerate.
rem Ordinary production path (no EM_FIRST_FSM).
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_wide mkdir build\pbsb_two_wide
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two_wide.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_wide\ /Fe:build\pbsb_two_wide\pbsb_two_wide.exe
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_wide\obj_em mkdir build\pbsb_two_wide\obj_em
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EM_FIRST_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two_wide.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_wide\obj_em\ /Fe:build\pbsb_two_wide\pbsb_two_wide_em.exe
if errorlevel 1 exit /b 1
if not exist build\pbsb_two_wide\obj_nos2b mkdir build\pbsb_two_wide\obj_nos2b
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EM_FIRST_FSM /D EM_NOS2B_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pbsb_two_wide.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pbsb_two_wide\obj_nos2b\ /Fe:build\pbsb_two_wide\pbsb_two_wide_nos2b.exe
exit /b %errorlevel%
