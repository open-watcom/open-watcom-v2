# EditDLL Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay epm
    wmake /h /i
    cdsay ..\cw
    wmake /h /i
    cdsay ..\viw\objwin
    wmake /h /i
    cdsay ..\objnt
    wmake /h /i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\editdll\epm\epmlink.dll  <relroot>\rel2\binp\dll\
    <CPCMD> <devdir>\editdll\viw\objwin\weditviw.dll <relroot>\rel2\binw\weditviw.dll
    <CPCMD> <devdir>\editdll\viw\objnt\weditviw.dll <relroot>\rel2\binnt\weditviw.dll

    <CPCMD> <devdir>\editdll\cw\weditcw.dll <relroot>\rel2\binw\weditcw.dll
    <CPCMD> <devdir>\editdll\cw\weditcwn.dll <relroot>\rel2\binnt\weditcw.dll
    <CPCMD> <devdir>\editdll\cw\weditcw.c <relroot>\rel2\src\editdll\weditcw.c
    <CPCMD> <devdir>\editdll\cw\weditcw.h <relroot>\rel2\src\editdll\weditcw.h
    <CPCMD> <devdir>\editdll\cw\weditcw.lnk <relroot>\rel2\src\editdll\weditcw.lnk
    <CPCMD> <devdir>\editdll\cw\weditcwn.lnk <relroot>\rel2\src\editdll\weditcwn.lnk
    <CPCMD> <devdir>\editdll\cw\makefile <relroot>\rel2\src\editdll\makefile
    <CPCMD> <devdir>\editdll\wedit.h <relroot>\rel2\src\editdll\wedit.h
    <CPCMD> <devdir>\editdll\wedit.doc <relroot>\rel2\src\editdll\wedit.doc

[ BLOCK <1> clean ]
#==================
    sweep killobjs
