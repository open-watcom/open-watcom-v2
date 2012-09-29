# UI lib Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=ui library

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> qnx/tix/*.tix <OWRELROOT>/qnx/tix/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
