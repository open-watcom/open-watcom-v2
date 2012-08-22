# SPY Builder Control file
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
    <CPCMD> wini86/wspy.exe    <OWRELROOT>/binw/wspy.exe
    <CPCMD> wini86/wspyhk.dll  <OWRELROOT>/binw/wspyhk.dll

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wspy.exe     <OWRELROOT>/binnt/wspy.exe
    <CPCMD> nt386/ntspyhk.dll  <OWRELROOT>/binnt/ntspyhk.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wspy.exe     <OWRELROOT>/axpnt/wspy.exe
    <CPCMD> ntaxp/ntspyhk.dll  <OWRELROOT>/axpnt/ntspyhk.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
