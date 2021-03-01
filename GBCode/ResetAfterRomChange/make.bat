REM  Generate Rom "ram" code
..\..\third_party\gbdk\bin\sdcc -mgbz80 --fsigned-char --max-allocs-per-node 50000 --no-std-crt0 -I ../../third_party/gbdk/include -I ../../third_party/gbdk/include/asm -c -o ./obj/ram.rel ./C/romreset.c
..\..\third_party\gbdk\bin\sdldgb -n -i -k ../../third_party/gbdk/lib/small/asxxxx/gbz80 -l gbz80.lib -k ../../third_party/gbdk/lib/small/asxxxx/gb -l gb.lib -b _RAMCODE=50176 -b _DATA=52480 -o ./obj/ram.ihx ./obj/ram.rel

"C:\Program Files\7-Zip\7z" e -o./obj/ ./obj/ram.ihx ram -y

python ..\..\Python\bin2c.py ./obj/ram RomResetCode >./GB/romreset.c
python ..\..\Python\obj2ofs.py ./obj/ram.rel _RAMCODE "RAM_CODE_ADDR 50176" >./GB/romreset.h

