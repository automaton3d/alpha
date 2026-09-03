@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d E:\alpha\experiments
cl /nologo /O2 /EHsc alphaC_limit.cpp /Fe:alphaC_limit.exe
alphaC_limit.exe > alphaC_table.txt
cl /nologo /O2 /EHsc alphaE_limit.cpp /Fe:alphaE_limit.exe
alphaE_limit.exe > alphaE_table.txt
