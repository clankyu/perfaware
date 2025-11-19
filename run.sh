#!/bin/bash

gcc sim86/main.c sim86/decoder.c sim86/memory.c -I sim86 -o main

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    # Execute the program
    ./main "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

