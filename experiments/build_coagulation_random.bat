@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\coagulation_random mkdir build\coagulation_random
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM /D COLOR_MATCHED_FSM /D COAGULATION_RANDOM_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\coagulation_random.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\coagulation_random\ /Fe:build\coagulation_random\coagulation_random.exe
exit /b %errorlevel%
