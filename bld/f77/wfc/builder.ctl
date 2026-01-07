# WFC Builder Control file
# ========================

set PROJNAME=wfc

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/deftool.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================

    <CCCMD> i86/dos386/wfc.exe          "<OWRELROOT>/binw/"
    <CCCMD> i86/dos386/wfc.sym          "<OWRELROOT>/binw/"
    <CCCMD> 386/dos386/wfc386.exe       "<OWRELROOT>/binw/"
    <CCCMD> 386/dos386/wfc386.sym       "<OWRELROOT>/binw/"

    <CCCMD> i86/os2386/wfc.exe          "<OWRELROOT>/binp/"
    <CCCMD> i86/os2386/wfc.sym          "<OWRELROOT>/binp/"
    <CCCMD> 386/os2386/wfc386.exe       "<OWRELROOT>/binp/"
    <CCCMD> 386/os2386/wfc386.sym       "<OWRELROOT>/binp/"
    <CCCMD> axp/os2386/wfcaxp.exe       "<OWRELROOT>/binp/"

    <CCCMD> i86/nt386/wfc.exe           "<OWRELROOT>/binnt/"
    <CCCMD> i86/nt386/wfc.sym           "<OWRELROOT>/binnt/"
    <CCCMD> 386/nt386/wfc386.exe        "<OWRELROOT>/binnt/"
    <CCCMD> 386/nt386/wfc386.sym        "<OWRELROOT>/binnt/"
    <CCCMD> axp/nt386/wfcaxp.exe        "<OWRELROOT>/binnt/"

    <CCCMD> i86/linux386/wfc.exe        "<OWRELROOT>/binl/wfc"
    <CCCMD> i86/linux386/wfc.sym        "<OWRELROOT>/binl/"
    <CCCMD> 386/linux386/wfc386.exe     "<OWRELROOT>/binl/wfc386"
    <CCCMD> 386/linux386/wfc386.sym     "<OWRELROOT>/binl/"

    <CCCMD> axp/ntaxp/wfcaxp.exe        "<OWRELROOT>/axpnt/"

    <CCCMD> i86/ntx64/wfc.exe           "<OWRELROOT>/binnt64/"
    <CCCMD> i86/ntx64/wfc.sym           "<OWRELROOT>/binnt64/"
    <CCCMD> 386/ntx64/wfc386.exe        "<OWRELROOT>/binnt64/"
    <CCCMD> 386/ntx64/wfc386.sym        "<OWRELROOT>/binnt64/"
    <CCCMD> axp/ntx64/wfcaxp.exe        "<OWRELROOT>/binnt64/"
    <CCCMD> i86/linuxx64/wfc.exe        "<OWRELROOT>/binl64/wfc"
    <CCCMD> i86/linuxx64/wfc.sym        "<OWRELROOT>/binl64/"
    <CCCMD> 386/linuxx64/wfc386.exe     "<OWRELROOT>/binl64/wfc386"
    <CCCMD> 386/linuxx64/wfc386.sym     "<OWRELROOT>/binl64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
