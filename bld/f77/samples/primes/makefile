fincpath        = $(%WATCOM)\src\fortran\os2

.before
    set finclude=$(fincpath)

.extensions:
.extensions: .exe .obj .for .fi .h

objs = primes.obj

primes.exe : $(objs) primes.res $(__makefiles__)
    wlink op map, st=64k, symf=primes d a system os2v2_pm lib os2386 f primes
    rc primes.res

.for : $(%finclude)
.for.obj
    wfc386 $[* /nowarn /d1 /ox /explicit

clean: .symbolic
        @if exist *.exe del *.exe
        @if exist *.map del *.map
        @if exist *.sym del *.sym
        @if exist *.obj del *.obj
