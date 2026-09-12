@echo off
rem WP8 option (2) census, RIGID variant: family-selective channel plus
rem FAMILY_RIGID_FSM (a non-co-located family homes every copy on its chief, so
rem islands re-cohere rather than splitting).  Candidate macros only.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_famr mkdir build\island_census_famr
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D HOMB_PRODUCER_FSM /D HOMB_CONSUMER_TRANSPORT /D POLAR_MAGNITUDE_FSM /D FAMILY_SELECTIVE_FSM /D FAMILY_RIGID_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_famr\ /Fe:build\island_census_famr\island_census_famr.exe
exit /b %errorlevel%
