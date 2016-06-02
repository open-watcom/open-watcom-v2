# IDE Builder Control file
# ========================

set PROJNAME=ide

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> ../ide.doc      <OWRELROOT>/ide.doc

    <CCCMD> wini86/ide.exe  <OWRELROOT>/binw/
    <CCCMD> wini86/ide.sym  <OWRELROOT>/binw/
    <CCCMD> win/wsrv.pif    <OWRELROOT>/binw/
    <CCCMD> win/wd.pif      <OWRELROOT>/binw/
    <CCCMD> os2386/ide.exe  <OWRELROOT>/binp/
    <CCCMD> os2386/ide.sym  <OWRELROOT>/binp/
    <CCCMD> nt386/ide.exe   <OWRELROOT>/binnt/
    <CCCMD> nt386/ide.sym   <OWRELROOT>/binnt/
    <CCCMD> ntaxp/ide.exe   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/ide.sym   <OWRELROOT>/axpnt/

    <CCCMD> ntx64/ide.exe   <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
