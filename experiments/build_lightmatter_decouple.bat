@echo off
rem Light-matter decoupling candidate harnesses (colour/matched FSM).
rem - basic : colour+matched+chief only (resting plateau 8 K / 235 D)
rem - dd    : basic + D x D promotion + K x K clash (coagulation macro set)
rem LIGHT_MATTER_FSM adds the provisional pB/sB decoupling stand-in.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\lightmatter_decouple mkdir build\lightmatter_decouple
if not exist build\lightmatter_decouple\obj_basic mkdir build\lightmatter_decouple\obj_basic
if not exist build\lightmatter_decouple\obj_dd mkdir build\lightmatter_decouple\obj_dd
if not exist build\lightmatter_decouple\obj_exc mkdir build\lightmatter_decouple\obj_exc
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM ^
 /D COLOR_MATCHED_FSM /D LIGHT_MATTER_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\lightmatter_decouple.cpp src\model\charges.cpp ^
 src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp ^
 src\model\simulation.cpp src\model\utils.cpp ^
 /Fobuild\lightmatter_decouple\obj_basic\ /Fe:build\lightmatter_decouple\lightmatter_decouple_basic.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM ^
 /D COLOR_MATCHED_FSM /D LIGHT_MATTER_FSM /D EXCLUSION_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\lightmatter_decouple.cpp src\model\charges.cpp ^
 src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp ^
 src\model\simulation.cpp src\model\utils.cpp ^
 /Fobuild\lightmatter_decouple\obj_exc\ /Fe:build\lightmatter_decouple\lightmatter_decouple_exc.exe
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D COLOR_ENCOUNTER_FSM /D COLOR_CHIEF_FSM /D COLOR_COHESION_FSM ^
 /D COLOR_MATCHED_FSM /D DD_PROMOTION_FSM /D KK_CLASH_FSM ^
 /D LIGHT_MATTER_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\lightmatter_decouple.cpp src\model\charges.cpp ^
 src\model\initSim.cpp src\model\interaction.cpp src\model\polarization.cpp ^
 src\model\simulation.cpp src\model\utils.cpp ^
 /Fobuild\lightmatter_decouple\obj_dd\ /Fe:build\lightmatter_decouple\lightmatter_decouple_dd.exe
exit /b %errorlevel%
