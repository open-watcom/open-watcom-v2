# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>
set PROJNAME=restest

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================

[ BLOCK . . ]
#============
cdsay <PROJDIR>
