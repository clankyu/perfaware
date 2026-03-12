#!/bin/bash

gcc src/virtual_address.c -I src -o addr

if [ $? -eq 0 ]; then
    echo "[DEBUG]: Compilation successful, running program."
    ./addr "$@"
else
    echo "[DEBUG]: Compilation failed."
fi

