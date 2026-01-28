#!/bin/bash

gcc src/haversine_main.c src/json_parser.c src/buffer.c -I src -o processor -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./processor "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

