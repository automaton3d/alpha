@echo off
rem Short island-census regression: 4 frames on the production path,
rem then the independent Python audit. Outputs in build\island_census\run4.
setlocal
cd /d "%~dp0.."
if not exist build\island_census mkdir build\island_census
if not exist build\island_census\run4 mkdir build\island_census\run4
build\island_census\island_census.exe 4 16384 build\island_census\run4
if errorlevel 1 exit /b %errorlevel%
python experiments\analyze_island_census.py build\island_census\run4
exit /b %errorlevel%
