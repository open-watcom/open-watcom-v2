# IMAGE EDITOR Builder Control file
# =================================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
    <CPCMD> <devdir>\sdk\imgedit\wini86\wimgedit.exe <relroot>\rel2\binw\
    <CPCMD> <devdir>\sdk\imgedit\nt386\wimgedit.exe <relroot>\rel2\binnt\
#    <CPCMD> <devdir>\sdk\imgedit\ntaxp\wimgedit.exe <relroot>\rel2\axpnt\

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
