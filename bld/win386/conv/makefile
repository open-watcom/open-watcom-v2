.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .def .c

.def : def

def_files = &
clib.def &
commdlg.def &
ddeml.def &
dpmi.def &
lzexpand.def &
mmsystem.def &
odbc.def &
ole.def &
penwin.def &
shell.def &
stress.def &
toolhelp.def &
ver.def &
win31.def &
win31_16.def &

all : conv.exe win386.lib .SYMBOLIC
        @%null

conv.exe : conv.obj makefile
#       wlink sys dos file conv
        wlink file conv

conv.obj : conv.c
        $(comp286) -ms-os-s conv.c -d1

win386.lib : $(def_files) win386 conv.exe fldenv.obj _call16.obj
        conv win386 -s -q
        wlib /n /b /p=16 win386.lib +w386lib.obj +fldenv.obj +_call16.obj
        @erase w386lib.obj
        @copy winglue.asm ..
        @copy dllthunk.c ..
        @copy dllthk.asm ..
        @copy winglue.inc ..\h
        @erase winglue.asm
        @erase dllthk.asm
        @erase dllthunk.c
        @erase winglue.inc

fldenv.obj : fldenv.asm
        wasm -i$(watcomdir)h fldenv
_call16.obj : _call16.asm
        wasm _call16
