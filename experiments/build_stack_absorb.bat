@echo off
rem build_stack_absorb.bat - builds the PAIR_STACK_ABSORB_FSM regression probe
rem in BOTH modes: candidate (macro ON) and reference control (macro OFF).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
if not exist obj mkdir obj
set SRC=experiments\stack_absorb_probe.cpp src\model\attractor.cpp src\model\charges.cpp src\model\geometry.cpp src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp src\model\wavefront.cpp
set FLAGS=/nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src
echo [1/2] candidate (PAIR_STACK_ABSORB_FSM ON) -^> experiments\stack_absorb_probe.exe
cl %FLAGS% /D PAIR_STACK_ABSORB_FSM %SRC% /Fo"obj\\" /Fe:experiments\stack_absorb_probe.exe
if errorlevel 1 exit /b 1
echo [2/2] reference control (macro OFF) -^> experiments\stack_absorb_probe_ref.exe
cl %FLAGS% %SRC% /Fo"obj\\" /Fe:experiments\stack_absorb_probe_ref.exe
if errorlevel 1 exit /b 1
echo both probe variants built.
