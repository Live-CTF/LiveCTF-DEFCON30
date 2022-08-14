#!/bin/bash
# This file should contain the commands necessary to run the challenge
# For simple, single binary challenges, you just need to change BINARY_NAME

cd build
socat TCP-LISTEN:31337,reuseaddr,fork EXEC:./challenge,raw,pty,echo=0
