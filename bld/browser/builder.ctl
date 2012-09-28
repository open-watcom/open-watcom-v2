# Browser Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=browser

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

# pmake priorities are used to build:
# 1) dlgprs/o
# 2) gen
# 3) everywhere else.
#
# gen is dependent on dlgprs/o
# the os_dos dlgprs/o and gen are dependent on windows.h and not selected.
# brg/* are independent of dlgprs/o and gen

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> brg/dos386/wbrg.exe    <OWRELROOT>/binw/wbrg.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wbrw.exe        <OWRELROOT>/binw/wbrw.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wbrw.exe        <OWRELROOT>/binp/wbrw.exe
    <CPCMD> brg/os2386/wbrg.exe    <OWRELROOT>/binp/wbrg.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wbrw.exe         <OWRELROOT>/binnt/wbrw.exe
    <CPCMD> brg/nt386/wbrg.exe     <OWRELROOT>/binnt/wbrg.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axpnt/wbrw.exe         <OWRELROOT>/axpnt/wbrw.exe
    <CPCMD> brg/ntaxp/wbrg.exe     <OWRELROOT>/axpnt/wbrg.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
