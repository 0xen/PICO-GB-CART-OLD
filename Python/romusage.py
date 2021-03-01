#!/usr/bin/env python3
import sys
from PIL import Image

def hexdump(b, w=32):
    print("\n".join(b[i:i + w].hex() for i in range(0, len(b), w)))

def render_to_texels(data, twidth=16):
    rowbytes = 16 * twidth
    numrows = -(-len(data) // rowbytes)
    texels = bytearray()
    for i in range(numrows):
        rowdata = data[i * rowbytes:i * rowbytes + rowbytes]
        if len(rowdata) < rowbytes:
            rowdata = rowdata + bytes(rowbytes - len(rowdata))
        plane0 = b''.join(rowdata[i::16] for i in range(0, 16, 2))
        plane1 = b''.join(rowdata[i::16] for i in range(1, 17, 2))
        for p0, p1 in zip(plane0, plane1):
            texels.extend(
                ((p0 >> x) & 1) | (((p1 >> x) & 1) << 1)
                for x in range(7, -1, -1)
            )
    im = Image.new('P', (8 * twidth, 8 * numrows))
    im.putdata(texels)
    im.putpalette(b'\xC0\xFF\x5F\x80\xBF\x5F\x40\x7F\x5F\x00\x3F\x5F')
    return im

def main(argv=None):
    argv = argv or sys.argv
    if len(argv) < 2:
        print("romusage.py: no filename; try romusage.py --help")
        sys.exit(1)
    infilename = argv[1]
    outfilename = argv[2] if len(argv) > 2 else None
    if infilename in ('--help', '-h'):
        print("usage: romusage.py ROMNAME [PNGNAME]")
        return
    with open(infilename, "rb") as infp:
        romdata = infp.read()
    twidth = 32
    tiles = render_to_texels(romdata, twidth)
    if outfilename:
        tiles.save(outfilename)
    else:
        tiles.show()

if __name__=='__main__':
    if 'idlelib' in sys.modules:
        main(['./romusage.py', '../gb240p.gb'])
    else:
        main()
