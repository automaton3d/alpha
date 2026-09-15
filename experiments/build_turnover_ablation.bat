@echo off
rem Item 2 ("what to try next"): the EXCLUSION on/off ablation on a prepared
rem configuration with TURNOVER, so the flux harness (quantization/) has data it
rem can fit.  Two binaries: reference and /D EXCLUSION_FSM.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\turnover_ablation\obj_ref mkdir build\turnover_ablation\obj_ref
if not exist build\turnover_ablation\obj_exc mkdir build\turnover_ablation\obj_exc
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\turnover_ablation.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\turnover_ablation\obj_ref\ /Fe:build\turnover_ablation\turnover_ablation_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D EXCLUSION_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\turnover_ablation.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\turnover_ablation\obj_exc\ /Fe:build\turnover_ablation\turnover_ablation_exc.exe
exit /b %errorlevel%
