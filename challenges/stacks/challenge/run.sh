#!/bin/bash
# This file should contain the commands necessary to run the challenge
# For simple, single binary challenges, you just need to change BINARY_NAME

BINARY_NAME="./build/stacks"

socat TCP-LISTEN:31337,reuseaddr,fork EXEC:$BINARY_NAME,raw,pty,echo=0⏎
