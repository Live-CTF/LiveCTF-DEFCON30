#!/bin/bash

set -e

HANDOUT_FILES=(
    "./src/challenge.c"
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
gcc src/challenge.c -Wall -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp -L $f handout/
done
