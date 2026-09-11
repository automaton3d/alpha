@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\inertia_revalidation\production mkdir build\inertia_revalidation\production
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D INERTIA_REVALIDATION /I src\include /I src\include\zlib /I src experiments\inertia_legacy_runner.cpp src\model\charges.cpp src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp /Fobuild\inertia_revalidation\production\ /Fe:build\inertia_revalidation\production\original.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D INERTIA_REVALIDATION /I src\include /I src\include\zlib /I src experiments\inertia_revalidation.cpp build\inertia_revalidation\production\charges.obj build\inertia_revalidation\production\initSim.obj build\inertia_revalidation\production\interaction.obj build\inertia_revalidation\production\polarization.obj build\inertia_revalidation\production\simulation.obj build\inertia_revalidation\production\utils.obj /Fobuild\inertia_revalidation\production\ /Fe:build\inertia_revalidation\production\diagnostic.exe
if errorlevel 1 exit /b 1
if not exist build\inertia_revalidation\matched mkdir build\inertia_revalidation\matched
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D INERTIA_REVALIDATION /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM /D COLOR_MATCHED_FSM /D INERTIA_MATCHED_FSM /D DD_PROMOTION_FSM /D KK_CLASH_FSM /I src\include /I src\include\zlib /I src experiments\inertia_matched.cpp src\model\charges.cpp src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp /Fobuild\inertia_revalidation\matched\ /Fe:build\inertia_revalidation\matched\original.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D INERTIA_REVALIDATION /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM /D COLOR_MATCHED_FSM /D INERTIA_MATCHED_FSM /D DD_PROMOTION_FSM /D KK_CLASH_FSM /I src\include /I src\include\zlib /I src experiments\inertia_revalidation.cpp build\inertia_revalidation\matched\charges.obj build\inertia_revalidation\matched\initSim.obj build\inertia_revalidation\matched\interaction.obj build\inertia_revalidation\matched\polarization.obj build\inertia_revalidation\matched\simulation.obj build\inertia_revalidation\matched\utils.obj /Fobuild\inertia_revalidation\matched\ /Fe:build\inertia_revalidation\matched\diagnostic.exe
if errorlevel 1 exit /b 1
exit /b 0


