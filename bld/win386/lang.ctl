# WIN386 Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay conv
    wmake /h /i
    cdsay ..\ext
    wmake /h /i
    cdsay ..\dll
    wmake /h /i
    cdsay ..\wbind
    wmake /h /i
    cdsay ..\vxd
    wmake /h /i
    cdsay ..

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> ext\win386.ext <relroot>\rel2\binw\win386.ext
    <CPCMD> dll\w386dll.ext <relroot>\rel2\binw\w386dll.ext
    <CPCMD> conv\win386.lib <relroot>\rel2\lib386\win\win386.lib
    <CPCMD> vxd\wdebug.386 <relroot>\rel2\binw\wdebug.386
    <CPCMD> vxd\wemu387.386 <relroot>\rel2\binw\wemu387.386
    <CPCMD> wbind\wbind.exe <relroot>\rel2\binw\wbind.exe
    <CPCMD> wbind\wbindnt.exe <relroot>\rel2\binnt\wbind.exe

[ BLOCK <1> clean ]
#==================
    sweep killobjs
