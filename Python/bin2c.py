#!/usr/bin/env python3

import sys

def bin2c(filename, arrayName):
    with open(filename, "rb") as f:
        data = f.read()
        
        sys.stdout.write("const unsigned int {:s}Size = {:d};\n".format(arrayName, f.tell()))

        sys.stdout.write("const unsigned char {:s}[] = {{\n".format(arrayName))
        j = 0
        for i in data:
            sys.stdout.write("0x{:02x},".format(i))
            if j == 16:
                sys.stdout.write("\n")
                j = 0
            else: 
                j = j + 1
        if i != 0:
            sys.stdout.write("\n")    
        sys.stdout.write("};\n")


    return

if __name__=='__main__':
    bin2c(sys.argv[1], sys.argv[2])
