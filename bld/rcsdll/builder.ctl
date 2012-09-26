# RCSDLL Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> wini86/rcsdll.dll  <OWRELROOT>/binw/rcsdll.dll
    <CPCMD> bat/*.bat          <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/rcsdll.dll  <OWRELROOT>/binp/dll/rcsdll.dll
    <CPCMD> cmd/*.cmd          <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/rcsdll.dll   <OWRELROOT>/binnt/rcsdll.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/rcsdll.dll   <OWRELROOT>/axpnt/rcsdll.dll

[ BLOCK . . ]
#============
cdsay <PROJDIR>
