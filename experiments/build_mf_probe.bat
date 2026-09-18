@echo off
rem build_mf_probe.bat - builds the multifrequency mechanics probe
rem with MULTIFREQ_RAY_FSM enabled to test the ray detection mechanics.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
if not exist obj mkdir obj

set SRC=experiments\stack_absorb_probe.cpp src\model\attractor.cpp src\model\charges.cpp src\model\geometry.cpp src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp src\model\wavefront.cpp
set FLAGS=/nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src

echo [1/2] multifrequency ray FSM (MULTIFREQ_RAY_FSM ON) -> experiments\mf_probe_ray.exe
cl %FLAGS% /D MULTIFREQ_RAY_FSM %SRC% /Fo:"obj\\" /Fe:experiments\mf_probe_ray.exe
if errorlevel 1 exit /b 1

echo [2/2] combined FSM (MULTIFREQ_RAY_FSM + PAIR_STACK_ABSORB_FSM ON) -> experiments\mf_probe_combined.exe
cl %FLAGS% /D MULTIFREQ_RAY_FSM /D PAIR_STACK_ABSORB_FSM %SRC% /Fo:"obj\\" /Fe:experiments\mf_probe_combined.exe
if errorlevel 1 exit /b 1

echo both multifrequency probe variants built.