# IDEBAT Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay <projdir>\os2
    wmake -i -h
    cdsay <projdir>\nt
    wmake -i -h
    cdsay <projdir>\dos
    wmake -i -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\idebatch\os2\batserv.exe <relroot>\rel2\binp\batserv.exe
    <CPCMD> <devdir>\idebatch\nt\batserv.exe <relroot>\rel2\binnt\batserv.exe
    ifarch wstrip <devdir>\idebatch\dos\int.exe <relroot>\rel2\binw\dosserv.exe
    <CPCMD> <devdir>\idebatch\dos\batchbox.pif <relroot>\rel2\binw\batchbox.pif

[ BLOCK <1> clean ]
#==================
    sweep killobjs
