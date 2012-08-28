
mthread.exe : mthread.obj
    *wlink d all file mthread.obj SYS nt op q op nocase

mthread.obj : mthread.for
    wfc386 mthread.for -d1 -q -dep

clean : .SYMBOLIC
        @if exist *.exe del *.exe
        @if exist *.rex del *.rex
        @if exist *.dll del *.dll
        @if exist *.obj del *.obj
        @if exist *.map del *.map
