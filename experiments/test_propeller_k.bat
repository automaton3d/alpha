@echo off
setlocal
cd /d "%~dp0.."
call experiments\build_propeller_k.bat
if errorlevel 1 exit /b 1
call :check right same 8 0
if errorlevel 1 exit /b 1
call :check left same 8 0
if errorlevel 1 exit /b 1
call :check right foreign 8 0
if errorlevel 1 exit /b 1
call :check right same 0 0
if errorlevel 1 exit /b 1
call :check right same 3 1
if errorlevel 1 exit /b 1
call :check left same 5 2
if errorlevel 1 exit /b 1
call :check right same 8 3
if errorlevel 1 exit /b 1
call :check left same 1 4
if errorlevel 1 exit /b 1
call :check right same 2 5
if errorlevel 1 exit /b 1
echo PASS: both directions, all six common phases, coincident seed and foreign affinity.
exit /b 0

:check
build\propeller_k\propeller_k.exe build\propeller_k\pk_check_%1_%2_%3_%4.csv 120 %3 %1 %2 17 %4 > build\propeller_k\pk_check_%1_%2_%3_%4.log
exit /b %errorlevel%
