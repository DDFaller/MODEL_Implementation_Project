@echo off
setlocal

echo Compiling poly_comparison...

gcc -Wall -Wextra -std=c11 ^
    main.c ^
    src/naive.c ^
    src/karatsuba.c ^
    src/tom.c ^
    src/utils/timer.c ^
    src/utils/timer_log.c ^
    src/utils/utils.c ^
    -o poly_comparison ^
    -lm

if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

echo Compilation finished.

echo Running poly_comparison...
poly_comparison.exe

endlocal
