# Viperdemo Builder Control file
# ==============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay threed\os2
    viperb -r os2_3d.tgt
    wmake -i -h -f project.mk

    cdsay ..\win
    viperb -r win_3d.tgt
    wmake -i -h -f project.mk

    cdsay ..\nt
    viperb -r nt_3d.tgt
    wmake -i -h -f project.mk

    cdsay ..\..\src\os2
    viperb -r drawos2.tgt
    rem copy <devdir>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f drawos2.mk1 <devdir>\viprdemo\src\os2\box.obj
    wmake -i -h -f drawos2.mk1 <devdir>\viprdemo\src\os2\drawroom.obj
    wmake -i -h -f drawos2.mk1 <devdir>\viprdemo\src\os2\furnitu.obj
    rem del wpp386.exe

    cdsay ..\win
    viperb -r draw16.tgt
    rem copy <devdir>\plusplus\bin\rppi8610.exe wpp.exe
    wmake -i -h -f draw16.mk1 <devdir>\viprdemo\src\win\box.obj
    wmake -i -h -f draw16.mk1 <devdir>\viprdemo\src\win\drawroom.obj
    wmake -i -h -f draw16.mk1 <devdir>\viprdemo\src\win\furnitu.obj
    rem del wpp.exe

    cdsay ..\win386
    viperb -r draw.tgt
    rem copy <devdir>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f draw.mk1 <devdir>\viprdemo\src\win386\box.obj
    wmake -i -h -f draw.mk1 <devdir>\viprdemo\src\win386\drawroom.obj
    wmake -i -h -f draw.mk1 <devdir>\viprdemo\src\win386\furnitu.obj
    rem del wpp386.exe

    cdsay ..\win32
    viperb -r draw32.tgt
    rem copy <devdir>\plusplus\bin\rpp38610.exe wpp386.exe
    wmake -i -h -f draw32.mk1 <devdir>\viprdemo\src\win32\box.obj
    wmake -i -h -f draw32.mk1 <devdir>\viprdemo\src\win32\drawroom.obj
    wmake -i -h -f draw32.mk1 <devdir>\viprdemo\src\win32\furnitu.obj
    rem del wpp386.exe
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    cd src
    <CPCMD> *.* <relroot>\rel2\samples\ide\
    <CPCMD> fortran\*.* <relroot>\rel2\samples\ide\fortran\
    <CPCMD> fortran\win32\*.* <relroot>\rel2\samples\ide\fortran\win32\
    <CPCMD> fortran\os2\*.* <relroot>\rel2\samples\ide\fortran\os2\
    <CPCMD> fortran\win\*.* <relroot>\rel2\samples\ide\fortran\win\
    <CPCMD> fortran\win386\*.* <relroot>\rel2\samples\ide\fortran\win386\
    <CPCMD> win32\*.* <relroot>\rel2\samples\ide\win32\
    <CPCMD> os2\*.* <relroot>\rel2\samples\ide\os2\
    <CPCMD> win\*.* <relroot>\rel2\samples\ide\win\
    <CPCMD> win386\*.* <relroot>\rel2\samples\ide\win386\
    cd ..
    <CPCMD> threed\os2\os2_3d.dll <relroot>\rel2\samples\ide\
    <CPCMD> threed\os2\os2_3d.dll <relroot>\rel2\samples\ide\os2\
    <CPCMD> threed\win\win_3d.dll <relroot>\rel2\samples\ide\win\
    <CPCMD> threed\win\win_3d.dll <relroot>\rel2\samples\ide\win386\
    <CPCMD> threed\nt\nt_3d.dll  <relroot>\rel2\samples\ide\win32\

    <CPCMD> threed\os2\os2_3d.dll   <relroot>\rel2\samples\ide\fortran\
    <CPCMD> threed\os2\os2_3d.dll   <relroot>\rel2\samples\ide\fortran\os2\
    <CPCMD> threed\win\win_3d.dll   <relroot>\rel2\samples\ide\fortran\win\
    <CPCMD> threed\win\win_3d.dll   <relroot>\rel2\samples\ide\fortran\win386\
    <CPCMD> threed\nt\nt_3d.dll     <relroot>\rel2\samples\ide\fortran\win32\
    <CPCMD> src\os2\box.obj         <relroot>\rel2\samples\ide\fortran\os2\
    <CPCMD> src\os2\drawroom.obj    <relroot>\rel2\samples\ide\fortran\os2\
    <CPCMD> src\os2\furnitu.obj     <relroot>\rel2\samples\ide\fortran\os2\
    <CPCMD> src\win\box.obj         <relroot>\rel2\samples\ide\fortran\win\
    <CPCMD> src\win\drawroom.obj    <relroot>\rel2\samples\ide\fortran\win\
    <CPCMD> src\win\furnitu.obj     <relroot>\rel2\samples\ide\fortran\win\
    <CPCMD> src\win386\box.obj      <relroot>\rel2\samples\ide\fortran\win386\
    <CPCMD> src\win386\drawroom.obj <relroot>\rel2\samples\ide\fortran\win386\
    <CPCMD> src\win386\furnitu.obj  <relroot>\rel2\samples\ide\fortran\win386\
    <CPCMD> src\win32\box.obj       <relroot>\rel2\samples\ide\fortran\win32\
    <CPCMD> src\win32\drawroom.obj  <relroot>\rel2\samples\ide\fortran\win32\
    <CPCMD> src\win32\furnitu.obj   <relroot>\rel2\samples\ide\fortran\win32\
    cd ..

[ BLOCK <1> clean ]
#==================
    sweep killobjs
