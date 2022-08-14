#!/bin/bash

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)


# TODO: remove debug
#gcc -DDEBUG -z execstack -fno-stack-protector -O2 src/aes.c src/challenge.c -O0  -Wall -o build/challenge
cp /usr/lib/x86_64-linux-gnu/libgcc_s.so.1 libgcc_s.so
gcc -Isrc -no-pie -O2 src/aes.c src/challenge.c -O0 -o build/challenge
# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
