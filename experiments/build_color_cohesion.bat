@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\color_cohesion mkdir build\color_cohesion
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\color_fsm.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\color_cohesion\ /Fe:build\color_cohesion\color_fsm.exe
exit /b %errorlevel%
