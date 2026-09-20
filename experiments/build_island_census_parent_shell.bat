@echo off
rem PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR + PARENT_SHELL_RELEASE_FSM.
rem The completed rule set of 19 Sep 2026 plus brake (iii) in its PARAMETER-FREE form: a delegate farther
rem from its chief than the model's own contact range 2*RMAX is outside the shell cohesion can claim and
rem is released.  Size-dependent loss on a TRANSITION, because the displacement route is closed
rem (inertia_fixture.h:95 asserts length<=1: one cell per constituent per light frame).
rem Ablation partner: build_island_census_parent_repair.bat (same rules, no brake).
rem
rem Usage: build_island_census_parent_shell.bat
rem        build\island_census_parent\island_census_parent_shell.exe [frames] [sieve] [outdir] [EL]
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_parent mkdir build\island_census_parent
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR ^
 /D PARENT_SHELL_RELEASE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_parent\shell_arm\ /Fe:build\island_census_parent\island_census_parent_shell.exe
exit /b %errorlevel%
