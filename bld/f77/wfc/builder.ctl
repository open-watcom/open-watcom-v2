# WFC Builder Control file
# ========================

set PROJNAME=wfc

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

    <CCCMD> i86/dos386/wfc.exe          <OWRELROOT>/binw/wfc.exe
    <CCCMD> i86/dos386/wfc.sym          <OWRELROOT>/binw/wfc.sym
    <CCCMD> 386/dos386/wfc386.exe       <OWRELROOT>/binw/wfc386.exe
    <CCCMD> 386/dos386/wfc386.sym       <OWRELROOT>/binw/wfc386.sym

    <CCCMD> i86/os2386/wfc.exe          <OWRELROOT>/binp/wfc.exe
    <CCCMD> i86/os2386/wfc.sym          <OWRELROOT>/binp/wfc.sym
    <CCCMD> 386/os2386/wfc386.exe       <OWRELROOT>/binp/wfc386.exe
    <CCCMD> 386/os2386/wfc386.sym       <OWRELROOT>/binp/wfc386.sym
#    <CCCMD> axp/os2386/wfcaxp.exe       <OWRELROOT>/binp/wfcaxp.exe

    <CCCMD> i86/nt386/wfc.exe           <OWRELROOT>/binnt/wfc.exe
    <CCCMD> i86/nt386/wfc.sym           <OWRELROOT>/binnt/wfc.sym
    <CCCMD> 386/nt386/wfc386.exe        <OWRELROOT>/binnt/wfc386.exe
    <CCCMD> 386/nt386/wfc386.sym        <OWRELROOT>/binnt/wfc386.sym
#    <CCCMD> axp/nt386/wfcaxp.exe        <OWRELROOT>/binnt/wfcaxp.exe

    <CCCMD> i86/linux386/wfc.exe        <OWRELROOT>/binl/wfc
    <CCCMD> i86/linux386/wfc.sym        <OWRELROOT>/binl/wfc.sym
    <CCCMD> 386/linux386/wfc386.exe     <OWRELROOT>/binl/wfc386
    <CCCMD> 386/linux386/wfc386.sym     <OWRELROOT>/binl/wfc386.sym

    <CCCMD> axp/ntaxp/wfcaxp.exe        <OWRELROOT>/axpnt/wfcaxp.exe

    <CCCMD> i86/ntx64/wfc.exe           <OWRELROOT>/binnt64/wfc.exe
    <CCCMD> i86/ntx64/wfc.sym           <OWRELROOT>/binnt64/wfc.sym
    <CCCMD> 386/ntx64/wfc386.exe        <OWRELROOT>/binnt64/wfc386.exe
    <CCCMD> 386/ntx64/wfc386.sym        <OWRELROOT>/binnt64/wfc386.sym
#    <CCCMD> axp/ntx64/wfcaxp.exe        <OWRELROOT>/binnt64/wfcaxp.exe
    <CCCMD> i86/linuxx64/wfc.exe        <OWRELROOT>/binl64/wfc
    <CCCMD> 386/linuxx64/wfc386.exe     <OWRELROOT>/binl64/wfc386

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
