@echo off
rem Builds production CPU sources once, then the probe and regression tests.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src ^
    experiments\inertia_probe.cpp ^
    src\model\attractor.cpp ^
    src\model\charges.cpp ^
    src\model\geometry.cpp ^
    src\model\initSim.cpp ^
    src\model\interaction.cpp ^
    src\model\polarization.cpp ^
    src\model\simulation.cpp ^
    src\model\utils.cpp ^
    src\model\wavefront.cpp ^
    /Foobj\ /Fe:experiments\inertia_probe.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src ^
    experiments\inertia_test.cpp obj\charges.obj obj\initSim.obj obj\interaction.obj ^
    obj\polarization.obj obj\simulation.obj obj\utils.obj /Foobj\ /Fe:experiments\inertia_test.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src ^
    experiments\seed_test.cpp obj\charges.obj obj\initSim.obj obj\interaction.obj ^
    obj\polarization.obj obj\simulation.obj obj\utils.obj /Foobj\ /Fe:experiments\seed_test.exe
exit /b %errorlevel%
