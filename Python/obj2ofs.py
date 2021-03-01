#!/usr/bin/env python3

import sys

def obj2ofs(filename, section, define):
    if len(define) > 0:
        print('#define {:s}'.format(define))

    detected = False
    with open(filename) as f:
        line = f.readline()
        while line:
            decoded_line = [x.strip() for x in line.split(' ')]
            if decoded_line[0] == 'A':
                detected = (decoded_line[1] == section)
            if (detected and decoded_line[0] == 'S'):
                print('#define {:s}_ofs 0x{:s}u'.format(decoded_line[1], decoded_line[2][5:]))
            line = f.readline()
    return

if __name__=='__main__':
    obj2ofs(sys.argv[1], sys.argv[2], sys.argv[3])
