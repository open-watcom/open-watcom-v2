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
    <CPCMD> dosi86/wsample.exe      <RELROOT>/binw/wsample.exe
    <CCCMD> rsii86/wsamprsi.exe     <RELROOT>/binw/wsamprsi.exe
    <CCCMD> pls386/wsamppls.exp     <RELROOT>/binw/wsamppls.exp
    <CPCMD> wini86/wsamplew.exe     <RELROOT>/binw/wsamplew.exe

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> os2i86/wsampos2.exe     <RELROOT>/binp/wsampos2.exe
    <CPCMD> os2386/wsmpos22.exe     <RELROOT>/binp/wsample.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wsmpnt.exe        <RELROOT>/binnt/wsample.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wsample.exe    <RELROOT>/binl/wsample

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wsmpaxp.exe       <RELROOT>/axpnt/wsample.exe

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> nlm/wsample.nlm         <RELROOT>/nlm/wsample.nlm

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
