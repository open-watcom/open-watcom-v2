compile = $(comp286) $[* /d2 /ml /dDOS
auilib  = $(auidir)\ol\aui.lib
guilib  = $(guidir)\ui\guidosl.lib
guilib2 = $(guidir)\ui\guios2l.lib
reslib = $(wres_dir)\lib\wresl.lib
resfile = appui

app.exe appj.exe : $(objs) app.lnk $(auilib) $(reslib) $(guilib2) $(resfile).res $(resfile)j.res
        $(linker) op stack=12k name app sys os2 @app &
                lib $(reslib),$(guilib2),$(uidir)\os2\uilibl,wresl
        copy app.exe appj.exe
        wstrip /q /a /r app.exe . $(resfile).res
        wstrip /q /a /r appj.exe . $(resfile)j.res

appdos.exe : $(objs) app.lnk $(auilib) $(reslib) $(guilib) $(resfile).res $(resfile)j.res
        $(linker) op stack=12k name appdos sys dos @app &
                lib $(reslib),$(guilib),$(uidir)\dos\uilibl
        wstrip /q /a /r appdos.exe . $(resfile).res

!include ..\makefile
