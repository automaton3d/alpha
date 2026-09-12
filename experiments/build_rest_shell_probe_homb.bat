@echo off
rem Open-channel + directional-producer build: POLAR_BOOTSTRAP_ADDRESS breaks the
rem election fixed point (so pB/sB become live) and HOMB_PRODUCER_FSM ports the
rem CUDA homing producers into the CPU encounter.  Candidate macros only; NOT a
rem measurement of the reference dynamics.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D HOMB_PRODUCER_FSM /I src\include /I src\include\zlib /I src ^
    experiments\rest_shell_probe.cpp ^
    src\model\attractor.cpp ^
    src\model\charges.cpp ^
    src\model\geometry.cpp ^
    src\model\initSim.cpp ^
    src\model\interaction.cpp ^
    src\model\polarization.cpp ^
    src\model\simulation.cpp ^
    src\model\utils.cpp ^
    src\model\wavefront.cpp ^
    /Foobj\ /Fe:experiments\rest_shell_probe_homb.exe
exit /b %errorlevel%
