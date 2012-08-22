# Viperdemo Builder Control file
# ==============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>

    cdsay src/os2
    echo ide2make -r drawos2.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    ide2make -r drawos2.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    # <CPCMD> <DEVDIR>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f drawos2.mk1  <DEVDIR>/viprdemo/src/os2/box.obj
    wmake -i -h -f drawos2.mk1  <DEVDIR>/viprdemo/src/os2/drawroom.obj
    wmake -i -h -f drawos2.mk1  <DEVDIR>/viprdemo/src/os2/furnitu.obj
    # rm -f wpp386.exe

    cdsay ../win
    echo ide2make -r draw16.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    ide2make -r draw16.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    # <CPCMD> <DEVDIR>\plusplus\bin\rppi8610.exe wpp.exe
    wmake -i -h -f draw16.mk1   <DEVDIR>/viprdemo/src/win/box.obj
    wmake -i -h -f draw16.mk1   <DEVDIR>/viprdemo/src/win/drawroom.obj
    wmake -i -h -f draw16.mk1   <DEVDIR>/viprdemo/src/win/furnitu.obj
    # rm -f wpp.exe

    cdsay ../win386
    echo ide2make -r draw.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    ide2make -r draw.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    # <CPCMD> <DEVDIR>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f draw.mk1     <DEVDIR>/viprdemo/src/win386/box.obj
    wmake -i -h -f draw.mk1     <DEVDIR>/viprdemo/src/win386/drawroom.obj
    wmake -i -h -f draw.mk1     <DEVDIR>/viprdemo/src/win386/furnitu.obj
    # rm -f wpp386.exe

    cdsay ../win32
    echo ide2make -r draw.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    ide2make -r draw32.tgt -c <DEVDIR>/viper/cfg/ide.cfg
    # <CPCMD> <DEVDIR>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f draw32.mk1   <DEVDIR>/viprdemo/src/win32/box.obj
    wmake -i -h -f draw32.mk1   <DEVDIR>/viprdemo/src/win32/drawroom.obj
    wmake -i -h -f draw32.mk1   <DEVDIR>/viprdemo/src/win32/furnitu.obj
    # rm -f wpp386.exe
    cd <PROJDIR>

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    cd src
    <CPCMD> ./*.*                   <RELROOT>/samples/ide/
    <CPCMD> fortran/*.*             <RELROOT>/samples/ide/fortran/
    <CPCMD> fortran/win32/*.*       <RELROOT>/samples/ide/fortran/win32/
    <CPCMD> fortran/os2/*.*         <RELROOT>/samples/ide/fortran/os2/
    <CPCMD> fortran/win/*.*         <RELROOT>/samples/ide/fortran/win/
    <CPCMD> fortran/win386/*.*      <RELROOT>/samples/ide/fortran/win386/
    <CPCMD> win32/*.tgt             <RELROOT>/samples/ide/win32/
    <CPCMD> os2/*.tgt               <RELROOT>/samples/ide/os2/
    <CPCMD> win/*.tgt               <RELROOT>/samples/ide/win/
    <CPCMD> win386/*.tgt            <RELROOT>/samples/ide/win386/
    cd ..
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> threed/os2/os2_3d.dll   <RELROOT>/samples/ide/
    <CPCMD> threed/os2/os2_3d.dll   <RELROOT>/samples/ide/os2/
    <CPCMD> threed/os2/os2_3d.dll   <RELROOT>/samples/ide/fortran/
    <CPCMD> threed/os2/os2_3d.dll   <RELROOT>/samples/ide/fortran/os2/
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> threed/nt/nt_3d.dll     <RELROOT>/samples/ide/win32/
    <CPCMD> threed/nt/nt_3d.dll     <RELROOT>/samples/ide/fortran/win32/
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> threed/win/win_3d.dll   <RELROOT>/samples/ide/win/
    <CPCMD> threed/win/win_3d.dll   <RELROOT>/samples/ide/win386/
    <CPCMD> threed/win/win_3d.dll   <RELROOT>/samples/ide/fortran/win/
    <CPCMD> threed/win/win_3d.dll   <RELROOT>/samples/ide/fortran/win386/
  [ ENDIF ]
    <CPCMD> src/os2/box.obj         <RELROOT>/samples/ide/fortran/os2/
    <CPCMD> src/os2/drawroom.obj    <RELROOT>/samples/ide/fortran/os2/
    <CPCMD> src/os2/furnitu.obj     <RELROOT>/samples/ide/fortran/os2/
    <CPCMD> src/win/box.obj         <RELROOT>/samples/ide/fortran/win/
    <CPCMD> src/win/drawroom.obj    <RELROOT>/samples/ide/fortran/win/
    <CPCMD> src/win/furnitu.obj     <RELROOT>/samples/ide/fortran/win/
    <CPCMD> src/win386/box.obj      <RELROOT>/samples/ide/fortran/win386/
    <CPCMD> src/win386/drawroom.obj <RELROOT>/samples/ide/fortran/win386/
    <CPCMD> src/win386/furnitu.obj  <RELROOT>/samples/ide/fortran/win386/
    <CPCMD> src/win32/box.obj       <RELROOT>/samples/ide/fortran/win32/
    <CPCMD> src/win32/drawroom.obj  <RELROOT>/samples/ide/fortran/win32/
    <CPCMD> src/win32/furnitu.obj   <RELROOT>/samples/ide/fortran/win32/
    cd ..

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    wmake -i -h -f clean.mif

[ BLOCK . . ]
#============

cdsay <PROJDIR>
