#!/bin/bash

gcc sim86/dissasembler.c sim86/decoder.c -I sim86 -o dissasembler

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./dissasembler "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

