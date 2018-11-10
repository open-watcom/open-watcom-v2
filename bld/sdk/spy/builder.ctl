# SPY Builder Control file
# ============================

set PROJNAME=wspy

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/wspy.exe    <OWRELROOT>/binw/
    <CCCMD> wini86/wspyhk.dll  <OWRELROOT>/binw/
    <CCCMD> nt386/wspy.exe     <OWRELROOT>/binnt/
    <CCCMD> nt386/ntspyhk.dll  <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wspy.exe     <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/ntspyhk.dll  <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wspy.exe     <OWRELROOT>/binnt64/
    <CCCMD> ntx64/ntspyhk.dll  <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
