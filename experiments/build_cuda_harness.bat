@echo off
rem build_cuda_harness.bat - headless CUDA parity harness.
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set CUDA=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.2
if not exist "%CUDA%\bin\nvcc.exe" set CUDA=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8
if not exist "E:\alpha\obj\ch" mkdir "E:\alpha\obj\ch"
set INC=-I"E:\alpha\src\include" -I"E:\alpha\src\include\zlib" -I"E:\alpha\src" -I"E:\alpha\src\include\zlib\cuda" -I"E:\alpha\src\cuda" -I"E:\vcpkg\installed\x64-windows\include"
"%CUDA%\bin\nvcc.exe" -allow-unsupported-compiler -c -std=c++17 -O1 -DNOMINMAX -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -DUSE_CUDA %INC% --compiler-options /MD,/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -o "E:\alpha\obj\ch_cuda_automaton.obj" "E:\alpha\src\cuda\cuda_automaton.cu" || exit /b 1
"%CUDA%\bin\nvcc.exe" -allow-unsupported-compiler -c -std=c++17 -O1 -DNOMINMAX -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -DUSE_CUDA %INC% --compiler-options /MD,/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -o "E:\alpha\obj\ch_cuda_constants.obj" "E:\alpha\src\cuda\cuda_constants.cu" || exit /b 1
"%CUDA%\bin\nvcc.exe" -allow-unsupported-compiler -c -std=c++17 -O1 -DNOMINMAX -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 %INC% --compiler-options /MD,/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH=1 -o "E:\alpha\obj\ch_harness.obj" "E:\alpha\experiments\cuda_harness.cu" || exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /I E:\alpha\src\include /I E:\alpha\src\include\zlib /I E:\alpha\src /FoE:\alpha\obj\ch\ /c E:\alpha\src\model\attractor.cpp E:\alpha\src\model\charges.cpp E:\alpha\src\model\geometry.cpp E:\alpha\src\model\initSim.cpp E:\alpha\src\model\interaction.cpp E:\alpha\src\model\polarization.cpp E:\alpha\src\model\simulation.cpp E:\alpha\src\model\utils.cpp E:\alpha\src\model\wavefront.cpp || exit /b 1
cl /nologo /std:c++20 /O2 /EHsc /MD /D NOMINMAX /Fe"E:\alpha\experiments\cuda_harness.exe" ^
    "E:\alpha\obj\ch_cuda_automaton.obj" "E:\alpha\obj\ch_cuda_constants.obj" "E:\alpha\obj\ch_harness.obj" ^
    "E:\alpha\obj\ch\attractor.obj" "E:\alpha\obj\ch\charges.obj" "E:\alpha\obj\ch\geometry.obj" ^
    "E:\alpha\obj\ch\initSim.obj" "E:\alpha\obj\ch\interaction.obj" "E:\alpha\obj\ch\polarization.obj" ^
    "E:\alpha\obj\ch\simulation.obj" "E:\alpha\obj\ch\utils.obj" "E:\alpha\obj\ch\wavefront.obj" ^
    /link /LIBPATH:"%CUDA%\lib\x64" cudart.lib || exit /b 1
echo HARNESS_BUILD_OK
