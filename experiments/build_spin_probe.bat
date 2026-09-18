@echo off
rem J1 of the spin programme (experiments/J1_SPIN.md): does total circulation J
rem protect a group against the existing surface-escape threat?
rem   ref : SURFACE_ESCAPE_FSM alone                  (the threat)
rem   gate: SURFACE_ESCAPE_FSM + SPIN_GATED_FSM       (threat + S1/S3 veto)
rem Ordinary production simulation() path.  Each arm is run twice, `j0` (m = 0,
rem J = 0) and `jnz` (opposite transverse m, J = (0,0,-5), sum(m) = 0).
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\spin_probe\obj_ref mkdir build\spin_probe\obj_ref
if not exist build\spin_probe\obj_gate mkdir build\spin_probe\obj_gate
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spin_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spin_probe\obj_ref\ /Fe:build\spin_probe\spin_probe_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM /D SPIN_GATED_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spin_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spin_probe\obj_gate\ /Fe:build\spin_probe\spin_probe_gate.exe
if errorlevel 1 exit /b 1
rem J2 (experiments/J1_SPIN.md): does J SELECT a size, or only protect whatever
rem size is present?  Rings of N members, coherent vs cancelling momenta.
if not exist build\spin_probe\obj_ring_ref mkdir build\spin_probe\obj_ring_ref
if not exist build\spin_probe\obj_ring_gate mkdir build\spin_probe\obj_ring_gate
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spin_ring_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spin_probe\obj_ring_ref\ /Fe:build\spin_probe\spin_ring_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM /D SPIN_GATED_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\spin_ring_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\spin_probe\obj_ring_gate\ /Fe:build\spin_probe\spin_ring_gate.exe
exit /b %errorlevel%