#!/bin/bash

gcc src/os_timer_main.c src/os_performance_metrics.c -I src/ -o freq

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./freq "$@"
else
    echo "[DEBUG]: Compilation failed."
fi
