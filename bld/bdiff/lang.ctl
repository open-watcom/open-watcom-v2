# bpatch Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake /h /i bpatch.exe
    # Make BPATCH.QNX for Database guys. Easier than getting them set up
    wmake /h bpatch.qnx

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\bdiff\bpatch.exe <relroot>\rel2\binw\bpatch.exe

[ BLOCK <1> clean ]
#==================
    wmake /h clean
