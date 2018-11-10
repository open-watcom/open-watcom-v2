# MISC Builder Control file
# =========================

set PROJNAME=misc

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================

    <CPCMD> watcom.ico              <OWRELROOT>/
    <CPCMD> <OWROOT>/license.txt    <OWRELROOT>/

    <CCCMD> nt386/unicode*          <OWRELROOT>/binnt/
    <CCCMD> ntx64/unicode*          <OWRELROOT>/binnt64/
    <CCCMD> os2386/unicode*         <OWRELROOT>/binp/
    <CCCMD> linux386/unicode*       <OWRELROOT>/binl/
    <CCCMD> linuxx64/unicode*       <OWRELROOT>/binl64/
    <CCCMD> wini86/unicode*         <OWRELROOT>/binw/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
