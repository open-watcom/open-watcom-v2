dinput.exe: dinput.c .AUTODEPEND
    wcl386 -bt=win95 -l=win95 -zw -5r -oxt -"libpath $(%lib)" dinput.c dinput8.lib dxguid.lib

clean: .SYMBOLIC
    @if exist *.err del *.err
    @if exist *.obj del *.obj
    @if exist *.exe del *.exe
