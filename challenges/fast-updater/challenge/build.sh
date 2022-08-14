#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
)

# Build binaries
gcc src/challenge.c -O0 -fno-stack-protector -Wall -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
