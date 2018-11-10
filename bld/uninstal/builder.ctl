# UNINSTAL Builder Control file
# =============================

set PROJNAME=uninstal

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> nt386/uninstal.exe <OWRELROOT>/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
