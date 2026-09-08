@echo off
rem Waits for the lightmatter_decouple_basic run to finish, then audits the
rem 32-frame run dir and writes a DONE marker. Launch detached with:
rem   cmd /c start "" /b experiments\finish_lightmatter.bat
setlocal
cd /d "%~dp0.."
:wait
tasklist | find /i "lightmatter_decouple_basic.exe" >nul
if not errorlevel 1 (
    timeout /t 5 /nobreak >nul
    goto wait
)
python experiments\analyze_lightmatter.py build\lightmatter_decouple\basic32_seed_1 > build\lightmatter_decouple\basic32_seed_1\analysis.txt 2>&1
echo done> build\lightmatter_decouple\basic32_seed_1\DONE
type build\lightmatter_decouple\basic32_seed_1\analysis.txt
