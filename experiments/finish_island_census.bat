@echo off
rem Waits for the island_census run to finish, then runs the independent
rem audit and writes a DONE marker.  Launch detached with:
rem   cmd /c start "" /b experiments\finish_island_census.bat
setlocal
cd /d "%~dp0.."
:wait
tasklist | find /i "island_census.exe" >nul
if not errorlevel 1 (
    timeout /t 5 /nobreak >nul
    goto wait
)
python experiments\analyze_island_census.py build\island_census\run64 > build\island_census\run64\analysis.txt 2>&1
echo done> build\island_census\run64\DONE
type build\island_census\run64\analysis.txt
