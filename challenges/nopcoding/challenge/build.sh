#!/bin/bash -xe

HANDOUT_FILES=(
    "./build/challenge"
)

wget -v -O capstone.tar.gz https://github.com/capstone-engine/capstone/archive/4.0.2.tar.gz
tar xf capstone.tar.gz

cd capstone-4.0.2
./make.sh nix32
./make.sh install

cd ..

# Build binaries
gcc src/nopcoding.c -O0 -no-pie -o build/challenge -lcapstone -m32

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done

cp /usr/lib/libcapstone.so.4 build/
cp /usr/lib/libcapstone.so.4 handout
cp /usr/lib/i386-linux-gnu/libc.so.6 handout
cp /usr/lib/i386-linux-gnu/ld-linux.so.2 handout
cp src/nopcoding.c handout
