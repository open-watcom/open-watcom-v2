#
# Make file for CauseWay DLL support example
#
all:	dlltest.exe dlls.dll dllr.dll

dlltest.exe : dlltest.c dllfunc.h
 wcc386 /s /4s /d2 dlltest.c
 wlink @dlltest

dlls.dll : dll.c
 wcc386 /s /4s /bd dll.c /fo=dlls.obj
 wlink @dlls.lnk

dllr.dll : dll.c
 wcc386 /s /4r /bd dll.c /fo=dllr.obj
 wlink @dllr.lnk



