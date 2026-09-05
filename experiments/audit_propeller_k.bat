@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0.."
call experiments\build_propeller_k.bat
if errorlevel 1 exit /b 1
set failures=0
> build\propeller_k\audit_phases.csv echo phase_p,phase_k,exit_code
for %%P in (0 1 2 3 4 5) do (
  for %%K in (0 1 2 3 4 5) do (
    build\propeller_k\propeller_k.exe build\propeller_k\audit_phase_%%P_%%K.csv 12 0 right same 17 %%P %%K > build\propeller_k\audit_phase_%%P_%%K.log 2>&1
    set result=!errorlevel!
    >> build\propeller_k\audit_phases.csv echo %%P,%%K,!result!
    if not !result! equ 0 set /a failures+=1 >nul
  )
)
echo Failed acceptance cases: !failures!/36. See build\propeller_k\audit_phases.csv
if !failures! gtr 0 exit /b 2
exit /b 0
