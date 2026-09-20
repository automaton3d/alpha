@echo off
rem ============================================================================
rem run_all.bat - single reproducibility entry point for the alpha project.
rem
rem Scope: this script runs the *fast, deterministic* checks that validate the
rem reference (CPU, all candidate macros OFF) build and its source fingerprint.
rem The heavy campaigns (island census, inertia revalidation, gravity probes,
rem light-matter decoupling) are NOT run here by default; their commands and
rem expected outputs are listed in experiments\*.md and experiments\WORK_PLAN.md.
rem
rem Usage:   run_all.bat            (quick: GUI build + fingerprint check)
rem          set FULL=1 && run_all.bat   (also runs the reference island census)
rem
rem All output is written under build\run_all\ ; a summary is printed at the end.
rem ============================================================================
setlocal
cd /d "%~dp0"
rem --- Check for custom vcvars64.bat path if USE_CUDA=1 is set
if "%USE_CUDA%" == "1" (
    if "%VCVARSBAT_PATH%" == "" (
        set VCVARSBAT_PATH="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
        echo [run_all] Using default vcvars64.bat path: %VCVARSBAT_PATH%
    )
    call "%VCVARSBAT_PATH%" >nul
) else (
    call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
)
if errorlevel 1 (
    echo [run_all] vcvars64.bat not found or failed.
    exit /b 1
)
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

rem --- 0. ODR/link gate (fast shared-header check) ---------------------------
rem Catches non-inline definitions in shared headers (LNK2005/LNK1169) in
rem seconds - the isqrt regression of 2026-09-17 (commit 3cc70c4) would have
rem been caught here instead of after the full 45-object GUI build.
echo [run_all] 0/3 running the ODR/link gate - nmake check-odr ...
nmake check-odr > build\run_all\00_odr_gate.log 2>&1
if errorlevel 1 goto :odr_fail
call :stamp "odr_gate OK"
goto :step1

:odr_fail
echo [run_all] ODR gate FAILED - duplicate symbols in shared headers - see build\run_all\00_odr_gate.log
call :stamp "odr_gate FAILED"
goto :end

rem --- 1. GUI reference build (CPU) ------------------------------------------
:step1
echo [run_all] 1/3 building the GUI - nmake build\automaton.exe ...
nmake build\automaton.exe > build\run_all\01_gui_build.log 2>&1
if errorlevel 1 goto :gui_fail
call :stamp "gui_build OK"
goto :step2

:gui_fail
echo [run_all] GUI build FAILED - see build\run_all\01_gui_build.log
call :stamp "gui_build FAILED"
goto :end

rem --- 2. Reference invariant: model source fingerprint -----------------------
rem The alpha_probe build + smoke test that used to live here was RETIRED on
rem 2026-09-19: the search for the coupling constant was premature and the whole
rem campaign is parked in attic/alpha_campaign/.  The reference invariant it
rem asserted is covered by the source fingerprint, which is independent of any
rem harness (see doc/REFERENCE_CONFIG.md).
:step2
echo [run_all] 2/3 checking the model fingerprint ...
powershell -NoProfile -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1 > build\run_all\02_fingerprint.log 2>&1
rem The script's exit code is the check (0 = MATCH); it is used instead of a
rem findstr over the log, because a PowerShell redirect writes a Unicode log.
if errorlevel 1 goto :fp_fail
call :stamp "fingerprint OK"
goto :step3

:fp_fail
echo [run_all] model fingerprint MISMATCH - see build\run_all\02_fingerprint.log
call :stamp "fingerprint FAILED"
goto :end

rem --- 3. Reference measurement (optional, slow): the L=9 island census -------
rem This step ran the 200-frame coupling probe until 19 Sep 2026; that probe is
rem parked (attic/alpha_campaign/) and the reference measurement it performed is
rem now the production-path island census, whose expected row is frozen in
rem doc/REFERENCE_CONFIG.md (section "Frozen expected values").
:step3
if not defined FULL goto :skip_full
echo [run_all] 3/3 running the reference island census (L=9, 2 frames) ...
if not exist build\run_all\census mkdir build\run_all\census
cmd /c experiments\build_island_census.bat > build\run_all\05_build_census.log 2>&1
if errorlevel 1 goto :census_fail
build\island_census\island_census.exe 2 16384 build\run_all\census > build\run_all\06_census_reference.log 2>&1
call :stamp "census_reference done"
goto :end

:census_fail
echo [run_all] island census build FAILED - see build\run_all\05_build_census.log
call :stamp "census_build FAILED"
goto :end

:skip_full
echo [run_all] 3/3 skipped -- set FULL=1 to run the reference island census.
call :stamp "census_reference skipped"

:end
echo.
echo ---- run_all summary (build\run_all\report.txt) ----
type "%REPORT%"
echo.
echo Reference configuration note: all candidate macros OFF.  Other harnesses
echo (island_census, inertia_revalidation, island_rate_probe, spin probes)
echo are separate builds - see experiments\WORK_PLAN.md.
endlocal
exit /b 0

:stamp
echo %~1 >> "%REPORT%"
exit /b 0
