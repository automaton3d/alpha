@echo off
rem Directional island census: the production-path census harness plus the WP8
rem candidate channel -- open bootstrap (POLAR_BOOTSTRAP_ADDRESS,
rem POLAR_BROADCAST_WAVE), the ported directional producers
rem (HOMB_PRODUCER_FSM), the c[]->reloc[] consumer (HOMB_CONSUMER_TRANSPORT)
rem and liveness by magnitude (POLAR_MAGNITUDE_FSM).  With this channel the
rem 81-distinct-centres acceptance test can be scored by the same census as the
rem reference run.  Candidate macros only; NOT a measurement of the reference.
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_census_dir mkdir build\island_census_dir
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D POLAR_BOOTSTRAP_ADDRESS /D POLAR_BROADCAST_WAVE /D HOMB_PRODUCER_FSM /D HOMB_CONSUMER_TRANSPORT /D POLAR_MAGNITUDE_FSM ^
 /I src\include /I src\include\zlib /I src ^
 experiments\island_census.cpp src\model\attractor.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\island_census_dir\ /Fe:build\island_census_dir\island_census_dir.exe
exit /b %errorlevel%
