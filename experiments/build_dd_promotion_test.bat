@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\dd_promotion mkdir build\dd_promotion
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I src\include /I src\include\zlib experiments\dd_promotion_test.cpp /Fobuild\dd_promotion\ /Fe:build\dd_promotion\dd_promotion_test.exe
if errorlevel 1 exit /b 1
build\dd_promotion\dd_promotion_test.exe
exit /b %errorlevel%
