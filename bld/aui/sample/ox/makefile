compile = $(comp286) $[* /d1 /oaxt /ml
auilib = $(auidir)\ox\aui.lib

!ifdef %os2box
target=app2.exe
!else
target=app.exe
!endif

all : .symbolic $(target)
        @%null

app2.exe : $(objs) app.lnk $(auilib)
        wlink op stack=12k name app2 sys os2 pmc @app lib $(guidir)\ui\guios2l lib $(uidir)\os2\uilibl

app.exe : $(objs) app.lnk $(auilib)
        wlink sys dos @app lib $(guidir)\ui\guidosl lib $(uidir)\dos\uilibl

!include ..\makefile
