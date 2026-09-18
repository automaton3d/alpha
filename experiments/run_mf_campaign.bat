@echo off
rem ============================================================================
rem run_mf_campaign.bat - Multifrequency Mechanics Campaign
rem Runs all three probe variants to validate the multifrequency mechanics:
rem   1. PAIR_STACK_ABSORB_FSM ON     (stack absorption)
rem   2. MULTIFREQ_RAY_FSM ON        (ray detection mechanics)
rem   3. Both macros ON              (combined multifrequency mechanics)
rem   4. Both macros OFF             (reference control)
rem ============================================================================
cd /d E:\alpha
if not exist build\run_all mkdir build\run_all
set REPORT=build\run_all\mf_campaign_report.txt
echo Multifrequency Mechanics Campaign Report > "%REPORT%"
echo Generated: %DATE% %TIME% >> "%REPORT%"
echo. >> "%REPORT%"

echo [MF CAMPAIGN] Building and running multifrequency probes...
echo [MF CAMPAIGN] Report: %REPORT%
echo.

rem --- Probe 1: PAIR_STACK_ABSORB_FSM only ---
echo [1/4] PAIR_STACK_ABSORB_FSM (stack absorption) ...
experiments\stack_absorb_probe.exe 7 6 1 > build\run_all\mf_probe_01_absorb.log 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [1/4] PASSED >> "%REPORT%"
) else (
    echo [1/4] FAILED >> "%REPORT%"
)
type build\run_all\mf_probe_01_absorb.log | findstr /C:"assertions done"

rem --- Probe 2: Reference control (no macros) ---
echo [2/4] Reference control (no macros) ...
experiments\stack_absorb_probe_ref.exe 7 6 1 > build\run_all\mf_probe_02_reference.log 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [2/4] PASSED >> "%REPORT%"
) else (
    echo [2/4] FAILED >> "%REPORT%"
)
type build\run_all\mf_probe_02_reference.log | findstr /C:"assertions done"

rem --- Probe 3: MULTIFREQ_RAY_FSM only ---
echo [3/4] MULTIFREQ_RAY_FSM (ray detection) ...
experiments\mf_probe_ray.exe 7 6 1 > build\run_all\mf_probe_03_ray.log 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [3/4] PASSED >> "%REPORT%"
) else (
    echo [3/4] FAILED >> "%REPORT%"
)
type build\run_all\mf_probe_03_ray.log | findstr /C:"assertions done"

rem --- Probe 4: Both macros combined ---
echo [4/4] Combined MULTIFREQ_RAY_FSM + PAIR_STACK_ABSORB_FSM ...
experiments\mf_probe_combined.exe 7 6 1 > build\run_all\mf_probe_04_combined.log 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [4/4] PASSED >> "%REPORT%"
) else (
    echo [4/4] FAILED >> "%REPORT%"
)
type build\run_all\mf_probe_04_combined.log | findstr /C:"assertions done"

echo.
echo ---- MF Campaign Summary ----
type "%REPORT%"
echo.