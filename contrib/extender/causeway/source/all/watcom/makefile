all:	..\current\watcom\read.me ..\current\watcom\cwsystem.lnk
all:	..\current\watcom\cwstub.exe cwdstub.exe
all:	..\current\watcom\cw.trp cwhelp.exe
all:	vesa.obj ..\current\watcom\cwhelp.cfg
all:	dllstrts.obj dllstrtr.obj dllfunc.obj dlltest.exe dlls.dll dllr.dll


#
# Update the read.me file
#
..\current\watcom\read.me: read.me
 copy read.me ..\current\watcom


#
# Update the linker script file
#
..\current\watcom\cwsystem.lnk: cwsystem.lnk
 copy cwsystem.lnk ..\current\watcom
 copy cwsystem.lnk d:\watcom\binb\cwsystem.lnk


#
# Update the EXE stub file
#
..\current\watcom\cwstub.exe: cwstub.exe
 copy cwstub.exe ..\current\watcom\cwstub.exe
 copy cwstub.exe d:\watcom\binb\cwstub.exe


#
# Update the DLL stub file
#
cwdstub.exe: cwdstub.asm
 ml cwdstub.asm
 copy cwdstub.exe ..\current\watcom\cwdstub.exe
 copy cwdstub.exe d:\watcom\binb\cwdstub.exe


#
# Update the trap file
#
..\current\watcom\cw.trp: cw.trp
 copy cw.trp ..\current\watcom
 copy cw.trp d:\watcom\bin

cw.trp:
 cd trap\lcl\dos\dosx\rsi\dos.trp
 wmake w
 cd..
 cd..
 cd..
 cd..
 cd..
 cd..
 copy trap\bin\rsi.trp cw.trp


#
# Update the trap file helper
#
cwhelp.exe:	cwhelp.asm
 ml -c /Fl cwhelp.asm
 cwl /nostub /flat /map cwhelp c2asm,cwhelp.cw,,system
 cwc /m2 /l255 cwhelp.cw
 copy /b cwdstub.exe + cwhelp.cw cwhelp.exe
 del cwhelp.cw
 copy cwhelp.exe ..\current\watcom
 copy cwhelp.exe d:\watcom\bin

#
# Update the trap file config file.
#
..\current\watcom\cwhelp.cfg: cwhelp.cfg
 copy cwhelp.cfg ..\current\watcom


#
#Update the VESA patch
#
vesa.obj: vesa.asm
 ml -c vesa.asm
 copy vesa.obj ..\current\watcom
 copy vesa.txt ..\current\watcom



dllstrts.obj : dllstart.asm
 wasm -bt=DOS -ms -3s_ dllstart.asm -fo=dllstrts.obj
 copy dllstrts.obj ..\current\watcom
 copy dllstrts.obj d:\watcom\lib386\dos

dllstrtr.obj : dllstart.asm
 wasm -bt=DOS -ms -3r_ dllstart.asm -fo=dllstrtr.obj
 copy dllstrtr.obj ..\current\watcom
 copy dllstrtr.obj d:\watcom\lib386\dos

cwapi.obj : d:\cw\cwapi.obj
 copy d:\cw\cwapi.obj
 copy cwapi.obj ..\current\watcom
 copy cwapi.obj d:\watcom\lib386\dos

dllfunc.obj : dllfunc.asm
 wasm -bt=DOS -ms -3s_ dllfunc.asm
 copy dllfunc.obj ..\current\watcom
 copy dllfunc.obj d:\watcom\lib386\dos



