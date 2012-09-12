# IDEBAT Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=idebatch

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2/batserv.exe  <OWRELROOT>/binp/batserv.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/batserv.exe   <OWRELROOT>/binnt/batserv.exe
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos/int.exe      <OWRELROOT>/binw/dosserv.exe
# there is no makefile in the wini86 dir
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/batchbox.pif <OWRELROOT>/binw/batchbox.pif
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> nt/axp/batserv.exe  <OWRELROOT>/axpnt/batserv.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
