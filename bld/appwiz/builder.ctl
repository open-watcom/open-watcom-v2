# Open Watcom Application Wizard control file
#============================================

set PROJDIR=<CWD>
set PROJNAME=appwiz

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>
