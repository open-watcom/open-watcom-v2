# IDEBAT Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=idebatch

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> os2386/batserv.exe  <OWRELROOT>/binp/batserv.exe
    <CCCMD> nt386/batserv.exe   <OWRELROOT>/binnt/batserv.exe
    <CCCMD> dosi86/int.exe      <OWRELROOT>/binw/dosserv.exe
# there is no makefile in the wini86 dir
    <CPCMD> win/batchbox.pif 	<OWRELROOT>/binw/batchbox.pif
    <CCCMD> ntaxp/batserv.exe   <OWRELROOT>/axpnt/batserv.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
