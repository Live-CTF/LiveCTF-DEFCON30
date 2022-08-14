#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
)

# Build binaries
gcc -s src/challenge.c -O0 -Wall -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
