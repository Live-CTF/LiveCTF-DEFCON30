#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "./build/pacman"
    "/usr/aarch64-linux-gnu/lib/libc.so.6"
    "/usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1"
)

# Build binaries
aarch64-linux-gnu-gcc src/challenge.c -march=armv8.3-a -no-pie -g -Wno-incompatible-pointer-types -o build/pacman
cp src/run build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp -L $f handout/
done
