# dipdump Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=dipdump

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>
