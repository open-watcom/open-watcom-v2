compile = $(comp286) $[* /d1 /mm /oax
auilib = $(auidir)\ol\aui.lib

!ifdef %os2box
target=app2.exe
!else
target=app.exe
!endif

all : .symbolic $(target)
        @%null

app2.exe : $(objs) app.lnk $(auilib)
        wlink op stack=12k name app2 sys os2 pmc @app lib $(guidir)\dos\guios2l

app.exe : $(objs) app.lnk $(auilib)
        wlink sys dos @app lib $(guidir)\dos\guidosl

!include ..\makefile
