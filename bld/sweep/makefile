#
# For NT:   sweep.exe is Win32 console, sweepb.exe is bound 16-bit DOS/32-bit NT console
# For OS/2: sweep.exe is 32-bit OS/2 console, sweepb.exe is bound 16-bit DOS/32-bit PM console
#

all : .symbolic sweep.exe sweepb.exe
        @%null

sweep.exe : sweep.c
        wcl386 sweep /ox

sweepb.exe : sweep.c
        $(comp286) sweep /ox /fpc /d1 /ms /zq
        $(linker) op quiet sys dos name sweep.dos file sweep
!ifdef __NT__
        $(comp386) sweep /ox /fpc /d1 /zq
        $(linker) op quiet,stub=sweep.dos name sweepb sys nt file sweep
!else
        $(linker) op quiet,stub=sweep.dos name sweepb sys os2 pmc file sweep
!endif
        del sweep.dos

clean : .symbolic
        if exist *.obj del *.obj
        if exist *.exe del *.exe



