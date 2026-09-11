@echo off
rem WP3.3: build promotion_three twice - reference vs the intra-island promotion
rem fix (DD_INTRA_ISLAND_FIX).  Ordinary production simulation() path.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\promotion_three\obj_ref mkdir build\promotion_three\obj_ref
if not exist build\promotion_three\obj_fix mkdir build\promotion_three\obj_fix
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\promotion_three.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\promotion_three\obj_ref\ /Fe:build\promotion_three\promotion_three_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D DD_INTRA_ISLAND_FIX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\promotion_three.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\promotion_three\obj_fix\ /Fe:build\promotion_three\promotion_three_fix.exe
exit /b %errorlevel%
