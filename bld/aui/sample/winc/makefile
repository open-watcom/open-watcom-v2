compile = $(comp286) $[* /d2 /ml /zW
auilib = $(auidir)\win\aui.lib
guilib = $(guidir)\ui\guiwinl.lib
uilib = $(uidir)\win\uilibl.lib
reslib = $(watcomdir)\lib\wreslw.lib
resfile = appui.res

app.exe : $(objs) app.lnk $(auilib) $(guilib) $(uilib) $(resfile) $(reslib)
        $(linker) sys windows debug all @app lib $(guilib),$(uilib),$(auilib),$(reslib) import DEATH GDI.121 import RESURRECTION GDI.122
        wstrip /q /a /r app.exe . $(resfile)

!include ..\makefile
