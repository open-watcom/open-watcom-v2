ml /DDEBUG1=1 /c /Focwdebug.obj cw.asm
d:\devel\watcom\wl32 /sy/m/f/ds/i cwdebug,cwdebug
d:\devel\watcom\exesplit cwdebug.exe
erase file1.dsk
erase cwdebug.cw
ren file2.dsk cwdebug.cw
rem ..\cwc\cwc /l243 cwdebug.cw
erase cwdebug.exe
copy /b ..\cw32.exe + cwdebug.cw cwdebug.exe
