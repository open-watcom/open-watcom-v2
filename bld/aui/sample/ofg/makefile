compile = $(comp386) $[* /d2 /mf /dDOS
auilib = $(auidir)\of\aui.lib

app.exe : $(objs) app.lnk $(auilib)
        wlink sys dos4g @app lib $(guidir)\ui\guidosf,$(uidir)\dos\uilibf

!include ..\makefile
