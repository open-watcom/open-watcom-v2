# DRWATCOM Builder Control file
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
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> win/wini86/drwatcom.exe  <OWRELROOT>/binw/drwatcom.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/nt386/drwatcom.exe    <OWRELROOT>/binnt/drwatcom.exe
    <CPCMD> nt/win95/drwatcom.exe    <OWRELROOT>/binnt/_drwin95.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> nt/ntaxp/drwatcom.exe    <OWRELROOT>/axpnt/drwatcom.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
