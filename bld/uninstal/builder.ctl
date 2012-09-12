# UNINSTAL Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> <2> rel cprel ]
#==========================
    [ IFDEF (os_nt "") <2*> ]
      <CPCMD> nt386/uninstal.exe <OWRELROOT>/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
