#!/bin/bash

gcc dissasembler.c -o dissasembler

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./dissasembler "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

