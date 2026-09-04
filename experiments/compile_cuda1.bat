@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set CUDA=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.2
if not exist "%CUDA%\bin\nvcc.exe" set CUDA=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8
"%CUDA%\bin\nvcc.exe" -allow-unsupported-compiler -c -std=c++17 -O1 -DNOMINMAX -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -DUSE_CUDA -I"E:\alpha\src\include" -I"E:\alpha\src\include\zlib" -I"E:\alpha\src" -I"E:\alpha\src\include\zlib\cuda" -I"E:\alpha\src\cuda" -I"E:\vcpkg\installed\x64-windows\include" --compiler-options /MD,/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -o "E:\alpha\obj\cuda_automaton_test.obj" "E:\alpha\src\cuda\cuda_automaton.cu" > "E:\alpha\experiments\cuda_compile1.log" 2>&1
echo EXITCODE=%errorlevel% >> "E:\alpha\experiments\cuda_compile1.log"
