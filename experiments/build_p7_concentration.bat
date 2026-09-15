@echo off
rem WP-P7: build p7_concentration twice - reference vs the candidate intra-family
rem promotion fix (DD_INTRA_ISLAND_FIX).  Ordinary production simulation() path.
rem   ref: a prepared co-located 1K+2D island must split through T2 (2K+1D)
rem   fix: T2 is disabled inside the family, so the island keeps 1K+2D
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\p7_concentration\obj_ref mkdir build\p7_concentration\obj_ref
if not exist build\p7_concentration\obj_fix mkdir build\p7_concentration\obj_fix
if not exist build\p7_concentration\obj_escape mkdir build\p7_concentration\obj_escape
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\p7_concentration.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\p7_concentration\obj_ref\ /Fe:build\p7_concentration\p7_concentration_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D DD_INTRA_ISLAND_FIX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\p7_concentration.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\p7_concentration\obj_fix\ /Fe:build\p7_concentration\p7_concentration_fix.exe
if errorlevel 1 exit /b 1
rem Candidate of item 4: escape by surface (a delegate with no internal contact
rem for one full partner rotation is released).  Prediction in PREDICTED_NS_RULE.md.
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\p7_concentration.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\p7_concentration\obj_escape\ /Fe:build\p7_concentration\p7_concentration_escape.exe
exit /b %errorlevel%
