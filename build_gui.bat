@echo off
rem build_gui.bat - builds the automaton GUI (CPU-only) from e:\alpha sources.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set VCPKG_ROOT=E:\vcpkg\installed\x64-windows
cd /d E:\alpha
if not exist "obj" mkdir obj
if not exist "build" mkdir build
nmake build\automaton.exe > build_gui.log 2>&1
echo EXITCODE=%ERRORLEVEL% >> build_gui.log
