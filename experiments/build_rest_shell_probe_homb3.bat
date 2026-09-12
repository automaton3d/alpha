@echo off
rem WP8 (i)+(iii): the full candidate channel -- open bootstrap, directional
rem producers, the c[]->reloc[] consumer, and liveness by MAGNITUDE
rem (POLAR_MAGNITUDE_FSM) so the sector no longer depends on the sign quadrant.
rem Candidate macros only; NOT a measurement of the reference dynamics.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D HOMB_PRODUCER_FSM /D HOMB_CONSUMER_TRANSPORT /D POLAR_MAGNITUDE_FSM /I src\include /I src\include\zlib /I src ^
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
    /Foobj3\ /Fe:experiments\rest_shell_probe_homb3.exe
exit /b %errorlevel%
