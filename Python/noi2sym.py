#!/usr/bin/env python3

import sys

def noi2sym(filename):
    symbols = {}
    with open(filename) as f:
        line = f.readline()
        while line:
            decoded_line = [x.strip() for x in line.split(' ')]
            if decoded_line[0] == 'DEF':
                addr = int(decoded_line[2], 16)
                print('{:02x}:{:04x} {}'.format(addr >> 16, addr & 0xFFFF, decoded_line[1]))
            line = f.readline()
    return symbols

if __name__=='__main__':
    noi2sym(sys.argv[1])
