compile = $(comp386) $[*
auilib = $(auidir)\nt\aui.lib
resfile = app.res
rcopt = -bt=nt
addinc = /i$(lang_root)\h\nt

app.exe : $(objs) app.lnk $(auilib) $(resfile)
        $(linker) sys nt_win @app lib $(guidir)\win\guintf,nt
        wrc -k $(rcopt) $(resfile) app.exe

!include ..\makefile
