@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\island_identity mkdir build\island_identity
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib /I src experiments\island_identity_test.cpp /Fobuild\island_identity\ /Fe:build\island_identity\island_identity_test.exe
if errorlevel 1 exit /b 1
build\island_identity\island_identity_test.exe
exit /b %errorlevel%
