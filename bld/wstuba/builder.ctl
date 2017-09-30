# WSTUBA Builder Control file
# ===========================

set PROJNAME=wstub

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binw/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binw/wstubq.exe
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binnt/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binnt/wstubq.exe
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binp/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binp/wstubq.exe
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binl/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binl/wstubq.exe
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binnt64/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binnt64/wstubq.exe
    <CCCMD> dos386/wstub.exe    <OWRELROOT>/binl64/wstub.exe
    <CCCMD> dos386/wstubq.exe   <OWRELROOT>/binl64/wstubq.exe

    <CCCMD> wstub.asm           <OWRELROOT>/src/wstub.asm

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
