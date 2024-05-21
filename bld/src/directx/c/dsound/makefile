dsound.exe: dsound.c .AUTODEPEND
    wcl386 -bt=win95 -l=win95 -zw -5r -oxt -"libpath $(%lib)" dsound.c dsound.lib dxguid.lib

clean: .SYMBOLIC
    @if exist *.err del *.err
    @if exist *.obj del *.obj
    @if exist *.exe del *.exe
