ml /c cw.asm
\linker\wl32\wl32 /f/ds cw
\linker\wl32\exesplit cw.exe
erase file1.dsk
erase cw.cw
ren file2.dsk cw.cw
..\cwc\cwc /l243 cw.cw
erase cw.exe
copy /b ..\cw32.exe + cw.cw cw.exe
