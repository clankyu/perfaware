#!/bin/bash

gcc src/haversine_generator_main.c src/haversine_formula.c src/haversine_generator.c -I src -o generator -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./generator "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

