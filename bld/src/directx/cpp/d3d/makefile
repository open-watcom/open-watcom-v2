d3d.exe: d3d.cpp .autodepend
    wcl386 -bt=win95 -l=win95 -zw -5r -oxt -"libpath $(%lib)" d3d.cpp d3d9.lib

clean: .symbolic
    @if exist *.err del *.err
    @if exist *.obj del *.obj
    @if exist *.exe del *.exe
