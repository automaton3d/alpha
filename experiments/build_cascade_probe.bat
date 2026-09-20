@echo off
rem Builds the A2 cascade probe: the canonical superposed seed with `CASCADE_LOG` defined, so every
rem encounter that changes a source's role is recorded (tick, addresses, kinds, charge word, parent).
rem The macro adds no rule and writes nothing to the lattice -- it only fills a ledger -- but it is a
rem PROBE build: the reference (all candidate macros OFF) is unaffected.
rem
rem Usage: build_cascade_probe.bat
rem        build\cascade_probe\cascade_probe.exe [frames] [sieve] [EL]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\cascade_probe mkdir build\cascade_probe
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D CASCADE_LOG ^
 /I src\include /I src\include\zlib /I src ^
 experiments\cascade_probe.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\cascade_probe\ /Fe:build\cascade_probe\cascade_probe.exe
exit /b %errorlevel%
