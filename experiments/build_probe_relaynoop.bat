@echo off
rem build_probe_orphan.bat - alpha_probe with ORPHAN_GUIDANCE_FSM (field experiment)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d %~dp0..
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D ORPHAN_GUIDANCE_FSM /D ORPHAN_RELAY_NOOP /I src\include /I src\include\zlib /I src ^
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
    /Fe:experiments\alpha_probe_relaynoop.exe
