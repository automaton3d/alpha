@echo off
setlocal
cd /d "%~dp0.."
if not exist build\inertia_matched\inertia_matched.exe exit /b 1
for %%C in (rest right left island island_left foreign broken zero many) do (
  build\inertia_matched\inertia_matched.exe %%C 96 > build\inertia_matched\%%C.log 2>&1
  if errorlevel 1 exit /b 1
)
python experiments\analyze_inertia_matched.py
exit /b %errorlevel%
