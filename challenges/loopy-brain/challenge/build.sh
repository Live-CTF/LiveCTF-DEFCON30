#!/bin/bash

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
gcc src/challenge.c -Wall -Wl,-z,norelro -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done

cp /usr/lib/x86_64-linux-gnu/libc.so.6 handout
cp /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 handout
