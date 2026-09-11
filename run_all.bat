@echo off
rem ============================================================================
rem run_all.bat - single reproducibility entry point for the alpha project.
rem
rem Scope: this script runs the *fast, deterministic* checks that validate the
rem reference (CPU, all candidate macros OFF) build and the headless probe.
rem The heavy campaigns (island census, inertia revalidation, gravity probes,
rem light-matter decoupling) are NOT run here by default; their commands and
rem expected outputs are listed in experiments\*.md and experiments\WORK_PLAN.md.
rem
rem Usage:   run_all.bat            (quick smoke: GUI build + probe smoke)
rem          set FULL=1 && run_all.bat   (also runs the 200-frame reference probe)
rem
rem All output is written under build\run_all\ ; a summary is printed at the end.
rem ============================================================================
setlocal
cd /d "%~dp0"
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 ( echo [run_all] vcvars64.bat not found or failed. & exit /b 1 )

rem NOTE: the ambient VCPKG_ROOT may point at the vcpkg bundled with Visual
rem Studio (which lacks freetype).  Force the project's vcpkg unless the caller
rem overrides it via ALPHA_VCPKG.
if not defined ALPHA_VCPKG set ALPHA_VCPKG=E:\vcpkg\installed\x64-windows
set VCPKG_ROOT=%ALPHA_VCPKG%
if not exist "obj" mkdir "obj"
if not exist "build" mkdir "build"
if not exist "build\run_all" mkdir "build\run_all"
set REPORT=build\run_all\report.txt
echo alpha run_all report > "%REPORT%"
call :stamp "start"

rem --- 1. GUI reference build (CPU) ------------------------------------------
echo [run_all] 1/3 building the GUI - nmake build\automaton.exe ...
nmake build\automaton.exe > build\run_all\01_gui_build.log 2>&1
if errorlevel 1 goto :gui_fail
call :stamp "gui_build OK"
goto :step2

:gui_fail
echo [run_all] GUI build FAILED - see build\run_all\01_gui_build.log
call :stamp "gui_build FAILED"
goto :end

rem --- 2. Headless probe: build + smoke run ---------------------------------
:step2
echo [run_all] 2/3 building and running alpha_probe - smoke test ...
cmd /c experiments\build_probe.bat > build\run_all\02_build_probe.log 2>&1
if not exist experiments\alpha_probe.exe goto :probe_fail
call :stamp "probe_build OK"
experiments\alpha_probe.exe 7 4 20 16384 256 > build\run_all\03_probe_smoke.log 2>&1
findstr /C:"alpha_A =" build\run_all\03_probe_smoke.log >nul
if errorlevel 1 goto :smoke_fail
call :stamp "probe_smoke OK"
goto :step3

:probe_fail
echo [run_all] probe build FAILED - see build\run_all\02_build_probe.log
call :stamp "probe_build FAILED"
goto :end

:smoke_fail
echo [run_all] probe smoke produced no alpha_A line.
call :stamp "probe_smoke NO alpha_A line"
goto :step3

rem --- 3. Reference 200-frame probe (optional, slow) -------------------------
:step3
if not defined FULL goto :skip_full
echo [run_all] 3/3 running the 200-frame reference probe ...
experiments\alpha_probe.exe 7 4 200 16384 256 > build\run_all\04_probe_reference.log 2>&1
call :stamp "probe_reference done"
goto :end

:skip_full
echo [run_all] 3/3 skipped -- set FULL=1 to run the 200-frame reference probe.
call :stamp "probe_reference skipped"

:end
echo.
echo ---- run_all summary (build\run_all\report.txt) ----
type "%REPORT%"
echo.
echo Reference configuration note: all candidate macros OFF.  Other harnesses
echo (island_census, inertia_revalidation, gravity_probe, lightmatter_decouple)
echo are separate builds - see experiments\WORK_PLAN.md.
endlocal
exit /b 0

:stamp
echo %~1 >> "%REPORT%"
exit /b 0
