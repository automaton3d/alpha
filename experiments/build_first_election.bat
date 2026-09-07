@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\first_election mkdir build\first_election
cl /nologo /std:c++20 /O2 /EHsc /MD /I src\include experiments\first_election.cpp /Fobuild\first_election\ /Fe:build\first_election\first_election.exe
exit /b %errorlevel%
