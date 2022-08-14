#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
)

# Build binaries
gcc src/challenge.c -Wall -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done

cp /usr/lib/x86_64-linux-gnu/libc.so.6 handout
cp /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 handout
