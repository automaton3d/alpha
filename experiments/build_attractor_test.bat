@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\attractor_test mkdir build\attractor_test
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib experiments\attractor_test.cpp src\model\attractor.cpp /Fobuild\attractor_test\ /Fe:build\attractor_test\attractor_test.exe
if errorlevel 1 exit /b 1
build\attractor_test\attractor_test.exe
exit /b %errorlevel%
