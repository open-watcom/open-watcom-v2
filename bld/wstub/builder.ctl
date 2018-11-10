# WSTUB Builder Control file
# ==========================

set PROJNAME=wstub

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/*.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/*.exe        <OWRELROOT>/binnt/
    <CCCMD> dos386/*.exe        <OWRELROOT>/binp/
    <CCCMD> dos386/*.exe        <OWRELROOT>/binl/
    <CCCMD> dos386/*.exe        <OWRELROOT>/binnt64/
    <CCCMD> dos386/*.exe        <OWRELROOT>/binl64/

    <CCCMD> wstub.asm           <OWRELROOT>/src/wstub.asm

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
