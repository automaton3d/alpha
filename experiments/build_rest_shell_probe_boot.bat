@echo off
rem Open-channel build: probe-rest with the (dormant) polarization bootstrap enabled,
rem so that the axis election can install a non-zero m on pairs formed from the
rem vacuum.  Candidate macros only; NOT a measurement of the reference dynamics.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /I src\include /I src\include\zlib /I src ^
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
    /Foobj\ /Fe:experiments\rest_shell_probe_boot.exe
exit /b %errorlevel%
