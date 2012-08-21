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
    <CPCMD> wini86/wspy.exe    <RELROOT>/binw/wspy.exe
    <CPCMD> wini86/wspyhk.dll  <RELROOT>/binw/wspyhk.dll

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wspy.exe     <RELROOT>/binnt/wspy.exe
    <CPCMD> nt386/ntspyhk.dll  <RELROOT>/binnt/ntspyhk.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/wspy.exe     <RELROOT>/axpnt/wspy.exe
    <CPCMD> ntaxp/ntspyhk.dll  <RELROOT>/axpnt/ntspyhk.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
