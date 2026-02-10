#!/bin/bash

gcc src/repetition_test_build.c -I src -o repeater -lm

if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    ./repeater "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

