INCLUDE = h
CC = wcc
OPTS = -ols -s

CFLAGS = $(OPTS) -d1

all: wbind.exe wbindnt.exe .symbolic

wbindnt.exe : wbind.c
    wcc386 wbind -ox -fpc -bt=NT -d1-hw
    wlink debug watcom all system nt name wbindnt file wbind option stack=12k,caseexact,map,symfile

wbind.exe : wbinddos.obj wbindos2.obj
   wlink debug watcom all sys dos name wbinddos.exe file wbinddos option stack=8k,caseexact,symfile
   wlink debug watcom all sys os2 pmc name wbind file wbindos2 opt stub=wbinddos.exe,stack=12k,caseexact,heap=8k,newfile,map,symfile

wbinddos.obj : wbind.c
        $(CC) $[* $(CFLAGS) -bt=dos -ms -fpc -w4 -fo=wbinddos.obj -d_DOS -d1-hw

wbindos2.obj : wbind.c
        $(CC) $[* $(CFLAGS) -bt=os2 -mc -fpc -w4 -fo=wbindos2.obj -d1-hw
