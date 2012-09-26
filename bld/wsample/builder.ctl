# WSAMPLE Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wsample

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/wsample.exe      <OWRELROOT>/binw/wsample.exe
    <CCCMD> rsii86/wsamprsi.exe     <OWRELROOT>/binw/wsamprsi.exe
    <CCCMD> pls386/wsamppls.exp     <OWRELROOT>/binw/wsamppls.exp
    <CPCMD> wini86/wsamplew.exe     <OWRELROOT>/binw/wsamplew.exe

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> os2i86/wsampos2.exe     <OWRELROOT>/binp/wsampos2.exe
    <CPCMD> os2386/wsmpos22.exe     <OWRELROOT>/binp/wsample.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wsmpnt.exe        <OWRELROOT>/binnt/wsample.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wsample.exe    <OWRELROOT>/binl/wsample

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wsmpaxp.exe       <OWRELROOT>/axpnt/wsample.exe

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> nlm/wsample.nlm         <OWRELROOT>/nlm/wsample.nlm

[ BLOCK . . ]
#============
cdsay <PROJDIR>
