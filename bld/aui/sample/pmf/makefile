compile = $(comp386) $[* /d2 /w4 /mf/s/fpc
auilib = $(auidir)\pmf\aui.lib
guilib = $(guidir)\win\guipmf.lib

app.exe : $(objs) app.lnk $(auilib) $(guilib)
        $(linker) sys os2v2_pm op symfile=app.sym @app lib $(guilib) lib os2386
        %make apppm.res
        $(%toolkit)\os2bin\rc -p -x apppm.res app.exe
        copy/b app.exe+app.sym

apppm.res : ..\apppm.rc ..\one.bmp &
         ..\two.bmp ..\three.bmp ..\four.bmp ..\five.bmp &
         ..\stop1.bmp ..\stop2.bmp ..\source1.bmp ..\source2.bmp
        @set oinc=$(%include)
        set include=$(watcom_dir)\os220_h;$(%include)
        set opath=$(%path)
        set path=$(%toolkit)\os2bin
        $(%toolkit)\os2bin\rc -r ..\apppm.rc
        copy ..\apppm.res .
        set path=$(%opath)
        @set include=$(%oinc)

!include ..\makefile
