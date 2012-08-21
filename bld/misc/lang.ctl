# MISC Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

    <CPCMD> unicode*                <RELROOT>/binw/
    <CPCMD> watcom.ico              <RELROOT>/
    <CPCMD> unicode*                <RELROOT>/binl/
    <CPCMD> <OWROOT>/license.txt    <RELROOT>/license.txt

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
