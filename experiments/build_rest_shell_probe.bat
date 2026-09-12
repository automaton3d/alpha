@echo off
rem Builds the probe-rest instrument (production CPU model sources, reference flags).
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src ^
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
    /Foobj\ /Fe:experiments\rest_shell_probe.exe
exit /b %errorlevel%
