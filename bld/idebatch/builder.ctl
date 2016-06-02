# IDEBAT Builder Control file
# ===========================

set PROJNAME=idebatch

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> os2386/batserv.exe  <OWRELROOT>/binp/
    <CCCMD> nt386/batserv.exe   <OWRELROOT>/binnt/
    <CCCMD> dosi86/int.exe      <OWRELROOT>/binw/dosserv.exe
# there is no makefile in the wini86 dir
    <CPCMD> win/batchbox.pif    <OWRELROOT>/binw/
    <CCCMD> ntaxp/batserv.exe   <OWRELROOT>/axpnt/

    <CCCMD> ntx64/batserv.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
