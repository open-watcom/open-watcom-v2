 ml -c /Fl cwhelp.asm >err.log
 set lib=c:\watcom\lib386;c:\watcom\lib386\dos;
 ..\cwl\cwl /nostub /flat /map cwhelp c2asm,cwhelp.cw,,system >>err.log
 type err.log
 pause
 copy cwhelp.cw cwhelp.002
 ..\cwc /m2 /l255 cwhelp.cw
 copy /b cwdstub.exe + cwhelp.cw cwhelp.exe
 copy cwhelp.exe c:\watcom\binw
