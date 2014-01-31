# makefile for test1.c - test that DLL threading
# starts as expected.

all: tdll32_1.dll texe32_1.exe texe32_2.exe

tdll32_1.dll : ../c/test1.c
        wcl386 -zq -wx -d2 -s -bt=os2 -bd -bm -fm -DQA_MAKE_DLL -l=os2v2_dll -fe=tdll32_1.dll ../c/test1.c
        wlib -q -o=tdll32_1.lib tdll32_1.dll

texe32_1.exe : ../c/test1.c tdll32_1.dll
        wcl386 -zq -wx -fm -bm -bt=os2 -DQA_MAKE_EXE -l=os2v2 -fe=texe32_1.exe ../c/test1.c tdll32_1.lib

texe32_2.exe : ../c/test2.c
        wcl386 -zq -wx -fm -bm -bt=os2 -l=os2v2 -fe=texe32_2.exe ../c/test2.c
