# WGML Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> wgml ]
#=================
    pmake -d wgml <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK . . ]
#============
cdsay <PROJDIR>
