# RCSDLL Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay OBJ286
    wmake /h /i
    cdsay ..\OBJ386
    wmake /h /i
    cdsay ..\OBJOS2
    wmake /h /i
    cdsay ..\OBJNT
    wmake /h /i
    cdsay ..\OBJWIN
    wmake /h /i
    cdsay ..\OBJQNX
    wmake /h /i

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\rcsdll\objos2\rcsdll.dll <relroot>\rel2\binp\dll\rcsdll.dll
    <CPCMD> <devdir>\rcsdll\objwin\rcsdll.dll <relroot>\rel2\binw\rcsdll.dll
    <CPCMD> <devdir>\rcsdll\objnt\rcsdll.dll <relroot>\rel2\binnt\rcsdll.dll
#    <CPCMD> <devdir>\rcsdll\objaxp\rcsdll.dll <relroot>\rel2\axpnt\rcsdll.dll
    copy <devdir>\rcsdll\bat\*.bat <relroot>\rel2\binw\
    copy <devdir>\rcsdll\cmd\*.cmd <relroot>\rel2\binp\

[ BLOCK <1> clean ]
#==================
    sweep killobjs


