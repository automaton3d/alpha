@echo off
rem winding_observatory -- B0 of the topological programme: measure the winding triple
rem of a prepared island's centre of mass around the three non-contractible cycles of T^3.
rem READ-ONLY: no rule, no planted global.  Reference path only (all candidate macros OFF).
rem
rem Usage: build_winding_observatory.bat
rem        build\winding_observatory\winding_observatory.exe <arm> [frames] [csv]
rem        arms: baseline | x+ x- | y+ y- | z+ z-
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist build\winding_observatory mkdir build\winding_observatory
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX ^
 /I src\include /I src\include\zlib /I src ^
 experiments\winding_observatory.cpp src\model\attractor.cpp src\model\charges.cpp ^
 src\model\geometry.cpp src\model\initSim.cpp src\model\interaction.cpp ^
 src\model\polarization.cpp src\model\simulation.cpp src\model\utils.cpp ^
 src\model\wavefront.cpp ^
 /Fobuild\winding_observatory\ /Fe:build\winding_observatory\winding_observatory.exe
exit /b %errorlevel%
