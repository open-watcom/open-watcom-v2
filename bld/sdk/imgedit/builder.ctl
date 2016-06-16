# IMAGE EDITOR Builder Control file
# =================================

set PROJNAME=wimgedit

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wimgedit.exe <OWRELROOT>/binw/
    <CCCMD> nt386/wimgedit.exe  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wimgedit.exe  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wimgedit.exe  <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
