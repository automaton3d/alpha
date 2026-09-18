@echo off
rem build_gui.bat - builds the automaton GUI (CPU-only) from e:\alpha sources.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set VCPKG_ROOT=E:\vcpkg\installed\x64-windows
cd /d E:\alpha
if not exist "obj" mkdir obj
if not exist "build" mkdir build

rem --- ODR/link gate: fails fast on non-inline definitions in shared headers.
rem History: isqrt lost its `inline` in model/simulation.h (commit 3cc70c4,
rem 2026-09-17) and the GUI could not link.  This gate catches that class of
rem regression in seconds instead of after a full 45-object build.
nmake check-odr > build_odr.log 2>&1
if errorlevel 1 goto :odr_fail

nmake build\automaton.exe > build_gui.log 2>&1
echo EXITCODE=%ERRORLEVEL% >> build_gui.log
goto :eof

:odr_fail
echo build_gui.bat: ODR gate FAILED - duplicate symbols in shared headers.
echo Full log: e:\alpha\build_odr.log
type build_odr.log
exit /b 1

