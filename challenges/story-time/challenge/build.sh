#!/bin/bash

[ ! -d ./handout ] && mkdir handout
rm -rf build && mkdir build

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

FLAGS="-O0 -Wl,--section-start=.plt=0x03133700 -no-pie"

# Build binaries
gcc -o build/challenge $FLAGS src/challenge.c

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp -L -r $f handout/
done
