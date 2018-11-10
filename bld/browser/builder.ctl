# Browser Builder Control file
# ============================

set PROJNAME=browser

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

# pmake priorities are used to build:
# 1) dlgprs/o
# 2) gen
# 3) everywhere else.
#
# gen is dependent on dlgprs/o
# the os_dos dlgprs/o and gen are dependent on windows.h and not selected.
# brg/* are independent of dlgprs/o and gen

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> brg/dos386/wbrg.exe    <OWRELROOT>/binw/wbrg.exe

    <CCCMD> wini86/wbrw.exe        <OWRELROOT>/binw/wbrw.exe

    <CCCMD> os2386/wbrw.exe        <OWRELROOT>/binp/wbrw.exe
    <CCCMD> brg/os2386/wbrg.exe    <OWRELROOT>/binp/wbrg.exe

    <CCCMD> nt386/wbrw.exe         <OWRELROOT>/binnt/wbrw.exe
    <CCCMD> brg/nt386/wbrg.exe     <OWRELROOT>/binnt/wbrg.exe

    <CCCMD> axpnt/wbrw.exe         <OWRELROOT>/axpnt/wbrw.exe
    <CCCMD> brg/ntaxp/wbrg.exe     <OWRELROOT>/axpnt/wbrg.exe

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
