# wcl Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> <devdir>\cc\wcl\wcl.exe <relroot>\rel2\binw\wcl.exe
    <CPCMD> <devdir>\cc\wcl\wcl386.exe <relroot>\rel2\binw\wcl386.exe
    <CPCMD> <devdir>\cc\wcl\wclnt.exe <relroot>\rel2\binnt\wcl.exe
    <CPCMD> <devdir>\cc\wcl\wcl386nt.exe <relroot>\rel2\binnt\wcl386.exe
#   <CPCMD> <devdir>\cc\wcl\wclaxpnt.exe <relroot>\rel2\binnt\wclaxp.exe
#   <CPCMD> <devdir>\cc\wcl\wclaxp.exe <relroot>\rel2\binw\wclaxp.exe
#   <CPCMD> <devdir>\cc\wcl\ntaxp\wclaxp.exe <relroot>\rel2\axpnt\wclaxp.exe
#   <CPCMD> <devdir>\cc\wcl\ntaxp\wcl386.exe <relroot>\rel2\axpnt\wcl386.exe
#   <CPCMD> <devdir>\cc\wcl\ntaxp\wcl.exe <relroot>\rel2\axpnt\wcl.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
