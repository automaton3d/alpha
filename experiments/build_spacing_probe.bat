@echo off
rem Item 1: is spacing emergent?  Three arms of the same probe --
rem   _ref   : no macro (no channel live)
rem   _pol   : polarization bootstrap live (POLAR_BOOTSTRAP_ADDRESS +
rem            POLAR_BROADCAST_WAVE + SPACING_POL, which calls seedAxis)
rem   _polem : the same plus EM_FIRST_FSM (electric decision before the identity
rem            merge, the documented ordering fix)
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\spacing_probe\obj_ref mkdir build\spacing_probe\obj_ref
if not exist build\spacing_probe\obj_pol mkdir build\spacing_probe\obj_pol
if not exist build\spacing_probe\obj_polem mkdir build\spacing_probe\obj_polem
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spacing_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spacing_probe\obj_ref\ /Fe:build\spacing_probe\spacing_probe_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D SPACING_POL ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spacing_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spacing_probe\obj_pol\ /Fe:build\spacing_probe\spacing_probe_pol.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D SPACING_POL /D EM_FIRST_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spacing_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spacing_probe\obj_polem\ /Fe:build\spacing_probe\spacing_probe_polem.exe
exit /b %errorlevel%
