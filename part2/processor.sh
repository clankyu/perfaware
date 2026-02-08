#!/bin/bash

gcc src/processor_build.c -I src -o processor -lm

if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    ./processor "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

