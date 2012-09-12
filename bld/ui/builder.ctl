# UI lib Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=ui library

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx/tix/*.tix <OWRELROOT>/qnx/tix/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
