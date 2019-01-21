# WDE Builder Control file
# ========================

set PROJNAME=wde

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/wde.exe      <OWRELROOT>/binw/
    <CCCMD> wini86/wde.sym      <OWRELROOT>/binw/
    <CCCMD> nt386/wde.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386/wde.sym       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wde.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wde.sym       <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wde.exe       <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
