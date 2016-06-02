# TECHINFO Builder Control file
# =============================

set PROJNAME=techinfo

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/techinfo.exe <OWRELROOT>/binw/
    <CCCMD> os2i86/techinfo.exe <OWRELROOT>/binp/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
