#pmake: banal

compiler = watcom

all : banal.exe bcomp.exe
	
banal.exe : banal.obj
	wlink d a f banal op map

banal.obj : banal.c
	wcc386 -w4 -we -d2 banal.c

bcomp.exe : bcomp.obj
	wlink d a f bcomp op map

bcomp.obj : bcomp.c
	wcc386 -w4 -we -d2 bcomp.c
