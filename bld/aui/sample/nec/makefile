compile = $(comp386) $[* /d2 /mf /dDOS /zk0
auilib = $(auidir)\of\aui.lib
reslib = $(wres_dir)\lib\wresf.lib
guilib = $(guidir)\ui\guidosf.lib
uilib = $(uidir)\necdos\uilibf.lib
resfile = appui.res


appr.exe : $(objs) app.lnk $(guilib) $(uilib) $(reslib) $(auilib) $(resfile)
        *wlink name appr sys dos4g @app lib $(guilib), $(uilib), $(reslib), $(auilib)
        wstrip /q /a /r appr.exe . $(resfile)

app.exp : $(objs) app.lnk $(guilib) $(uilib) $(reslib) $(auilib) $(resfile)
        wlink sys pharlap @app lib $(guilib), $(uilib), $(reslib), $(auilib)
        !$(pharlap)bind386 $(pharlap)run386b app
        wstrip /q /a /r app.exe . $(resfile)

!include ..\makefile
