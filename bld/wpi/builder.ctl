# wpi lib Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wpi library

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>
