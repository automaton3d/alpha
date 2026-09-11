@echo off
rem build_probe.bat - compiles the headless alpha_probe harness (e:\alpha only).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
if not exist obj mkdir obj
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src ^
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
    /Foobj\ /Fe:experiments\alpha_probe.exe
