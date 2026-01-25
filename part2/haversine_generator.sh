#!/bin/bash

gcc src/haversine_generator_main.c src/haversine_formula.c src/haversine_generator.c -I src -o profiler -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./profiler "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

