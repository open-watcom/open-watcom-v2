 # pmake : dos os2 x86

.extensions:
.extensions: .exe .obj .dbj .c

pmake.exe : pmake.dos pmake.obj pmkcmd.obj
        wlink op quiet,stub=$[@ sys os2 pmc name $^@ file {pmake pmkcmd}

pmake.dos : pmake.dbj pmkcmd.dbj
        wlink op quiet sys dos name $^@ file {pmake.dbj pmkcmd.dbj}

.c:..

.c.obj:
        wcc /oax/fpc/d1/mc/zq/bt=os2 /i.. $[@

.c.dbj:
        wcc /oax/fpc/d1/mc/zq/bt=dos /i.. /fo=.dbj $[@

clean: .SYMBOLIC .EXPLICIT
        @if exist *.?bj del *.?bj
        @if exist *.exe del *.exe
        @if exist *.dos del *.dos
        @if exist *.err del *.err
        @if exist *.sym del *.sym
