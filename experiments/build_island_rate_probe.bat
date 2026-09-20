@echo off
rem Builds the small-W rate-balance probe in its two arms:
rem   rb_ref.exe   SURFACE_ESCAPE_FSM                       (reference rules + the release rule)
rem   rb_cand.exe  + PARENT_SELECTIVE_FSM + PARENT_FUSION_ABSORB + PARENT_NOMINATION_REPAIR
rem so the A/B isolates the candidate's transitions from the release rule itself.
rem The tube allocates W = N + 2*PAIRS layers, so the escape timer (W_USED contact-free light
rem frames) expires in a few frames -- see experiments/PARENT_SELECTIVE.md for why the cubic
rem census cannot reach the release regime (~3.4 h per release at L=9).
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\rate_probe mkdir build\rate_probe
set SRC=experiments\island_rate_probe.cpp src\model\attractor.cpp src\model\charges.cpp ^
 src\model\geometry.cpp src\model\initSim.cpp src\model\interaction.cpp ^
 src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp src\model\wavefront.cpp
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /I src\include /I src\include\zlib /I src %SRC% ^
 /Fobuild\rate_probe\ref\ /Fe:build\rate_probe\rb_ref.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR ^
 /I src\include /I src\include\zlib /I src %SRC% ^
 /Fobuild\rate_probe\cand\ /Fe:build\rate_probe\rb_cand.exe
rem Third arm: the completed rule set plus the PARAMETER-FREE shell-release brake (iii): a
rem delegate farther from its chief than the contact range 2*RMAX is outside the shell cohesion can
rem hold and is released -- size-dependent loss on a TRANSITION, not on displacement (brake (i) was
rem falsified by the inertia_fixture `length<=1` step invariant).
if not exist build\rate_probe\shell mkdir build\rate_probe\shell
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D SURFACE_ESCAPE_FSM ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR ^
 /D PARENT_SHELL_RELEASE_FSM ^
 /I src\include /I src\include\zlib /I src %SRC% ^
 /Fobuild\rate_probe\shell\ /Fe:build\rate_probe\rb_shell.exe
if errorlevel 1 exit /b 1
rem Fourth arm: the completed rule set plus the shell brake and WITHOUT the contact-free escape rule, so
rem the shell release is the ONLY release channel.  This is the arm the shell-range discriminator needs:
rem with SURFACE_ESCAPE_FSM compiled in, a stray delegate also expires its contact-free timer and the two
rem channels cannot be told apart.
if not exist build\rate_probe\shellonly mkdir build\rate_probe\shellonly
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /D PARENT_SELECTIVE_FSM /D PARENT_FUSION_ABSORB /D PARENT_NOMINATION_REPAIR ^
 /D PARENT_SHELL_RELEASE_FSM ^
 /I src\include /I src\include\zlib /I src %SRC% ^
 /Fobuild\rate_probe\shellonly\ /Fe:build\rate_probe\rb_shellonly.exe
if errorlevel 1 exit /b 1
exit /b %errorlevel%
