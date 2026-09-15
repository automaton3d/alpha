@echo off
rem Menu item 3, closed by measurement: the pair-channel probe.  Builds one
rem binary with PAIR_WORD_LOG so the words of every pair formation are printed
rem (also useful for a long canonical census run).
rem Run from the repository root, e.g.
rem   build\pair_channel\pair_channel_probe.exe 8 r3
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\pair_channel mkdir build\pair_channel
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /D PAIR_WORD_LOG ^
 /I src\include /I src\include\zlib /I src ^
 experiments\pair_channel_probe.cpp src\model\charges.cpp src\model\initSim.cpp ^
 src\model\interaction.cpp src\model\polarization.cpp src\model\simulation.cpp ^
 src\model\utils.cpp /Fobuild\pair_channel\ /Fe:build\pair_channel\pair_channel_probe.exe
exit /b %errorlevel%
