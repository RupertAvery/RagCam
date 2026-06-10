@echo off
setlocal
pushd "%~dp0"

cmake -S . -B build\%1
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed.
    popd
    exit /b %ERRORLEVEL%
)

cmake --build build\%1 --config %1
if %ERRORLEVEL% neq 0 (
    echo Build failed.
    popd
    exit /b %ERRORLEVEL%
)

echo Build complete.
popd
endlocal
