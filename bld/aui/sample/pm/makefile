compile = $(comp286) $[* /d2 /w4 /ml/zu/s/fpc
auilib = $(auidir)\pm\aui.lib
guilib = $(guidir)\win\guipml.lib

app.exe appj.exe : $(objs) app.lnk $(auilib) $(guilib) apppm.res apppmj.res
        $(linker) sys os2 pm op symfile=app.sym @app lib $(guilib) lib os2
        copy app.exe appj.exe
        $(lang_root)\binp\rc -p -x apppm.res app.exe
        $(lang_root)\binp\rc -p -x apppmj.res appj.exe
        copy/b app.exe+app.sym
        copy/b appj.exe+app.sym

dlgoptn.dlg : ..\h\dlgoptn.dlg
    $(auidir)\bin\parsedlg ..\h\dlgoptn.dlg dlgoptn.dlg

!include ..\makefile
