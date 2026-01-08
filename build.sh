#!/bin/bash

CC=clang
CFLAGS="-O3 -Wall -mavx2 -msse4.2"
SRC_DIR="src"
OUTPUT="gitFromAlibaba"

$CC $CFLAGS "$SRC_DIR"/*.c -o $OUTPUT

if [ $? -eq 0 ]; then
    ./$OUTPUT "$@"
else
    echo "Compilation failed."
    exit 1
fi