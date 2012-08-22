# WSAMPLE Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
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

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
