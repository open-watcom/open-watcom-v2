# ORL Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    <CCCMD> <PROJDIR>/test/nt386/objread.exe    <RELROOT>/binnt/objread.exe
    <CCCMD> <PROJDIR>/test/os2386/objread.exe   <RELROOT>/binp/objread.exe
    <CCCMD> <PROJDIR>/test/linux386/objread.exe <RELROOT>/binl/objread

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
