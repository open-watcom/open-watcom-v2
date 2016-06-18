cobjs = TestFile.obj ErrTable.obj c_main.obj
jobjs = TestFile.obj ErrTable.obj j_main.obj


#add your files here

.extensions:
.extensions: .exe .obj .cpp .c

.c: .
.cpp: .
.obj: .

.c.obj: .AUTODEPEND
        wcc386 $[@ -bt=nt -w4 -d2 -i=$(%WATCOM)\h

.cpp.obj: .AUTODEPEND
        wpp386 $[@ -bt=nt -w4 -d2 -i=$(%WATCOM)\h

c_errcov.exe : $(cobjs)
        wlink op map name $^@ debug all sys nt file { $(cobjs) }

j_errcov.exe : $(jobjs)
        wlink op map name $^@ debug all sys nt file { $(jobjs) }

c_main.obj : main.cpp
        wpp386 -dAPP_TYPE=1 main.cpp -fo=c_main.obj -bt=nt -w4 -d2 -i=$(%WATCOM)\h

j_main.obj : main.cpp
        wpp386 -dAPP_TYPE=2 main.cpp -fo=j_main.obj -bt=nt -w4 -d2 -i=$(%WATCOM)\h

clean :.SYMBOLIC
        del *.exe
        del *.obj
        del *.err
        del *.map
