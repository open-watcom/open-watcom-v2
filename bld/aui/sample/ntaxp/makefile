compile = $(compaxp) $[*
auilib = $(auidir)\ntaxp\aui.lib
resfile = app.res
rcopt = -bt=nt -D__AXP__
addinc = /i$(lang_root)\h\nt
codeview = 1

app.exe : $(objs) app.lnk $(auilib) $(resfile) $(guidir)\win\guintaxp.lib
        $(linker) sys ntaxp_win @app lib $(guidir)\win\guintaxp,nt
        wrc -k $(rcopt) $(resfile) app.exe

!include ..\makefile
