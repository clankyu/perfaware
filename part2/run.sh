#!/bin/bash

gcc src/main.c -I src -o profiler -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./dissasembler "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

