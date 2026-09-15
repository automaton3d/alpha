@echo off
rem build_probe_em_orphan.bat - alpha_probe with the EM reorder AND the orphan
rem mediation channel, so the electroweak decision is taken BEFORE the identity
rem merge (EM_FIRST_FSM) while the orphan-shell recruit/relay channel is live
rem (ORPHAN_GUIDANCE_FSM).  Added for the distinct-family ("far") quantization
rem hunt: with the orphan channel alone the identity merge still pre-empts the
rem EM branch (see PHOTON_MEDIATION_FAR.md).
rem POLAR_BOOTSTRAP_ADDRESS + POLAR_BROADCAST_WAVE are included so pB/sB can be
rem live (the EM guard needs them; use the [pol] argument to seed the axis).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d %~dp0..
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D EM_FIRST_FSM /D ORPHAN_GUIDANCE_FSM /I src\include /I src\include\zlib /I src ^
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
    /Fe:experiments\alpha_probe_em_orphan.exe
