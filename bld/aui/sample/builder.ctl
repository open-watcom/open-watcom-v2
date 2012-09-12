# AUI Sample Builder Control file
# ===============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay .

[ BLOCK <1> tests ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel cprel ]
#======================

[ BLOCK . . ]
#============
cdsay <PROJDIR>
