@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\inertia_matched mkdir build\inertia_matched
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM /D COLOR_MATCHED_FSM /D INERTIA_MATCHED_FSM /D DD_PROMOTION_FSM /D KK_CLASH_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\inertia_matched.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\inertia_matched\ /Fe:build\inertia_matched\inertia_matched.exe
exit /b %errorlevel%
