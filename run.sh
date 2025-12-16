#!/bin/bash

gcc sim86/main.c sim86/decoder.c sim86/memory.c sim86/execute.c sim86/instruction.c sim86/text.c -I sim86 -o dissasembler

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./dissasembler "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

