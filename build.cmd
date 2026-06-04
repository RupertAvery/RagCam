@echo off
setlocal
pushd "%~dp0"

cmake -S . -B build\Debug
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed.
    popd
    exit /b %ERRORLEVEL%
)

cmake --build build\Debug --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed.
    popd
    exit /b %ERRORLEVEL%
)

echo Build complete.
popd
endlocal
