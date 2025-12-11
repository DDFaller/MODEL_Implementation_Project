@echo off
setlocal

echo Compiling poly_comparison...

make clean
make

if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

echo Compilation finished.

echo Running poly_comparison...
poly_comparison.exe

endlocal