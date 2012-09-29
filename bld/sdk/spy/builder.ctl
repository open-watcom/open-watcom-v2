# SPY Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wspy

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wini86/wspy.exe    <OWRELROOT>/binw/wspy.exe
    <CCCMD> wini86/wspyhk.dll  <OWRELROOT>/binw/wspyhk.dll
    <CCCMD> nt386/wspy.exe     <OWRELROOT>/binnt/wspy.exe
    <CCCMD> nt386/ntspyhk.dll  <OWRELROOT>/binnt/ntspyhk.dll
    <CCCMD> ntaxp/wspy.exe     <OWRELROOT>/axpnt/wspy.exe
    <CCCMD> ntaxp/ntspyhk.dll  <OWRELROOT>/axpnt/ntspyhk.dll

[ BLOCK . . ]
#============
cdsay <PROJDIR>
