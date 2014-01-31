# makefile for tstrtnt1.c - test that DLL threading
# starts as expected.

all: tstdll1.dll tstexe1.exe

tstdll1.dll : ../c/tstrtnt1.c
        wcl386 -zq -wx -i"$(%WATCOM)/h/nt" -d2 -s -bt=nt -bd -bm -fm -DQA_MAKE_DLL -l=nt_dll -fe=tstdll1.dll -"option offset=0x01000000" ../c/tstrtnt1.c
        wlib -q -o=tstdll1.lib tstdll1.dll

tstexe1.exe : ../c/tstrtnt1.c tstdll1.dll
        wcl386 -zq -i"$(%WATCOM)/h/nt" -fm -bm -bt=nt -DQA_MAKE_EXE -l=nt -fe=tstexe1.exe ../c/tstrtnt1.c tstdll1.lib

