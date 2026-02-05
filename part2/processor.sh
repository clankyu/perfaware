#!/bin/bash

gcc src/haversine_main.c src/json_parser.c src/buffer.c src/haversine_generator.c src/haversine_formula.c src/os_performance_metrics.c src/profiler.c -I src -o processor -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./processor "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

