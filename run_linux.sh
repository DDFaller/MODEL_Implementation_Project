#!/usr/bin/env bash
set -euo pipefail

echo "Compiling poly_comparison..."

gcc -Wall -Wextra -std=c11 \
    main.c \
    src/naive.c \
    src/karatsuba.c \
    src/tom.c \
    src/utils/timer.c \
    src/utils/timer_log.c \
    src/utils/utils.c \
    -o poly_comparison \
    -lm

echo "Compilation finished."

echo "Running ./poly_comparison ..."
./poly_comparison
