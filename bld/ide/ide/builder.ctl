# IDE Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=ide

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> ../ide.doc      <OWRELROOT>/ide.doc

    <CCCMD> wini86/ide.exe  <OWRELROOT>/binw/
    <CCCMD> win/wsrv.pif    <OWRELROOT>/binw/
    <CCCMD> win/wd.pif      <OWRELROOT>/binw/
    <CCCMD> os2386/ide.exe  <OWRELROOT>/binp/
    <CCCMD> nt386/ide.exe   <OWRELROOT>/binnt/
    <CCCMD> ntaxp/ide.exe   <OWRELROOT>/axpnt/

    <CCCMD> ntx64/ide.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
