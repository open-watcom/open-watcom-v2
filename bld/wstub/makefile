all : .symbolic wstub.exe wstubq.exe
        @%null
wstub.exe : wstub.obj
        wlink file wstub option quiet sys dos op map
wstubq.exe : wstubq.obj
        wlink file wstubq option quiet sys dos op map
wstub.obj : wstub.c
        $(comp286) wstub /oaxs /zq
wstubq.obj : wstub.c
        $(comp286) wstub /oaxs /zq /dQUIET=1 /fo=wstubq

clean : .symbolic
        @if exist *.?bj del *.?bj
        @if exist *.exe del *.exe
        @if exist *.lst del *.lst
        @if exist *.err del *.err
        @if exist *.map del *.map
        @if exist *.dmp del *.dmp
        @if exist *.sym del *.sym
        @if exist *.tmp del *.tmp
