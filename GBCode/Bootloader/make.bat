REM  Generate Rom "ram" code

mkdir obj

..\..\third_party\gbdk\bin\lcc -Wm-ys -tempdir=obj -o bootloader.gb ./C/main.c ../ResetAfterRomChange/GB/romreset.c

python ..\..\Python\bin2c.py ./bootloader.gb bootloaderCode >./GB/bootloadercode.c

