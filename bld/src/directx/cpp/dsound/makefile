dsound.exe: dsound.cpp .autodepend
    wcl386 -bt=win95 -l=win95 -zw -5r -oxt -"libpath $(%lib)" dsound.cpp dsound.lib dxguid.lib

clean: .symbolic
    @if exist *.err del *.err
    @if exist *.obj del *.obj
    @if exist *.exe del *.exe
