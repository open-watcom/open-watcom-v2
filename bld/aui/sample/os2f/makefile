compile = $(comp386) $[* /d2 /mf
auilib = $(auidir)\of\aui.lib
guilib = $(guidir)\ui\guios2f.lib
uilib = $(uidir)\os2\uilibf.lib
reslib = $(wres_dir)\lib\wresf.lib
resfile = appui.res

app.exe : $(objs) app.lnk $(auilib) $(guilib) $(uilib) $(reslib) $(resfile)
        wlink sys os2v2 @app lib $(auilib),$(guilib),$(uilib),$(reslib)
        wstrip /q /a /r app.exe . $(resfile)

!include ..\makefile
