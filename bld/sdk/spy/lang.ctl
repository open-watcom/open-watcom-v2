# SPY Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
    <CPCMD> <devdir>\sdk\spy\wini86\wspy.exe <relroot>\rel2\binw\wspy.exe
    <CPCMD> <devdir>\sdk\spy\wini86\wspyhk.dll <relroot>\rel2\binw\wspyhk.dll
    <CPCMD> <devdir>\sdk\spy\nt386\wspy.exe <relroot>\rel2\binnt\wspy.exe
    <CPCMD> <devdir>\sdk\spy\nt386\ntspyhk.dll <relroot>\rel2\binnt\ntspyhk.dll
#    <CPCMD> <devdir>\sdk\spy\ntaxp\wspy.exe <relroot>\rel2\axpnt\wspy.exe
#    <CPCMD> <devdir>\sdk\spy\ntaxp\ntspyhk.dll <relroot>\rel2\axpnt\wspyhk.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
