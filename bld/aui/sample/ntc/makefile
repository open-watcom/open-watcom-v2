compile = $(comp386) $[* /d2
auilib = $(auidir)\nt\aui.lib
reslib = $(wres_dir)\lib\wresf.lib
guilib = $(guidir)\ui\guintf.lib
uilib = $(uidir)\nt\uilibf.lib
resfile = appui.res

app.exe : $(objs) app.lnk $(auilib) $(guilib) $(uilib) $(reslib) $(resfile)
        $(linker) sys nt @app lib $(auilib),$(guilib),$(uilib),$(reslib)
        wstrip /q /a /r app.exe . $(resfile)

!include ..\makefile
