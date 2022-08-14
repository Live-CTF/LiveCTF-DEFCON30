import argparse
import struct
from pwn import *

parser = argparse.ArgumentParser()
parser.add_argument("address", default="127.0.0.1:8000", help="Address of challenge")


args = parser.parse_args()

HOST, PORT = args.address.split(':')

r = remote(HOST, int(PORT))

# LOL Sorry
# These teams are way better at heap than us and we're sure they'll solve it

# Narrator: they did indeed solve it
