@echo off
rem WP8 option (2) probe, RIGID variant: the family-selective channel plus
rem FAMILY_RIGID_FSM -- a family whose copies are not co-located sends every copy
rem to its chief, so an island re-coheres instead of splitting (self-correcting,
rem not a freeze).  Candidate macros only.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D HOMB_PRODUCER_FSM /D HOMB_CONSUMER_TRANSPORT /D POLAR_MAGNITUDE_FSM /D FAMILY_SELECTIVE_FSM /D FAMILY_RIGID_FSM /I src\include /I src\include\zlib /I src ^
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
    /Foobjfamr\ /Fe:experiments\rest_shell_probe_famrigid.exe
exit /b %errorlevel%
