# UNINSTAL Builder Control file
# =============================

set PROJNAME=uninstal

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> nt386/uninstal.exe <OWRELROOT>/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
