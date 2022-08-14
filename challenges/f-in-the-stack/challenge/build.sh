#!/bin/bash

# Build binaries
gcc -masm=intel -z relro -z now -fPIE -pie -s /src/challenge.c -o build/challenge

# Copy required files to handout
HANDOUT_FILES=(
    "./build/challenge"
    "Dockerfile.run"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
