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

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/ide.exe  <OWRELROOT>/binw/
    <CPCMD> win/wsrv.pif    <OWRELROOT>/binw/
    <CPCMD> win/wd.pif      <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/ide.exe  <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/ide.exe   <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp) <2*> ] 
    <CPCMD> ntaxp/ide.exe   <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
