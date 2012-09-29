# IDE lib Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=ide library

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>
