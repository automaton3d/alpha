@echo off
rem build_probe_boot.bat - alpha_probe experimental build:
rem   /D EM_FIRST_FSM : one-shot W-address axis bootstrap in elect()
rem   /D POLAR_BROADCAST_WAVE    : first-arrival distance wave in diffusePass()
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d %~dp0..
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D EM_FIRST_FSM /I src\include /I src\include\zlib /I src ^
    experiments\alpha_probe.cpp ^
    src\model\attractor.cpp ^
    src\model\charges.cpp ^
    src\model\geometry.cpp ^
    src\model\initSim.cpp ^
    src\model\interaction.cpp ^
    src\model\polarization.cpp ^
    src\model\simulation.cpp ^
    src\model\utils.cpp ^
    src\model\wavefront.cpp ^
    /Fe:experiments\alpha_probe_em.exe
